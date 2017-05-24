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
//##### INGRID Library #########
#include "INGRIDEVENTSUMMARY.h"
#include "IngridHitSummary.h"
#include "IngridSimHitSummary.h"

const static int CosmicStartCyc = 14;
const static int CosmicEndCyc   = 15;

int main(int argc,char *argv[]){
  TROOT        root ("GUI","GUI");
  TApplication app  ("App",0,0);
  FileStat_t fs;
  int run_number;
  int calib_run_number;
  int c=-1;
  int sub_run_number=0;
  while ((c = getopt(argc, argv, "r:s:c:")) != -1) {
    switch(c){
    case 'r':
      run_number=atoi(optarg);
      calib_run_number=run_number;
      break;
    case 's':
      sub_run_number=atoi(optarg);
      break;
    case 'c':
      calib_run_number=atoi(optarg);
      break;
    }
  }
  char buff[300];




  //#### Read file before calibration ######
  //########################################
  IngridEventSummary* summary = new IngridEventSummary();
  sprintf(buff,"/home/daq/data/cosmic/ingrid_%08d_%04d_calib.root",
	  run_number, sub_run_number);
  if(gSystem->GetPathInfo(buff,fs)){
    cout<<"Cannot open file "<<buff<<endl;
    exit(1);
  }
  TFile*            rfile = new TFile(buff,"read");
  TTree*             tree = (TTree*)rfile -> Get("tree");
  TBranch*          EvtBr = tree->GetBranch("fDefaultReco.");
  EvtBr                   ->SetAddress(&summary);
  tree                    ->SetBranchAddress("fDefaultReco.", &summary);
  int                nevt = (int)tree -> GetEntries();
  cout << "Total # of events = " << nevt <<endl;

  //#### Create file after calibration ######
  //#########################################
  sprintf(buff,"/home/daq/data/cosmic/ingrid_%08d_%04d_reduc.root",
	  run_number, sub_run_number);
  TFile*            wfile = new TFile(buff,"recreate");
  TTree*            wtree = new TTree("tree","tree");
  IngridEventSummary* wsummary = new IngridEventSummary(); 
  wtree              -> Branch   ("fDefaultReco.","IngridEventSummary", 
				 &wsummary,  64000,   99);


  for(int ievt = 0; ievt < nevt; ++ievt){
    if(ievt%1000==0)cout<<"Run#"<<run_number<<"\tcalib event:"<<ievt<<endl;
    summary -> Clear();
    wsummary-> Clear();
    tree    -> GetEntry(ievt);
    for(int mod=0; mod<16; mod++){
      for(int cyc=CosmicStartCyc; cyc<=CosmicEndCyc; cyc++){
        int ninghit = summary -> NIngridModHits(mod, cyc);
        for(int i=0; i<ninghit; i++){

	  IngridHitSummary *inghitsum;
	  inghitsum   = (IngridHitSummary*) (summary -> GetIngridModHit(i, mod, cyc) );
	  wsummary   -> AddIngridModHit( inghitsum, mod, cyc );
	}

      }//Cyc
    }//Mod

    wtree -> Fill();
    if(ievt%1000==0)wtree->AutoSave();
  }


  wtree -> Write();
  wfile -> Write();
  wfile -> Close();
  //app.Run();
}
 
