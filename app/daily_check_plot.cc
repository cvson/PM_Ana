#include <iostream>
#include <sstream>
#include <fstream>
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
#include <TH2.h>
#include <TProfile.h>

#include <TGraph.h>
#include <TGaxis.h>
#include <TMarker.h>
#include <TText.h>
#include <TMath.h>
#include <TSpectrum.h>
#include <TString.h>
#include <TSystem.h>

#include "TApplication.h"
#include "setup.hxx"
#include "root_setup.hxx"

const static int StartRun =     0;
const static int EndRun   =  6000;

int main(int argc,char *argv[]){
  char buff1[200],buff2[200];
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  root_setup froot_setup;

  char FileName[300];
  char RootFileName[300];
  sprintf( RootFileName, "default.root" );
  Int_t  year   = 2009;
  Int_t  month  =   11;
  Int_t c=-1;


  while ((c = getopt(argc, argv, "y:m:f:")) != -1) {
    switch(c){
    case 'f':
      sprintf( RootFileName, "%s", optarg );
      break;
    case 'y':
      year  = atoi( optarg );
      break;
    case 'm':
      month = atoi( optarg );
      break;
    }
  }
  FileStat_t    fs;

  char MeanFileName[300];
  double fGainMean[14][2][15][24];
  double fNoiseMean[14][2][15][24];
  double fCalcGainMean[14][2][15][24]  = {0};
  double fCalcNoiseMean[14][2][15][24] = {0};

  int entry = 0;
  sprintf(MeanFileName, 
	  "/home/daq/data/daily_check/mean_%d%02d.txt", 
	  year, month); 
  if( gSystem -> GetPathInfo( MeanFileName, fs ) ){
    cout<< "Cannot open file "
	<< MeanFileName
	<<endl;
    cout<< "this is the first time to analyze "
	<< year <<"\t"
	<< month<<endl;
  }
  else{
    ifstream mfile(MeanFileName);
    int mod, view, pln, ch;
    double meang, meann;
    while( mfile >> mod >> view >> pln >> ch >> meang >> meann ){
      fGainMean[mod][view][pln][ch]  = meang;
      fNoiseMean[mod][view][pln][ch] = meann;
 
    }
  }


  char LogFileName[300];
  sprintf(LogFileName, 
	  "/home/daq/data/daily_check/log_%d%02d.txt", 
	  year, month); 
  if( gSystem -> GetPathInfo( LogFileName, fs ) ){
    cout<< "Cannot open file "
	<< LogFileName
	<<endl;
    exit(1);
  }
  ifstream   logfile( LogFileName );

  //##### Make Root File ##########
  //###############################
  TFile*   rfile  = new TFile(RootFileName, "recreate");
  TH1F*   h       = new TH1F("h","h",10,0,10);
  TH2F*   fHGain  = new TH2F ("fHGain", "fHGain",
			       31, 0, 31,
			      200, 0, 20);
  TH2F*   fHNoise = new TH2F ("fHNoise", "fHNoise",
			       31,   0,    31,
			      475, 0.1,    2.);

  TH2F*   fHGainDiff  = new TH2F ("fHGainDiff", "fHGainDiff",
			       31,     0,  31,
			       200, -0.2, 0.2);
  TH2F*   fHGainDiffH  = new TH2F ("fHGainDiffH", "fHGainDiffH",
			       31,     0,  31,
			       200, -0.2, 0.2);
  TH2F*   fHGainDiffV  = new TH2F ("fHGainDiffV", "fHGainDiffV",
			       31,     0,  31,
			       200, -0.2, 0.2);
  TH2F*   fHNoiseDiff = new TH2F ("fHNoiseDiff", "fHNoiseDiff",
			       31,    0,    31,
			      200, -0.2,   0.2);


  ifstream   gfile;
  ifstream   nfile;
  Int_t       nRun;
  Int_t       nDay;
  Double_t    gate;
  while( logfile >> nDay >> nRun >> gate ){
    cout<<"Day :"    << nDay
	<<"\tRun# :" << nRun
	<<endl;
    //###### Gain File Reading #########
    //###### And Fill Gain 2D Hist #####
    sprintf( FileName,
	     "/home/daq/data/MPPC_calib/ingrid_%08d_0000.txt",
	     nRun);
    if( gSystem -> GetPathInfo( FileName, fs ) ){
      cout<<"Caution"<<endl;
      cout << "Cannot open file "
	   << FileName
	   << endl;
      cin.get();
    } // Not Exist File
    else {
      gfile.  open( FileName );
      Double_t        gain;
      Double_t    pedestal;
      Int_t            mod;
      Int_t           view;
      Int_t            pln;
      Int_t             ch;
      while( gfile 
	     >> mod >> view >> pln >> ch
	     >> pedestal    >> gain
	     ){
	if( gain > 13 ){
	  cout<<"Mod :"  <<mod
	      <<"\tView :" <<view
	      <<"\tPln:"   <<pln
	      <<"\tCh:"    <<ch
	      <<"\tgain:"  <<gain
	      <<endl;
	}
	fHGain -> Fill(nDay, gain);
	double diff = 1.0 * ( gain - fGainMean[mod][view][pln][ch] ) / fGainMean[mod][view][pln][ch];
	fHGainDiff -> Fill(nDay, diff);
	if(mod>=7)fHGainDiffV -> Fill(nDay, diff);
	if(mod<=6)fHGainDiffH -> Fill(nDay, diff);
	fCalcGainMean[mod][view][pln][ch] += gain;
	
      }
      gfile.   close();
      entry++;
    } // Exist File
    //###### Noise File Reading ########
    //###### And Fill Noise 2D Hist ####
    sprintf( FileName,
	     "/home/daq/data/MPPC_calib/ingrid_%08d_0001.txt",
	     nRun);
    if( gSystem -> GetPathInfo( FileName, fs ) ){
      cout<<"Caution"<<endl;
      cout << "Cannot open file "
	   << FileName
	   << endl;
      cin.get();
    } // Not Exist File
    else {
      nfile.  open( FileName );
      Double_t         caa;
      Double_t       nnoise;      
      Double_t       noise;
      Int_t            mod;
      Int_t           view;
      Int_t            pln;
      Int_t             ch;
      while( nfile 
	     >> mod >> view >> pln >> ch
	     >> noise    >> caa
	     ){
	nnoise = 1.0 * noise  * 800.0 / gate; //###### Caution #######
	if( nnoise > 1.2 ){
	  cout<<"Mod :"  <<mod
	      <<"\tView :" <<view
	      <<"\tPln:"   <<pln
	      <<"\tCh:"    <<ch
	      <<"\tnoise:" <<noise
	      <<"\tnnoise:" <<nnoise
	      <<endl;
	}
	fHNoise -> Fill(nDay, nnoise);
	double diff = 1.0 * ( nnoise - fNoiseMean[mod][view][pln][ch] ) / fNoiseMean[mod][view][pln][ch];
	fHNoiseDiff -> Fill(nDay, diff);
	fCalcNoiseMean[mod][view][pln][ch] += nnoise;
      }
      nfile.   close();
    } // Exist File
  }//nRun Loop


  ofstream mmfile(MeanFileName);
  for(int mod=0; mod<14; mod++){
    for(int view=0; view<2; view++){
      for(int pln=0; pln<15; pln++){
	for(int ch=0; ch<24; ch++){
	  mmfile   << mod  << "\t"
		   << view << "\t"
		   << pln  << "\t"
		   << ch   << "\t"
		   << fCalcGainMean[mod][view][pln][ch]/entry << "\t"
		   << fCalcNoiseMean[mod][view][pln][ch]/entry << endl;
	}
      }
    }
  }
  mmfile.close();

  //######## Write ###########
  //##########################
  fHGain  -> SetXTitle("Day");
  fHGain  -> SetYTitle("gain[ADC counts]");
  sprintf(buff1, "Gain%d_%d", year, month);
  fHGain  -> SetName(buff1);
  sprintf(buff1, "Gain Stability y%d m%2d", year, month);
  fHGain  -> SetTitle(buff1);
  fHNoise -> SetXTitle("Day");
  fHNoise -> SetYTitle("noise[MHz]");
  sprintf(buff1, "Noise%d_%d", year, month);
  fHNoise -> SetName(buff1);
  sprintf(buff1, "Noise Stability y%d m%2d", year, month);
  fHNoise -> SetTitle(buff1);
  h       -> Write();
  fHGain  -> Write();
  fHNoise -> Write();
  rfile   -> Write();
  rfile   -> Close();
}
