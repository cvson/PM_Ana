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
  while ((c = getopt(argc, argv, "r:hw")) != -1) {
    switch(c){
    case 'r':
      run_number=atoi(optarg);
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
  int anamod;
  cout<<"module?"<<endl;
  cin>>anamod;

  sprintf(buff1,"/home/daq/data/MPPC_calib/ingrid_%08d_0000.txt",run_number);
  FileStat_t fs;
  if(gSystem->GetPathInfo(buff1,fs)){
    cout<<"Cannot open file "<<buff1<<endl;
    exit(1);
  }
  ifstream file(buff1);
  //read hist
  int mod, view, pln, ch;
  double gain, pedestal, temp;
  TH1F*  fHped  = new TH1F("fHped","fHped",180,110,200);
  TH1F*  fHgain = new TH1F("fHgain","fHgain",100,4,16);
  while(file>>mod>>view>>pln>>ch){
    if(mod==anamod){
      file>>pedestal>>gain;
      if(fabs(10-gain)>7){
	if(view==0)cout<<"X Pln:"<<pln<<" Ch:"<<ch<<" gain:"<<gain<<endl;
	if(view==1)cout<<"Y Pln:"<<pln<<" Ch:"<<ch<<" gain:"<<gain<<endl;
      }
      else{
	fHped  -> Fill(pedestal);
	fHgain -> Fill(gain);
      }
    }
    else{
      file>>temp>>temp;
    }
  }

  TCanvas *c1 = new TCanvas("c1","c1",10,10,500,700);
  c1->Divide(1,2);
  c1->cd(1);
  fHped->Draw();
  c1->cd(2);
  fHgain->Draw();
  c1->Update();
  cin.get();
  cin.get();
}
