//##### Standard C++ lib. ######
#include<iostream>
#include<sstream>
#include<fstream>
using namespace std;
#include <iomanip.h>
#include <sys/stat.h>
//##### Root Library ###########
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
#include <TSystem.h>
#include <TF1.h>
#include <TMath.h>
//##### INGRID Library #########
#include "INGRIDEVENTSUMMARY.h"
#include "IngridHitSummary.h"
#include "IngridSimHitSummary.h"
//##### INGRID Software ########
#include "INGRID_Dimension.cxx"

INGRID_Dimension* fINGRID_Dimension;


int main(int argc,char *argv[]){
  TROOT        root ("GUI","GUI");
  TApplication app  ("App",0,0);
  fINGRID_Dimension = new INGRID_Dimension();
  int run_number;
  int calib_run_number;
  int c=-1;
  int sub_run_number;
  int nmodhits[14],nhits;
  char FileName[300], Output[300];
  bool usecyc[8];
  Float_t lppb, hppb;
  Int_t ppbbin=-1;
  while ((c = getopt(argc, argv, "r:f:o:")) != -1) {
    switch(c){
    case 'r':
      ppbbin=atoi(optarg);
      break;
    case 'f':
      sprintf(FileName,"%s",optarg);
      break;
    case 'o':
      sprintf(Output  ,"%s",optarg);
      break;
    }
  }

  if(ppbbin<0||ppbbin>10){
    cout<<"r must be 0 to 10"<<endl;
    exit(1);
  }
  
  lppb=2e12+1e12*ppbbin;
  hppb=lppb+1e12;


  //#### Read file before calibration ######
  //########################################

  FileStat_t fs;
  IngridEventSummary* summary = new IngridEventSummary();

  if(gSystem->GetPathInfo(FileName,fs)){
    cout<<"Cannot open file "<< FileName <<endl;
    exit(1);
  }
  TFile*            rfile = new TFile(FileName,"read");
  TTree*             tree = (TTree*)rfile -> Get("tree");
  TBranch*          EvtBr = tree->GetBranch("fDefaultReco.");
  EvtBr                   ->SetAddress(&summary);
  tree                    ->SetBranchAddress("fDefaultReco.", &summary);
  int                nevt = (int)tree -> GetEntries();
  cout << "Total # of events = " << nevt <<endl;

  //#### Create file after calibration ######
  //#########################################

  TFile*            wfile = new TFile(Output,"recreate");
  TTree*            wtree = new TTree("tree","tree");

  wtree->Branch("nhits",&nhits,"nhits/I");

  BeamInfoSummary* binfo;

  for(int ievt = 0; ievt < nevt; ++ievt){
  
    if(ievt%1000==0)cout<<"Run#"<<run_number<<"\tadd noise event:"<<ievt<<endl;
    summary -> Clear();
    tree    -> GetEntry(ievt);
    binfo = (BeamInfoSummary*)( summary -> GetBeamSummary(0) );
    memset(usecyc,false,sizeof(usecyc));

    for(int i=0;i<8;i++){
      if(binfo->ct_np[4][i+1]>lppb&&binfo->ct_np[4][i+1]<hppb){
	usecyc[i]=true;
      }
    }

    for(int cyc=4; cyc<12; cyc++){
      if(!usecyc[cyc-4])continue;
      int dummycyc=(cyc-4)/2;
      for(int mod=0; mod<14; mod++){
	nhits = summary -> NIngridModHits(mod, dummycyc);
	wtree -> Fill();
      }
    }
  }

  wtree -> Write();
  wfile -> Write();
  wfile -> Close();
  //app.Run();
}
 
