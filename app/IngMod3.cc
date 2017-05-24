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
#include "IngridSimVertexSummary.h"
#include "BeamInfoSummary.h"

int main(int argc,char *argv[]){
  TROOT        root ("GUI","GUI");
  TApplication app  ("App",0,0);
  int c=-1;
  char FileName[300], Output[300];
  while ((c = getopt(argc, argv, "f:o:")) != -1) {
    switch(c){

    case 'f':
      sprintf(FileName,"%s",optarg);
      break;
    case 'o':
      sprintf(Output  ,"%s",optarg);
      break;
    }
  }


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
  IngridEventSummary* wsummary = new IngridEventSummary(); 
  wtree              -> Branch   ("fDefaultReco.","IngridEventSummary", 
				 &wsummary,  64000,   99);

  int mod;

  for(int ievt = 0; ievt < nevt; ++ievt){
  
    if(ievt%1000==0)cout<<"Event:"<<ievt<<endl;

    summary -> Clear();
    wsummary-> Clear();
    tree    -> GetEntry(ievt);

    if((summary -> NIngridBeamSummarys()) > 0){
      BeamInfoSummary* binfo =  (BeamInfoSummary*)( summary -> GetBeamSummary(0) );
      wsummary -> AddBeamSummary(binfo);
    }

    if((summary -> NIngridSimVertexes()) > 0){
      IngridSimVertexSummary* simver =  (IngridSimVertexSummary*)( summary -> GetSimVertex(0) );
      wsummary -> AddSimVertex(simver);
    }

    for( int ibrec=0; ibrec < summary -> NIngridBasicRecons(); ibrec++ ){
      IngridBasicReconSummary* ingbasrec =  (IngridBasicReconSummary*)( summary -> GetBasicRecon(ibrec) );

      mod    = ingbasrec->hitmod;
      if(mod==3)
	wsummary -> AddBasicRecon(ingbasrec);
    }

    wtree -> Fill();
  }
  
  wtree -> Write();
  wfile -> Write();
  wfile -> Close();
}
