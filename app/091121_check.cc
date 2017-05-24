#include<iostream>
#include<vector>
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
#include "analysis_cosmic.hxx"
#include "setup.hxx"
//#include "root_setup.hxx"
Int_t             fMod;
Long_t            fTime;
Int_t            Cycle;
vector<int>*       adc;
vector<int>*      view;
vector<int>*       pln;
vector<int>*        ch;
vector<double>*     pe;
vector<int>*      nsec;
 

vector<double>  anape;
vector<int>   anaview;
vector<int>    anapln;
vector<int>     anach;

int main(int argc,char *argv[]){
  fINGRID_Dimension = new INGRID_Dimension();

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
  Int_t        NEvt       = tree->GetEntries();
  cout<<"---- # of Evt    = "<<NEvt<<"---------"<<endl;
  cin.get();
  Int_t           NumEvt;
  Int_t           nSpill;
  Long_t          UTime;
  pe   = 0;
  nsec = 0;
  view = 0;
  pln  = 0;
  ch   = 0;

  tree->SetBranchAddress("NumEvt",&NumEvt   );
  tree->SetBranchAddress("adc"  ,&adc   );
  tree->SetBranchAddress("pe"   ,&pe   );
  tree->SetBranchAddress("nsec" ,&nsec );
  tree->SetBranchAddress("view" ,&view );
  tree->SetBranchAddress("pln"  ,&pln  );
  tree->SetBranchAddress("ch"   ,&ch   );
  tree->SetBranchAddress("fMod" ,&fMod );
  tree->SetBranchAddress("Cycle",&Cycle);
  tree->SetBranchAddress("nSpill",&nSpill);
  tree->SetBranchAddress("UTime",&UTime);

  vector<Hit> allhit;

  while(1){
    Int_t n=0;
    cout<<"Entry?"<<endl;
    cin>>n;
    tree->GetEntry(n);
    cout<<"-----------------    "
	<<"Event # :"<<NumEvt<<endl
	<<"Spill # :"<<nSpill<<endl
	<<"UTime : " <<UTime <<endl
	<<"Mod  # : "<<fMod <<endl
	<<"    -----------------"<<endl;
   Int_t nHit = pe->size();

    //######### fill hit class ####################

   allhit.clear();
   for(Int_t i=0;i<nHit;i++){
     Hit hit  ;
     hit.pe   = pe  ->at(i);
     hit.time = nsec->at(i);
     hit.view = view->at(i);
     hit.pln  = pln ->at(i);
     hit.ch   = ch  ->at(i);
     allhit.push_back(hit);
   }
   vector<Hit> hitclster;
   while(fFindTimeClster(allhit, hitclster, fTime)){
     fSortTime(hitclster);
     Int_t nCls = hitclster.size();
      
     //Print(hitclster);
     fSortTime(allhit);
     Draw_Module();
     Draw_Hit(hitclster);
     Print(hitclster);
   }
  }//
  f->Close();
}
