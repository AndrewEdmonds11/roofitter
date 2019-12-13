#include "PlotSettings.hh"
#include "plot_cemDio_mom.C"
#include "plot_cemDioCrv_mom.C"

void plot_example_two_analyses(std::string filename) {

  PlotSettings plot1("ana_cemDio_mom", "fit_cemDio_mom", "model_cemDio_mom", "data_cemDio_mom");
  plot_cemDio_mom(filename, plot1);

  PlotSettings plot2("ana_cemDioCrv_mom", "fit_cemDioCrv_mom", "model_cemDioCrv_mom", "data_cemDioCrv_mom");
  plot_cemDioCrv_mom(filename, plot2);
}
