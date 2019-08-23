#include <iostream>
#include <sstream>
#include <fstream>

#include <getopt.h>

#include "cetlib_except/exception.h"

#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TCut.h"

#include "Main/inc/Analysis.hh"

namespace roofitter {

  struct InputArgs {
    InputArgs() : cfg_filename(""), need_help(false), debug_cfg(false), debug_cfg_filename("") { }

    std::string cfg_filename;
    bool need_help;
    bool debug_cfg;
    std::string debug_cfg_filename;
    std::string input_filename;
    std::string input_treename;
    std::string output_filename;
  };

  void PrintHelp() {
    std::cout << "Input Arguments:" << std::endl;
    std::cout << "\t-c, --config [cfg file]: input configuration file" << std::endl;
    std::cout << "\t-i, --input [root file]: input ROOT file containing the tree (overrides anything in cfg file)" << std::endl;
    std::cout << "\t-t, --tree [tree name]: tree name (inc. directory) in the input file (overrides anything in cfg file)" << std::endl;
    std::cout << "\t-o, --output [root file]: output ROOT file that will be created (overrides anything in cfg file)" << std::endl;
    std::cout << "\t-d, --debug-config [filename]: print out the final config file to file" << std::endl;
    std::cout << "\t-h, --help: print this help message" << std::endl;
  }

  void ProcessArgs(int argc, char** argv, InputArgs& args) {
    const char* const short_opts = "c:i:t:o:d:h";

    const option long_opts[] = {
      {"config", required_argument, nullptr, 'c'},
      {"input", required_argument, nullptr, 'i'},
      {"tree", required_argument, nullptr, 't'},
      {"output", required_argument, nullptr, 'o'},
      {"debug-config", required_argument, nullptr, 'd'},
      {"help", no_argument, nullptr, 'h'},
      {nullptr, no_argument, nullptr, 0}
    };

    while (true) {
      const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);

      if (-1 == opt)
	break;

      switch (opt) {
    
      case 'c':
	args.cfg_filename = std::string(optarg);
	break;

      case 'i':
	args.input_filename = std::string(optarg);
	break;

      case 't':
	args.input_treename = std::string(optarg);
	break;

      case 'o':
	args.output_filename = std::string(optarg);
	break;

      case 'd':
	args.debug_cfg = true;
	args.debug_cfg_filename = std::string(optarg);
	break;

      case 'h': // -h or --help
      case '?': // Unrecognized option
      default:
	args.need_help = true;
	break;
      }
    }
  }

  int main(int argc, char **argv) {

    InputArgs args;
    ProcessArgs(argc, argv, args);
    if (args.need_help) {
      PrintHelp();
      return 0;
    }

    mu2e::SimpleConfig config(args.cfg_filename);
    if (args.debug_cfg) {
      std::ofstream fileout(args.debug_cfg_filename);
      config.print(fileout);
      std::cout << "Config written to " << args.debug_cfg_filename << std::endl;
      return 0;
    }


    std::string filename = config.getString("input.filename", "");
    if (!args.input_filename.empty()) { // override cfg file with
      filename = args.input_filename;
    }
    if (filename.empty()) {
      throw cet::exception("roofitter::main()") << "No filename specified";
    }
    TFile* file = new TFile(filename.c_str(), "READ");
    if (file->IsZombie()) {
      throw cet::exception("roofitter::main") << "Input file " << filename << " is a zombie";
    }

    std::string treename = config.getString("input.treename", "");
    if (!args.input_treename.empty()) { // override cfg tree with
      treename = args.input_treename;
    }
    if (treename.empty()) {
      throw cet::exception("roofitter::main()") << "No treename specified";
    }
    TTree* tree = (TTree*) file->Get(treename.c_str());
    if (!tree) {
      throw cet::exception("roofitter::main") << "Input tree " << treename << " is not in file";
    }

    std::vector<Analysis> analyses;
    std::vector<std::string> ana_names;
    config.getVectorString("analyses", ana_names);
    for (const auto& i_ana_name : ana_names) {
      analyses.push_back(Analysis(i_ana_name, config));
    }

    for (auto& i_ana : analyses) {
      i_ana.fillData(tree);
      i_ana.fit();
      if (config.getBool(i_ana.name+".unfold", false)) {
	i_ana.unfold();
      }
      i_ana.calculate();
    }

    std::string outfilename = config.getString("output.filename", "");
    if (!args.output_filename.empty()) { // override cfg file with
      outfilename = args.output_filename;
    }
    if (outfilename.empty()) {
      throw cet::exception("roofitter::main()") << "No outfilename specified";
    }
    TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
    for (auto& i_ana : analyses) {
      TDirectory* outdir = outfile->mkdir(i_ana.name.c_str());
      outdir->cd();
      i_ana.Write();
      outfile->cd();
    }
    outfile->Write();
    outfile->Close();

    std::cout << "Done" << std::endl;
    return 0;
  }
}

int main(int argc, char **argv) {

  roofitter::main(argc, argv);
  return 0;
}
