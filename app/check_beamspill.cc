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
#include <TClonesArray.h>
#include <TObject.h>
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

#include "TApplication.h"


int main(int argc,char *argv[]){
  char buff1[200],buff2[200];
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  Int_t run_number;
  Int_t c=-1;  char FileName[300];
  bool cosmicflag=false;
  while ((c = getopt(argc, argv, "r:h")) != -1) {
    switch(c){
    case 'r':
      run_number=atoi(optarg);
      break;
    case 'h':
      cout<<"help menu"<<endl;
      cout<<"-r [run_number]"<<endl;
      break;
    }
  }

  //sprintf(buff1,"/home/daq/data/test/ingrid.root");
  sprintf(buff1,"/home/daq/data/root_new/ingrid_%08d_0000.root",run_number);
  FileStat_t fs;
  if(gSystem->GetPathInfo(buff1,fs)){
    cout<<"Cannot open file "<<buff1<<endl;
    exit(1);
  }


  TFile*               f  = new TFile(buff1,"read");
  TTree*            tree  = (TTree*)f->Get("tree");
  Int_t        Evt        = tree->GetEntries();
  cout<<"---- Start Spill Check  ! ---------"<<endl;
  cin.get();
  Int_t  nSpill;
  Int_t  nLastSpill = -1;
  Long_t UTime;
  tree->SetBranchAddress("nSpill",&nSpill);
  tree->SetBranchAddress("UTime" ,&UTime);
  Long_t ttime=0;
  for(int n=0; n<Evt; n++){
    tree->GetEntry(n);
    //if(ttime!=UTime){
      cout<<"Spill# :"<<nSpill<<endl;
      if( nLastSpill != -1){
	if(  nSpill >=  nLastSpill + 2 ) {
	  cout<<"caution!!"<<endl;
	  cout<<nSpill<<endl;
	  cin.get();
	}
      }

      nLastSpill = nSpill;
      ttime=UTime;
      //}
  }
  //######## Write and Close ####################
  f       -> Close();
}
 
