#include "PlotSettings.hh"
#include "plot_cemDio_mom_unfold.C"

void plot_example_unfold(std::string filename) {

  PlotSettings plot("ana_cemDio_mom_unfold", "fit_cemDio_mom", "model_cemDio_mom", "data_cemDio_mom");
  plot_cemDio_mom_unfold(filename, plot);
}
