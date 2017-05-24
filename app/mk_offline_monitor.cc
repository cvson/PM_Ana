#include<iostream>
#include<sstream>
#include<fstream>
using namespace std;
#include <iomanip.h>
#include <sys/stat.h>

#include <TROOT.h>
#include <TStyle.h>
#include <TApplication.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TEventList.h>
#include <TBranch.h>
#include <TH1.h>

#include <TGraph.h>
#include <TGaxis.h>
#include <TMarker.h>
#include <TText.h>
#include <TMath.h>
#include <TSpectrum.h>
#include <TString.h>
#include <TSystem.h>
#include <TBrowser.h>

#include "TApplication.h"
#include "plot.cxx"
#include "setup.hxx"



int main(int argc,char *argv[]){
  TROOT root("GUI","GUI");
  TApplication theApp("App",&argc,argv);
  TFile *f = new TFile("offline_monitor.root","recreate");
  Int_t run_number_1,run_number_2;
  int c=-1;

  while ((c = getopt(argc, argv, "r:s:")) != -1) {
    switch(c){
    case 'r':
      run_number_1=atoi(optarg);
      break;
    case 's':
      run_number_2=atoi(optarg);
      break;

    }
  }
  
  plot fplot(run_number_1,run_number_2);
  if(!fplot.flag)exit(1);


  f->mkdir("Gain_Plot");//it has the histogram and plot to monitor gain
  f->cd("Gain_Plot");
  fplot.plot_gain();

  f->mkdir("Gain_Ratio_Plot");//it has the histogram and plot to monitor gain
  f->cd("Gain_Ratio_Plot");
  fplot.plot_gain_ratio();

  f->mkdir("Pedestal_Plot");//it has the histogram and plot to monitor gain
  f->cd("Pedestal_Plot");
  fplot.plot_pedestal();

  f->mkdir("Pedestal_Sigma_Plot");//it has the histogram and plot to monitor gain
  f->cd("Pedestal_Sigma_Plot");
  fplot.plot_pedestal_sigma();

  f->mkdir("Noise_Plot");//it has the histogram and plot to monitor gain
  f->cd("Noise_Plot");
  fplot.plot_noise();

  f->mkdir("Noise_Ratio_Plot");//it has the histogram and plot to monitor gain
  f->cd("Noise_Ratio_Plot");
  fplot.plot_noise_ratio();
  

  f->Write();
  f->Close();

}
