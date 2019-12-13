#include "PlotSettings.hh"
#include "plot_cemDio_mom.C"

void plot_example(std::string filename) {

  PlotSettings plot("ana_cemDio_mom", "fit_cemDio_mom", "model_cemDio_mom", "data_cemDio_mom");
  plot_cemDio_mom(filename, plot);
}
