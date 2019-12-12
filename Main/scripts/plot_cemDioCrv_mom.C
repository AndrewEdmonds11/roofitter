void plot_cemDioCrv_mom(std::string filename) {

  TFile* file = new TFile(filename.c_str(), "READ");
  
  RooWorkspace* ws = (RooWorkspace*) file->Get("ana_cemDioCrv_mom/ana_cemDioCrv_mom");

  ws->Print();

  RooRealVar* mom = ws->var("mom");
  RooPlot* plot = mom->frame(RooFit::Range("fit"));
  
  RooAbsData* data = ws->data("data_fit_cemDioCrv_mom");
  data->plotOn(plot);

  Int_t cem_colour = kRed;
  Int_t dio_colour = kBlue;
  Int_t crv_colour = kMagenta;
  
  RooAbsPdf* pdf = ws->pdf("model_cemDioCrv_mom");
  pdf->plotOn(plot);
  RooHist* mom_pull = plot->pullHist();
  pdf->plotOn(plot, RooFit::Components("cemLLmomEffResp"), RooFit::LineColor(cem_colour), RooFit::LineStyle(kDashed));
  pdf->plotOn(plot, RooFit::Components("dioPol58momEffResp"), RooFit::LineColor(dio_colour), RooFit::LineStyle(kDashed));
  pdf->plotOn(plot, RooFit::Components("crvFlatmomEffResp"), RooFit::LineColor(crv_colour), RooFit::LineStyle(kDashed));

  RooRealVar* NCe = ws->var("NCe");
  RooRealVar* NDio = ws->var("NDio");
  RooRealVar* NCrv = ws->var("NCrv");
  std::cout << "NCe = " << NCe->getValV() << " +/- " << NCe->getError() << std::endl;
  std::cout << "NDio = " << NDio->getValV() << " +/- " << NDio->getError() << std::endl;
  std::cout << "NCrv = " << NCrv->getValV() << " +/- " << NCrv->getError() << std::endl;

  TCanvas* c = new TCanvas();
  TPad* pad1 = new TPad("", "", 0.0, 0.4, 1.0, 1.0);
  TPad* pad2 = new TPad("", "", 0.0, 0.0, 1.0, 0.4);
  pad1->Draw();
  pad2->Draw();
  
  pad1->SetLogy();
  pad1->cd();
  plot->Draw();
  TLatex* latex = new TLatex();
  latex->SetTextAlign(22);
  latex->SetTextSize(0.06);
  std::stringstream text;
  double ndc_x = 0.7;
  double first_ndc_y = 0.6;
  double step_ndc_y = -0.05;
  double current_ndc_y = first_ndc_y;
  text.str("");
  text << std::fixed << std::setprecision(1);
  text << "N Ce = " << NCe->getVal() << " #pm " << NCe->getError();
  latex->SetTextColor(cem_colour);
  latex->DrawLatexNDC(ndc_x, current_ndc_y, text.str().c_str());
  current_ndc_y += step_ndc_y;
  
  text.str("");
  text << std::fixed << std::setprecision(1);
  text << "N Dio = " << NDio->getVal() << " #pm " << NDio->getError();
  latex->SetTextColor(dio_colour);
  latex->DrawLatexNDC(ndc_x, current_ndc_y, text.str().c_str());
  current_ndc_y += step_ndc_y;

  text.str("");
  text << std::fixed << std::setprecision(1);
  text << "N Crv = " << NCrv->getVal() << " #pm " << NCrv->getError();
  latex->SetTextColor(crv_colour);
  latex->DrawLatexNDC(ndc_x, current_ndc_y, text.str().c_str());
  current_ndc_y += step_ndc_y;


  pad2->cd();
  RooPlot* mom_pull_frame = mom->frame();
  mom_pull_frame->addPlotable(mom_pull, "P");
  mom_pull_frame->Draw();    
}
