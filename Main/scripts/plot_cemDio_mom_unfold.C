#include "PlotSettings.hh"

void plot_cemDio_mom_unfold(std::string filename, const PlotSettings& plot) {

  TFile* file = new TFile(filename.c_str(), "READ");
  
  std::string wsname = plot.ana_name + "/" + plot.ana_name;
  RooWorkspace* ws = (RooWorkspace*) file->Get(wsname.c_str());
  if (!ws) {
    std::cout << "ERROR: No workspace named \"" << wsname << "\" in file" << std::endl;
    return;
  }
  ws->Print();

  RooRealVar* mom = ws->var("mom");
  RooPlot* rooplot = mom->frame(RooFit::Range("fit"));
  
  std::string dataname = plot.data_name;
  RooAbsData* data = ws->data(dataname.c_str());
  if (!data) {
    std::cout << "ERROR: No data named \"" << dataname << "\" in workspace" << std::endl;
    return;
  }
  data->plotOn(rooplot);

  Int_t cem_colour = kRed;
  Int_t dio_colour = kBlue;

  std::string modelname = plot.model_name;
  RooAbsPdf* model = ws->pdf(modelname.c_str());
  if (!model) {
    std::cout << "ERROR: No model named \"" << modelname << "\" in workspace" << std::endl;
    return;
  }
  model->plotOn(rooplot);
  RooHist* mom_pull = rooplot->pullHist();
  model->plotOn(rooplot, RooFit::Components("cemLLmomEffResp"), RooFit::LineColor(cem_colour), RooFit::LineStyle(kDashed));
  model->plotOn(rooplot, RooFit::Components("dioPol58momEffResp"), RooFit::LineColor(dio_colour), RooFit::LineStyle(kDashed));

  RooRealVar* NCe = ws->var("NCe");
  RooRealVar* NDio = ws->var("NDio");
  RooRealVar* NCeEff = ws->var("NCeEff");
  RooRealVar* NDioEff = ws->var("NDioEff");
  RooAbsPdf* Rmue = ws->pdf("Rmue");
  std::cout << "NCe = " << NCe->getVal() << " +/- " << NCe->getError() << std::endl;
  std::cout << "NDio = " << NDio->getVal() << " +/- " << NDio->getError() << std::endl;
  std::cout << "NCeEff = " << NCeEff->getVal() << " +/- " << NCeEff->getError() << std::endl;
  std::cout << "NDioEff = " << NDioEff->getVal() << " +/- " << NDioEff->getError() << std::endl;
  std::cout << "DIO Frac Smeared Away = " << ws->var("dioPol58FracSmeared")->getVal() << std::endl;
  std::cout << "CE Frac Smeared Away = " << ws->var("cemLLFracSmeared")->getVal() << std::endl;
  std::cout << "Rmue = " << Rmue->getVal() << std::endl;

  TCanvas* c = new TCanvas();
  TPad* pad1 = new TPad("", "", 0.0, 0.4, 1.0, 1.0);
  TPad* pad2 = new TPad("", "", 0.0, 0.0, 1.0, 0.4);
  pad1->Draw();
  pad2->Draw();
  
  pad1->SetLogy();
  pad1->cd();
  rooplot->Draw();
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
  text << "After efficiency correction: ";
  latex->SetTextColor(kBlack);
  latex->DrawLatexNDC(ndc_x, current_ndc_y, text.str().c_str());
  current_ndc_y += step_ndc_y;

  text.str("");
  text << std::fixed << std::setprecision(1);
  text << "N Ce = " << NCeEff->getVal() << " #pm ??";
  latex->SetTextColor(kBlack);
  latex->DrawLatexNDC(ndc_x, current_ndc_y, text.str().c_str());
  current_ndc_y += step_ndc_y;

  text.str("");
  text << std::fixed << std::setprecision(1);
  text << "N Dio = " << NDioEff->getVal() << " #pm ??";
  latex->SetTextColor(kBlack);
  latex->DrawLatexNDC(ndc_x, current_ndc_y, text.str().c_str());
  current_ndc_y += step_ndc_y;
  current_ndc_y += step_ndc_y;

  text.str("");
  text << std::scientific << std::setprecision(2);
  text << "True DIO > 95 MeV = " << ws->var("f_dio_above_95")->getVal() << " #pm ??";
  latex->SetTextColor(kBlack);
  latex->DrawLatexNDC(ndc_x, current_ndc_y, text.str().c_str());
  current_ndc_y += step_ndc_y;

  text.str("");
  text << std::fixed << std::setprecision(3);
  text << "Frac DIO Smeared Out = " << ws->var("dioPol58FracSmeared")->getVal() << " #pm ??";
  latex->SetTextColor(kBlack);
  latex->DrawLatexNDC(ndc_x, current_ndc_y, text.str().c_str());
  current_ndc_y += step_ndc_y;

  text.str("");
  text << std::scientific << std::setprecision(2);
  text << "Total DIO = " << ws->pdf("NDioTotal")->getVal() << " #pm ??";
  latex->SetTextColor(kBlack);
  latex->DrawLatexNDC(ndc_x, current_ndc_y, text.str().c_str());
  current_ndc_y += step_ndc_y;

  text.str("");
  text << std::scientific << std::setprecision(2);
  text << "#rightarrow N Cap = " << ws->pdf("NCap")->getVal() << " #pm ??";
  latex->SetTextColor(kBlack);
  latex->DrawLatexNDC(ndc_x, current_ndc_y, text.str().c_str());
  current_ndc_y += step_ndc_y;
  current_ndc_y += step_ndc_y;

  text.str("");
  text << std::scientific << std::setprecision(2);
  latex->SetTextColor(kBlack);
  text << "R_mue = " << ws->pdf("Rmue")->getVal() << " #pm ??";
  latex->DrawLatexNDC(ndc_x, current_ndc_y, text.str().c_str());
  current_ndc_y += step_ndc_y;


  pad2->cd();
  RooPlot* mom_pull_frame = mom->frame();
  mom_pull_frame->addPlotable(mom_pull, "P");
  mom_pull_frame->Draw();    

}
