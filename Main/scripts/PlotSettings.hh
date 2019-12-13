#ifndef PlotSettings_hh_
#define PlotSettings_hh_

struct PlotSettings {

  PlotSettings(std::string ana_name, std::string fit_name, std::string model_name, std::string data_name, 
	       Int_t colour = kBlack, int n_fit_params = -1, std::string leglabel = "") :
    ana_name(ana_name), fit_name(fit_name), model_name(model_name), data_name(data_name), 
    colour(colour), n_fit_params(n_fit_params), leglabel(leglabel) {}

  std::string ana_name;
  std::string fit_name;
  std::string model_name;
  std::string data_name;
  Int_t colour;
  int n_fit_params;
  std::string leglabel;
};

#endif
