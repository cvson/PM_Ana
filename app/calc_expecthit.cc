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

double get_expected(Int_t entry, Int_t gate, double noise, double caa){
  double u = noise * gate / 1000;
  
  //double expected = //entry * 
  double expec =   ( 1 - exp(-u) 
                     - u * exp(-u) * (1 - caa * caa / (1 - caa)) 
                     - u * u / 2 * exp(-u) * (1 - caa / (1 - caa))
		   );
  //cout<<expected<<endl;
  return expec;

}

int main(int argc,char *argv[]){
  char buff1[200],buff2[200];
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  root_setup froot_setup;

  //read run number
  Int_t run_number;
  Int_t c=-1;
  char FileName[300];
  Bool_t flWrite=false;
  Int_t ent;
  while ((c = getopt(argc, argv, "r:hwb:")) != -1) {
    switch(c){
    case 'r':
      run_number=atoi(optarg);
      break;
    case 'b':
      ent=atoi(optarg);
      break;
    case 'w':
      flWrite=true;
      break;
    case 'h':
      cout<<"help menu"<<endl;
      cout<<"-r [run_number]"<<endl;
      break;
    }
  }

  char temp[100];

  sprintf(buff1,"/home/daq/data/MPPC_calib/ingrid_%08d_0001.txt",run_number);
  FileStat_t fs;
  if(gSystem->GetPathInfo(buff1,fs)){
    cout<<"Cannot open file "<<buff1<<endl;
    exit(1);
  }
  ifstream file(buff1);

  double noise[14][2][15][24];
  double caa  [14][2][15][24];
  int mod,view,pln,ch;
  double tmp;
  while(file>>mod>>view>>pln>>ch){
    file>>noise[mod][view][pln][ch]>>caa[mod][view][pln][ch];
    
  }

  TFile *f = new TFile("/home/daq/expect.root","recreate");
  TH2F *fHChActMap[14][2];
  for(Int_t nummod=0;nummod<14;nummod++){
    for(Int_t view=0;view<2;view++){
      if(view==0)sprintf(buff1,"Mod%02dXActMap",nummod);
      if(view==1)sprintf(buff1,"Mod%02dYActMap",nummod);
      fHChActMap[nummod][view] = new TH2F(buff1,buff1,11,0,11,24,0,24);
    }
  } 
  for(Int_t nummod=0;nummod<14;nummod++){
    for(Int_t view=0;view<2;view++){
      for(Int_t numtpl=0;numtpl<11;numtpl++){
	for(Int_t numch=0;numch<24;numch++){
	  double exp = get_expected(ent, 1500, noise[nummod][view][numtpl][numch], caa[nummod][view][numtpl][numch]);
	  double r = noise[nummod][view][numtpl][numch]*caa[nummod][view][numtpl][numch]*caa[nummod][view][numtpl][numch];
	  fHChActMap[nummod][view]->Fill(numtpl,numch,exp);
	  if(nummod==7&&view==0)cout<<numtpl<<" "<<numch<<" "<<exp<<endl;
	}
      }
    }
  }
  f->Write();
  f->Close();

}
