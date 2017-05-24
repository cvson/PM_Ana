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

  sprintf(buff1,"/home/daq/data/cosmictest/ingrid_%08d_cosmic2.root",run_number);
  FileStat_t fs;
  if(gSystem->GetPathInfo(buff1,fs)){
    cout<<"Cannot open file "<<buff1<<endl;
    exit(1);
  }

  TFile *f = new TFile(buff1,"read");
  sprintf(buff1,"/home/daq/data/cosmictest/ingrid_%08d_cosmic_veto.txt",run_number);
  ofstream file(buff1);
  //read hist
  cout<<"read hist..."<<endl;
  TH1F *fHAdc[4][22];
  cout<<"module?"<<endl;
  int anamod;
  cin>>anamod;
  TCanvas *c1 = new TCanvas("c1","c1",10,10,800,800);
  c1->Divide(2,2);
  Int_t count=1;
  double mostless=100;
  TH1F *fHly = new TH1F("fHly","fHly",80,15,55);
  for(Int_t veto=0;veto<4;veto++){
    for(Int_t numch=0;numch<22;numch++){
      if(veto==0)sprintf(buff1,"Mod%02dRVetoCh%02d",anamod,numch);
      if(veto==1)sprintf(buff1,"Mod%02dLVetoCh%02d",anamod,numch);
      if(veto==2)sprintf(buff1,"Mod%02dBVetoCh%02d",anamod,numch);
      if(veto==3)sprintf(buff1,"Mod%02dUVetoCh%02d",anamod,numch);
      fHAdc[veto][numch] = (TH1F*)f->Get(buff1);
      Int_t nEnt = fHAdc[veto][numch]->GetEntries();
      Double_t fLY  = fHAdc[veto][numch]->GetMean();
	if(nEnt<24){
	  if(veto==0)cout<<"Event less RVeto Ch:"<<numch<<" ent:"<<nEnt<<endl;
	  if(veto==1)cout<<"Event less LVeto Ch:"<<numch<<" ent:"<<nEnt<<endl;
	  if(veto==2)cout<<"Event less BVeto Ch:"<<numch<<" ent:"<<nEnt<<endl;
	  if(veto==3)cout<<"Event less UVeto Ch:"<<numch<<" ent:"<<nEnt<<endl;
	}
	else if(fLY<16){
	  if(veto==0)cout<<"Event less RVeto Ch:"<<numch<<" ent:"<<nEnt<<" ly:"<<fLY<<endl;
	  if(veto==1)cout<<"Event less LVeto Ch:"<<numch<<" ent:"<<nEnt<<" ly:"<<fLY<<endl;
	  if(veto==2)cout<<"Event less BVeto Ch:"<<numch<<" ent:"<<nEnt<<" ly:"<<fLY<<endl;
	  if(veto==3)cout<<"Event less UVeto Ch:"<<numch<<" ent:"<<nEnt<<" ly:"<<fLY<<endl;
	}
	else{
	  fHly->Fill(fLY);
	  file<<fLY<<endl;
	  if(fLY<mostless)mostless=fLY;
	}
	if(flWrite){
	  c1->cd(count);
	  fHAdc[veto][numch]->Draw();
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

  c1->cd();
  c1->Clear();
  fHly->Draw();

  c1->Update();
  sprintf(buff1,"ingrid_%08d_LYVeto.pdf",run_number);
  c1->Print(buff1);
  cin.get();
  cin.get();
  cout<<mostless<<endl;
  f->Close();
}
