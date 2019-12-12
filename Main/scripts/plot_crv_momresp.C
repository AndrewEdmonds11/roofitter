void plot_crv_momresp(std::string filename) {

  const int n_analyses = 2;
  std::string names[n_analyses] = {"crv_momresp", "crv_momrespFixed"};
  Int_t colours[n_analyses] = {kRed, kBlue};
  int n_fit_params[n_analyses] = {7, 1};
  std::string leglabels[n_analyses] = {"floated", "fixed"};
  int precision[n_analyses] = {2, 1};

  TLegend* legend = new TLegend(0.55, 0.85, 0.85, 0.75, "");
  legend->SetBorderSize(0);
  legend->SetTextSize(0.035);
  legend->SetFillColor(kWhite);

  std::stringstream leglabel;

  TFile* file = new TFile(filename.c_str(), "READ");
  
  TCanvas* c = new TCanvas();
  //    c->SetLogy();

  for (int i_analysis = 0; i_analysis < n_analyses; ++i_analysis) {
    std::string name = names[i_analysis];
    std::string analysis_name = "ana_" + name;
    std::string fit_name = "fit_" + name;
    
    std::string wsname = analysis_name + "/" + analysis_name;
    RooWorkspace* ws = (RooWorkspace*) file->Get(wsname.c_str());
    ws->Print();

    RooRealVar* momresp = ws->var("momresp");
    //    RooPlot* plot = momresp->frame(RooFit::Range("fit"));
    RooPlot* plot = momresp->frame();
    plot->SetTitle("Momentum Response");
    plot->GetXaxis()->SetTitle("#frac{1}{2} (p_{down} - p_{up}) [MeV/c]");
    plot->GetYaxis()->SetTitle("Count / 0.25 MeV/c");
      
    std::string dataname = "data_" + fit_name;
    RooAbsData* data = ws->data(dataname.c_str());
    data->plotOn(plot);

      std::string modelname = "model_" + name;
      RooAbsPdf* model = ws->pdf(modelname.c_str());
      model->plotOn(plot, RooFit::LineColor(colours[i_analysis]));
      
      std::cout << "chi^2 / ndf = " << plot->chiSquare(n_fit_params[i_analysis]) << std::endl;

      plot->Draw("SAME");

      TH1F* hist = new TH1F("hist", "", 1,0,1);
      hist->SetLineColor(colours[i_analysis]);
      hist->SetLineWidth(2);

      leglabel.str("");
      leglabel << leglabels[i_analysis];

      std::cout << leglabel.str() << std::endl;
      legend->AddEntry(hist, leglabel.str().c_str(), "l");
  }

  legend->Draw();
}
