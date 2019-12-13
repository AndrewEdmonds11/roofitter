#include "plot_cemDio_mom.C"
#include "plot_crv_momresp.C"

void plot_example_constrain(std::string filename) {
  PlotSettings plot("ana_cemDio_mom_constrain", "fit_cemDio_mom", "model_cemDio_mom", "data_cemDio_mom");
  plot_cemDio_mom(filename, plot);

  PlotSettings floated("ana_cemDio_mom_constrain", "fit_crv_momresp", "model_crv_momresp", "data_crv_momresp", kRed, 7, "floated");
  std::vector<PlotSettings> plots;
  plots.push_back(floated);
  plot_crv_momresp(filename, plots);
}
