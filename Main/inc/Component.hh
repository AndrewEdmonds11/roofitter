#ifndef Component_hh_
#define Component_hh_

#include "RooWorkspace.h"
#include "ConfigTools/inc/SimpleConfig.hh"

#include "Main/inc/Observable.hh"

namespace trkana {

  typedef std::string ObsName;
  typedef std::string PdfName;

  struct Component {

    Component(const std::string& comp_name, const mu2e::SimpleConfig& config, RooWorkspace* ws) : name(comp_name) {
    }

    void constructPdfs(const Observables& all_obs, const mu2e::SimpleConfig& config, RooWorkspace* ws) {
      std::stringstream factory_cmd;

      int i_obs = -1;
      for (const auto& obs : all_obs) {
	++i_obs;
	// Construct the true Pdf
	std::string pdf = config.getString(name+"."+obs.name+".pdf");

	factory_cmd.str("");
	factory_cmd << pdf;
	ws->factory(factory_cmd.str().c_str());

	std::string truePdfName = name+obs.name;
	truePdfNames.insert(std::pair<ObsName, PdfName>(obs.name, truePdfName));
	std::string effPdfName = "";
	std::string resPdfName = "";

	// Construct the Pdf with Eff, if possible
	std::string currentPdfSuffix = "";
	if(!obs.effName.empty()) {
	  std::string this_suffix = "Eff";

	  std::string currentPdfName = truePdfName + currentPdfSuffix;
	  effPdfName = currentPdfName + this_suffix;
	  effPdfNames.insert(std::pair<ObsName, PdfName>(obs.name, effPdfName));
	    
	  // Construct the Pdfs that include efficiency
	  ws->import(*(new RooEffProd(effPdfName.c_str(), "", *ws->pdf(currentPdfName.c_str()), *ws->function(obs.effName.c_str()))));
	  
	  currentPdfSuffix += this_suffix;
	}

	// Construct the smeared pdf
	if (!obs.resName.empty()) {
	  std::string this_suffix = "Res";
	  std::string currentPdfName = truePdfName + currentPdfSuffix;
	  resPdfName = currentPdfName + this_suffix;
	  resPdfNames.insert(std::pair<ObsName, PdfName>(obs.name, resPdfName));
	  
	  factory_cmd.str("");
	  factory_cmd << "FCONV::" << resPdfName << "(" << obs.name << ", " << currentPdfName << ", " << obs.resName << ")";
	  ws->factory(factory_cmd.str().c_str());
	
	  ((RooFFTConvPdf*) ws->pdf(resPdfName.c_str()))->setBufferFraction(5.0);

	  currentPdfSuffix += this_suffix;
	}

	// Set any new integrator for all the Pdfs
	std::string new_integrator = config.getString(name+"."+obs.name+".integrator", "");
	if (!new_integrator.empty()) {
	  RooNumIntConfig customConfig(*RooAbsReal::defaultIntegratorConfig());
	  customConfig.method1D().setLabel(new_integrator.c_str());

	  if(!truePdfName.empty()) {
	    ws->pdf(truePdfName.c_str())->setIntegratorConfig(customConfig);
	  }
	  if(!effPdfName.empty()) {
	    ws->pdf(effPdfName.c_str())->setIntegratorConfig(customConfig);
	  }
	  if(!resPdfName.empty()) {
	    ws->pdf(resPdfName.c_str())->setIntegratorConfig(customConfig);
	  }
	}
      }

      // Now create 2D models from these
      if (all_obs.size()==2) {

	std::string xObs = all_obs.at(0).name;
	std::string xPdf = "";
 	if(resPdfNames.find(xObs) != resPdfNames.end()) {
	  xPdf = resPdfNames.at(xObs);
	}
 	else if(effPdfNames.find(xObs) != effPdfNames.end()) {
	  xPdf = effPdfNames.at(xObs);
	}
 	else if(truePdfNames.find(xObs) != truePdfNames.end()) {
	  xPdf = truePdfNames.at(xObs);
	}
	else {
	  throw cet::exception("Component::constructPdfs") << "No valid Pdf name for 2D pdf";
	}

	std::string yObs = all_obs.at(1).name;
	std::string yPdf = "";
 	if(resPdfNames.find(yObs) != resPdfNames.end()) {
	  yPdf = resPdfNames.at(yObs);
	}
 	else if(effPdfNames.find(yObs) != effPdfNames.end()) {
	  yPdf = effPdfNames.at(yObs);
	}
 	else if(truePdfNames.find(yObs) != truePdfNames.end()) {
	  yPdf = truePdfNames.at(yObs);
	}
	else {
	  throw cet::exception("Component::constructPdfs") << "No valid Pdf name for 2D pdf";
	}

	factory_cmd.str("");
	factory_cmd << "PROD::" << name << "2D(" << xPdf << "|" << yObs << "," << yPdf << ")";
	//	factory_cmd << "PROD::" << name << "2D(" << xPdf << "," << yPdf << ")";
	std::cout << factory_cmd.str() << std::endl;
	ws->Print();
	ws->factory(factory_cmd.str().c_str());
      }
      
      if (all_obs.size()>2) {
	throw cet::exception("Component::constructPdfs") << "Do not currently support more than two observables";
      }
    }

    // Returns the efficiency correction to apply to any yield
    // (i.e. it is the efficiency 
    double getEffCorrection(const Observable& obs, RooWorkspace* ws) const {

      RooAbsPdf* this_pdf = ws->pdf(resPdfNames.at(obs.name).c_str());
      RooRealVar* this_obs = ws->var(obs.name.c_str());
      RooFormulaVar* effFunc = (RooFormulaVar*) ws->function(obs.effName.c_str());
      TF1* effFn = effFunc->asTF(*this_obs, RooArgList(), RooArgList());

      double result = 0;
      double obs_step = obs.hist_bin_width;
      for (double i_obs = obs.hist_min; i_obs < obs.hist_max; i_obs += obs_step) {
	double j_obs = i_obs + obs_step;
	    
	this_obs->setRange("range", i_obs, j_obs);
	    
	double i_eff = effFn->Eval(i_obs);
	//	std::cout << "Eff @ " << i_obs << " MeV = " << i_eff << std::endl;

	RooAbsReal* pdf_integral = this_pdf->createIntegral(*this_obs, RooFit::NormSet(*this_obs), RooFit::Range("range"));
	double i_pdf = pdf_integral->getVal();
	//	std::cout << effPdfName << " @ " << i_obs << " MeV = " << i_pdf << std::endl;
	double i_effCorr = i_pdf / i_eff;
	result += i_effCorr;
      }

      return result;
    }

    // Returns the fraction of the true pdf that has smeared out
    double getFracSmeared(const Observable& obs, RooWorkspace* ws) const {

      RooRealVar* this_obs = ws->var(obs.name.c_str());
      RooAbsPdf* truePdf = ws->pdf(name.c_str());
      RooAbsPdf* resPdf = ws->pdf(obs.resName.c_str());
      double min_res = obs.res_valid_min; double max_res = obs.res_valid_max;

      double result = 0;
      double min_obs = obs.hist_min; double max_obs = obs.hist_max;
      double obs_step = obs.hist_bin_width;

      std::cout << "getFracSmeared() might take a while..." << std::endl;
      for (double i_obs = min_obs; i_obs < max_obs; i_obs += obs_step) {
	double j_obs = i_obs + obs_step;

	std::cout << i_obs << " MeV -- " << j_obs << " MeV" << std::endl;

	// How much will be smeared out the bottom
	this_obs->setMin(min_res);
	this_obs->setMax(max_res);	    
	double min_res_smear = min_res;
	double max_res_smear = min_obs-j_obs;
	double resPdf_integral_low_val = 0;
	if (max_res_smear > min_res_smear) {
	  this_obs->setRange("resRange", min_res_smear, max_res_smear);
	  RooAbsReal* resPdf_integral_low = resPdf->createIntegral(*this_obs, RooFit::NormSet(*this_obs), RooFit::Range("resRange"));
	  resPdf_integral_low_val = resPdf_integral_low->getVal();
	  std::cout << "Res Smear Integral Low (" << min_res_smear << " MeV -- " << max_res_smear << " MeV) = " << resPdf_integral_low_val << std::endl;
	}

	// How much will be smeared out the top
	min_res_smear = max_obs-j_obs;
	max_res_smear = max_res;
	double resPdf_integral_high_val = 0;
	if (max_res_smear > min_res_smear) {
	  this_obs->setRange("resRange", min_res_smear, max_res_smear);
	  RooAbsReal* resPdf_integral_high = resPdf->createIntegral(*this_obs, RooFit::NormSet(*this_obs), RooFit::Range("resRange"));
	  resPdf_integral_high_val = resPdf_integral_high->getVal();
	  std::cout << "Res Smear Integral High (" << min_res_smear << " MeV -- " << max_res_smear << " MeV) = " << resPdf_integral_high_val << std::endl;
	}

	// Reset the limits here so that they are correct when we exit the for loop
	this_obs->setMax(max_obs);
	this_obs->setMin(min_obs);
	if (resPdf_integral_low_val>1e-4 || resPdf_integral_high_val>1e-4) { // only calculate the truth if we have to
	  // How much of the truth is here?
	  this_obs->setRange("new", i_obs, j_obs);
	  RooAbsReal* truePdf_integral = truePdf->createIntegral(*this_obs, RooFit::NormSet(*this_obs), RooFit::Range("new"));
	  double truePdf_integral_val = truePdf_integral->getVal();
	  std::cout << "True PDF Integral (" << i_obs << " MeV -- " << j_obs << " MeV) = " << truePdf_integral_val << std::endl;
	  //	if(truePdf_integral_val<1e-3) {
	  //	  break;
	  //	}
	  double smeared_away = (resPdf_integral_low_val + resPdf_integral_high_val) * truePdf_integral_val;
	  std::cout << "Fraction Smeared Away = " << smeared_away << std::endl;
	  result += smeared_away;
	}
	else {
	  std::cout << "This region won't smear in either direction" << std::endl;
	}
      }

      return result;
    }

    std::string name;
    std::map<ObsName, PdfName> truePdfNames;
    std::map<ObsName, PdfName> effPdfNames;
    std::map<ObsName, PdfName> resPdfNames;
  };
  typedef std::vector<Component> Components;
}

#endif
