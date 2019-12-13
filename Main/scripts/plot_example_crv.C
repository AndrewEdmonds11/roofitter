#include "PlotSettings.hh"
#include "plot_cemDioCrv_mom.C"

void plot_example_crv(std::string filename) {

  PlotSettings plot("ana_cemDioCrv_mom", "fit_cemDioCrv_mom", "model_cemDioCrv_mom", "data_cemDioCrv_mom");
  plot_cemDioCrv_mom(filename, plot);
}
