#include "PlotSettings.hh"

void plot_crv_momresp(std::string filename, const std::vector<PlotSettings>& plots) {

  TLegend* legend = new TLegend(0.55, 0.85, 0.85, 0.75, "");
  legend->SetBorderSize(0);
  legend->SetTextSize(0.035);
  legend->SetFillColor(kWhite);

  std::stringstream leglabel;

  TFile* file = new TFile(filename.c_str(), "READ");
  
  TCanvas* c = new TCanvas();
  //    c->SetLogy();

  for (const auto& i_plot : plots) {
    std::string analysis_name = i_plot.ana_name;
    std::string fit_name = i_plot.fit_name;
    
    std::string wsname = analysis_name + "/" + analysis_name;
    RooWorkspace* ws = (RooWorkspace*) file->Get(wsname.c_str());
    if (!ws) {
      std::cout << "ERROR: No workspace named \"" << wsname << "\" in file" << std::endl;
      return;
    }
    ws->Print();

    RooRealVar* momresp = ws->var("momresp");
    //    RooPlot* plot = momresp->frame(RooFit::Range("fit"));
    RooPlot* plot = momresp->frame();
    plot->SetTitle("Momentum Response");
    plot->GetXaxis()->SetTitle("#frac{1}{2} (p_{down} - p_{up}) [MeV/c]");
    plot->GetYaxis()->SetTitle("Count / 0.25 MeV/c");
      
    std::string dataname = i_plot.data_name;
    RooAbsData* data = ws->data(dataname.c_str());
    if (!data) {
      std::cout << "ERROR: No data named \"" << dataname << "\" in workspace" << std::endl;
      return;
    }
    data->plotOn(plot);

    std::string modelname = i_plot.model_name;
    RooAbsPdf* model = ws->pdf(modelname.c_str());
    if (!model) {
      std::cout << "ERROR: No model named \"" << modelname << "\" in workspace" << std::endl;
      return;
    }
    model->plotOn(plot, RooFit::LineColor(i_plot.colour));
      
    std::cout << "chi^2 / ndf = " << plot->chiSquare(i_plot.n_fit_params) << std::endl;
    
    plot->Draw("SAME");
    
    TH1F* hist = new TH1F("hist", "", 1,0,1);
    hist->SetLineColor(i_plot.colour);
    hist->SetLineWidth(2);
    
    leglabel.str("");
    leglabel << i_plot.leglabel;
    
    std::cout << leglabel.str() << std::endl;
    legend->AddEntry(hist, leglabel.str().c_str(), "l");
  }

  legend->Draw();
}
