#ifndef Fit_hh_
#define Fit_hh_

#include "Main/inc/Configs.hh"
#include "Main/inc/Observable.hh"
#include "Main/inc/Component.hh"

namespace roofitter {

  struct CutConfig {
    fhicl::Atom<std::string> name{fhicl::Name("name"), fhicl::Comment("Cut name")};
    fhicl::Atom<std::string> leaf{fhicl::Name("leaf"), fhicl::Comment("Leaf of this tree cut")};
    fhicl::Atom<bool> invert{fhicl::Name("invert"), fhicl::Comment("Set to true if you want to invert the cut"), false};
  };

  struct FitConfig {
    fhicl::Atom<std::string> name{fhicl::Name("name"), fhicl::Comment("Fit name")};
    fhicl::Sequence< fhicl::Table<ObservableConfig> > observables{fhicl::Name("observables"), fhicl::Comment("List of observables")};
    fhicl::Sequence< fhicl::Table<ComponentConfig> > components{fhicl::Name("components"), fhicl::Comment("List of components")};
    fhicl::Sequence< fhicl::Table<CutConfig> > cuts{fhicl::Name("cuts"), fhicl::Comment("List of cuts to apply")};
    fhicl::Table<PdfConfig> model{fhicl::Name("model"), fhicl::Comment("The PDF for the full final model to fit")};
    fhicl::Atom<bool> unfold{fhicl::Name("unfold"), fhicl::Comment("Set to tru if you want to unfold the efficiency and response effects"), false};
  };

  class Fit {
  private:
    FitConfig _fitConf;

    Observables _observables;
    Components _components;

    TH1* _hist;
    RooFitResult* _fitResult;

  public:
    Fit(const FitConfig& cfg, RooWorkspace* ws) : _fitConf(cfg) {

      // Construct the observables
      if (_fitConf.observables().size() > 2) {
	throw cet::exception("Fit Constructor") << "More than 2 observables is not currently supported";
      }
      for (const auto& i_obs_cfg : _fitConf.observables()) {
	Observable i_obs(i_obs_cfg, ws);
	_observables.push_back(i_obs);
      }

      // Construct the components
      for (const auto& i_comp_cfg : _fitConf.components()) {
	Component i_comp(i_comp_cfg, ws, _observables);
	_components.push_back(i_comp);
      }

      // Construct the final model
      std::stringstream factory_cmd;
      factory_cmd.str("");
      factory_cmd << _fitConf.model().formula();
      ws->factory(factory_cmd.str().c_str());
    }

    TCut cutcmd() { 
      TCut result;
      for (const auto& i_cut_cfg : _fitConf.cuts()) {
	if (i_cut_cfg.invert()) {
	  result += !TCut(i_cut_cfg.leaf().c_str());
	}
	else {
	  result += TCut(i_cut_cfg.leaf().c_str());
	}
      }
      return result;
    }

    void fillData(TTree* tree, RooWorkspace* ws) {

      RooArgSet vars;
      RooRealVar* x_var = 0;
      RooRealVar* y_var = 0;
      std::string x_leaf = "";
      std::string y_leaf = "";
      for (const auto& i_obs : _observables) {
	const auto& i_obs_conf = i_obs.getConf();
	RooRealVar* var = ws->var(i_obs_conf.name().c_str());
	vars.add(*var);

	if (x_leaf.empty()) {
	  x_leaf = i_obs_conf.leaf();
	  x_var = var;
	}
	else if (y_leaf.empty()) {
	  y_leaf = i_obs_conf.leaf();
	  y_var = var;
	}
      }

      std::string histname = "h_" + _fitConf.name();
      std::string draw = "";
      if (vars.getSize()==1) {
	_hist = x_var->createHistogram(histname.c_str());
	draw = x_leaf;
      }
      else if (vars.getSize()==2) {
	_hist = x_var->createHistogram(histname.c_str(), RooFit::YVar(*y_var));
	draw = y_leaf+":"+x_leaf;
      }
      else {
	throw cet::exception("Fit::fillData()") << "Can't create histogram with more than two axes";
      }
      draw += ">>";
      draw += _hist->GetName();

      tree->Draw(draw.c_str(), cutcmd(), "goff");

      std::string dataname = "data_" + _fitConf.name();
      ws->import(*(new RooDataHist(dataname.c_str(), dataname.c_str(), vars, RooFit::Import(*_hist))));
    }

    int fit(RooWorkspace* ws) {
      std::string dataname = "data_" + _fitConf.name();
      RooAbsData* data = ws->data(dataname.c_str());
      RooAbsPdf* model = ws->pdf(_fitConf.model().name().c_str());
      if (!model) {
	throw cet::exception("Fit::fit()") << "Can't find model \"" << _fitConf.model().name() << "\" in RooWorkspace" << std::endl;
      }

      // For external constraints, just make the parameters in the passed PDFs constant
      // This works for the momresp from cosmic rays fit and cemDio_mom example
      // TODO:Actually use RooFit::ExternalConstraints
      RooArgSet extConstraints;
      for (const auto& i_extConstraint : _fitConf.model().externalConstraints()) {
	std::string i_extConstraintModel = "model_" + i_extConstraint;
	std::string i_extConstraintData = "data_fit_" + i_extConstraint;
	auto i_pdf = ws->pdf(i_extConstraintModel.c_str());
	if (!i_pdf) {
	  throw cet::exception("Fit::fit()") << "Can' find external constraint model \"" << i_extConstraintModel << "\" in RooWorkspace" << std::endl;
	}
	auto i_data = ws->data(i_extConstraintData.c_str());
	if (!i_data) {
	  throw cet::exception("Fit::fit()") << "Can' find external constraint data \"" << i_extConstraintData << "\" in RooWorkspace" << std::endl;
	}

	auto params = i_pdf->getParameters(*i_data);
	auto params_iter = params->createIterator();
	RooRealVar* i_param = NULL;
	while ( (i_param = (RooRealVar*) params_iter->Next()) ) {
	  //	  i_param->Print("v");
	  i_param->setConstant(true);
	  //	  i_param->Print("v");
	}
	//	params->Print("v");

	extConstraints.add(*i_pdf);
      }
      _fitResult = model->fitTo(*data, RooFit::Save(), RooFit::Range("fit"), RooFit::Extended(true));//, RooFit::ExternalConstraints(extConstraints));

      _fitResult->printValue(std::cout);
      _fitResult->Print("v");

      int status = _fitResult->status();
      return status;
    }

    void unfold(RooWorkspace* ws) {
      if (_fitConf.unfold()) {
	// Unfold efficiency
	// should have an efficiency function and yields of each component as function of the observable
	RooAddPdf* full_model = (RooAddPdf*) ws->pdf(_fitConf.model().name().c_str());
	if (!full_model) {
	  throw cet::exception("Fit::unfold()") << "Can't find model \"" << _fitConf.model().name() << "\" in RooWorkspace";
	}

	size_t i_element = 0;
	for (const auto& i_comp : _components) {
	  //	if (!i_comp.effPdfName.empty()) {
	  RooRealVar* i_comp_yield = (RooRealVar*) full_model->coefList().at(i_element);
	  double i_comp_yield_val = i_comp_yield->getVal();
	  double i_comp_yield_err = i_comp_yield->getPropagatedError(*_fitResult);
	  
	  double effCorr = i_comp.getEffCorrection(_observables.at(0), ws); //TODO: handle more than one dimension
	  double i_comp_final_yield_val = i_comp_yield_val * effCorr;
	  double i_comp_final_yield_err = (i_comp_yield_err / i_comp_yield_val) * i_comp_final_yield_val;
	  
	  std::string new_yield_name = i_comp_yield->GetName();
	  new_yield_name += "Eff";
	  RooRealVar* i_comp_final_yield = new RooRealVar(new_yield_name.c_str(), "", i_comp_final_yield_val);
	  i_comp_final_yield->setError(i_comp_final_yield_err);
	  ws->import(*i_comp_final_yield);

	  // Calculate the fraction of the tru spectrum that has smeared out

	  double frac_smeared_away = i_comp.getFracSmeared(_observables.at(0), ws); // TODO: handle more than one dimension
	  std::string frac_smeared_name = i_comp.getName() + "FracSmeared";
	  RooRealVar* frac_smeared = new RooRealVar(frac_smeared_name.c_str(), "", frac_smeared_away);
	  //	  unfold_eff_yield->setError(final_yield_err);
	  ws->import(*frac_smeared);
	  //	}
	  ++i_element;
	}
      }
    }

    void Write() {
      _hist->Write();      
      _fitResult->Write();
    }
  };

  typedef std::vector<Fit> Fits;
};

#endif
