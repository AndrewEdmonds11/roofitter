#ifndef Analysis_hh_
#define Analysis_hh_

#include "TFile.h"
#include "TH1.h"
#include "TF1.h"

#include "RooWorkspace.h"
#include "RooDataHist.h"
#include "RooPlot.h"
#include "RooAddPdf.h"
#include "RooFFTConvPdf.h"
#include "RooRealVar.h"
#include "RooFitResult.h"
#include "RooHistPdf.h"
#include "RooEffProd.h"

#include "RooNumIntConfig.h"

#include "Main/inc/Configs.hh"
#include "Main/inc/Fit.hh"

namespace roofitter {

  typedef std::string PdfName;
  typedef std::string Calculation;

  typedef std::vector<Calculation> Calculations;

  struct AnalysisConfig {
    fhicl::Atom<std::string> name{fhicl::Name("name"), fhicl::Comment("Analysis name")};
    fhicl::Sequence< fhicl::Table<FitConfig> > fits{fhicl::Name("fits"), fhicl::Comment("The list of fits for this analysis in order")};
    fhicl::Atom<bool> allow_failure{fhicl::Name("allow_failure"), fhicl::Comment("If set to true, then roofitter will not throw an exception for a failed fit."), false};
    fhicl::Sequence<std::string> calculations{fhicl::Name("calculations"), fhicl::Comment("A list of supplemental calculations that you want to calculate"), std::vector<std::string>()};
  };

  class Analysis {
  private:
    AnalysisConfig _anaConf;
    RooWorkspace* _ws;

    Fits _fits;

  public:
    Analysis(const AnalysisConfig& cfg) : 
      _anaConf(cfg),
      _ws(new RooWorkspace(_anaConf.name().c_str(), true))
    {
      std::cout << _anaConf.name() << std::endl;

      for (const auto& i_fit_cfg : _anaConf.fits()) {
	Fit i_fit(i_fit_cfg, _ws);
	_fits.push_back(i_fit);
      }
    }

    void fillData(TTree* tree) {
      for (auto& i_fit : _fits) {
	i_fit.fillData(tree, _ws);
      }
    }

    void fit() {
      for (auto& i_fit : _fits) {
	int status = i_fit.fit(_ws);
	if (status>0) {
	  if (!_anaConf.allow_failure()) {
	    throw cet::exception("Analysis::fitTo()") << "Fit failed! If you want roofitter to continue and not throw this exception then set allow_failure to true in your fcl file";
	  }
	}
      }      
    }

    void unfold() {
      for (auto& i_fit : _fits) {
	i_fit.unfold(_ws);
      }
    }

    void calculate() {
      std::stringstream factory_cmd;
      for (const auto& i_calc : _anaConf.calculations()) {
	factory_cmd.str("");
	factory_cmd << i_calc;
	_ws->factory(factory_cmd.str().c_str());
      }
    }

    void Write() {
      for (auto& i_fit : _fits) {
	i_fit.Write();
      }
      _ws->Print();
      _ws->Write();
    }

    const AnalysisConfig& getConf() const { return _anaConf; }
  };
}

#endif
