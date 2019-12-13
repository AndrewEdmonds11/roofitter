#include "plot_crv_momresp.C"

void plot_example_momresp(std::string filename) {

  PlotSettings floated("ana_crv_momresp", "fit_crv_momresp", "model_crv_momresp", "data_crv_momresp", kRed, 7, "floated");
  PlotSettings fixed("ana_crv_momrespFixed", "fit_crv_momrespFixed", "model_crv_momrespFixed", "data_crv_momrespFixed", kBlue, 1, "fixed");

  std::vector<PlotSettings> plots;
  plots.push_back(floated);
  plots.push_back(fixed);

  plot_crv_momresp(filename, plots);
}
