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

  char temp[100];

  sprintf(buff1,"/home/daq/data/cosmictest/ingrid_%08d_cosmic.root",run_number);
  FileStat_t fs;
  if(gSystem->GetPathInfo(buff1,fs)){
    cout<<"Cannot open file "<<buff1<<endl;
    exit(1);
  }

  TFile *f = new TFile(buff1,"read");

  sprintf(buff1,"/home/daq/data/cosmictest/ingrid_%08d_cosmic.txt",run_number);
  ofstream file(buff1);
  //read hist
  cout<<"read hist..."<<endl;
  TH1F *fHAdc[2][11][24];
  cout<<"module?"<<endl;
  int anamod;
  cin>>anamod;
  TCanvas *c1 = new TCanvas("c1","c1",10,10,800,800);
  c1->Divide(2,2);
  Int_t count=1;
  double mostless=100;
  TH1F *fHly = new TH1F("fHly","fHly",80,15,55);
  for(Int_t view=0;view<2;view++){
    for(Int_t numtpl=0;numtpl<11;numtpl++){
      for(Int_t numch=0;numch<24;numch++){
	if(view==0)sprintf(buff1,"Mod%02dXTPL%02dCh%02d",anamod,numtpl,numch);
	if(view==1)sprintf(buff1,"Mod%02dYTPL%02dCh%02d",anamod,numtpl,numch);
	fHAdc[view][numtpl][numch] = (TH1F*)f->Get(buff1);
	Int_t nEnt = fHAdc[view][numtpl][numch]->GetEntries();
	Double_t fLY  = fHAdc[view][numtpl][numch]->GetMean();
	if(nEnt<24){
	  if(view==0)cout<<"Event less X TPL:"<<numtpl<<" Ch:"<<numch<<" ent:"<<nEnt<<endl;
	  if(view==1)cout<<"Event less Y TPL:"<<numtpl<<" Ch:"<<numch<<" ent:"<<nEnt<<endl;
	}
	else if(fLY<16){
	  if(view==0)cout<<"LY less X TPL:"<<numtpl<<" Ch:"<<numch<<" ent:"<<nEnt<<" ly:"<<fLY<<endl;
	  if(view==1)cout<<"LY less Y TPL:"<<numtpl<<" Ch:"<<numch<<" ent:"<<nEnt<<" ly:"<<fLY<<endl;
	}
	else{
	  fHly->Fill(fLY);
	  file<<fLY<<endl;
	  if(fLY<mostless)mostless=fLY;
	}
	if(flWrite){
	  c1->cd(count);
	  fHAdc[view][numtpl][numch]->Draw();
	  count++;
	  if(count==5){
	    c1->Update();
	    cin.get();
	    c1->Clear();
	    c1->Divide(2,2);
	    count=1;
	  }
	}
      }//numch
    }//numtfb
  }//nummod
  c1->cd();
  c1->Clear();
  fHly->Draw();

  c1->Update();
  sprintf(buff1,"ingrid_%08d_LY.pdf",run_number);
  c1->Print(buff1);
  cin.get();
  cin.get();
  cout<<mostless<<endl;
  f->Close();
}
