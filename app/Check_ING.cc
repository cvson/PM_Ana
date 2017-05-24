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
#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TApplication.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TClonesArray.h>
#include <TObject.h>
#include <TEventList.h>
#include <TBranch.h>
#include <TSystem.h>
#include <TBrowser.h>
//##### INGRID Library #########
#include "INGRIDEVENTSUMMARY.h"
#include "IngridHitSummary.h"
#include "IngridSimHitSummary.h"
#include "IngridBasicReconSummary.h"


int main(int argc,char *argv[]){
  TROOT        root ("GUI","GUI");
  TApplication app  ("App",0,0);
  Int_t c=-1;  
  char FileName[300];

  while ((c = getopt(argc, argv, "f:")) != -1) {
    switch(c){
    case 'f':
      sprintf(FileName,"%s", optarg);
      break;
    }
  }
  FileStat_t fs;
  cout<<"reading "<<FileName<<"....."<<endl;
  if(gSystem->GetPathInfo(FileName,fs)){
    cout<<"Cannot open file "<<FileName<<endl;
    exit(1);
  }
  double total_pot=0;  
  TFile*            rfile = new TFile(FileName,"read");
  TTree*             tree = (TTree*)rfile -> Get("tree");
  TBranch*          EvtBr = tree->GetBranch("fDefaultReco.");
  IngridEventSummary* evt = new IngridEventSummary();
  EvtBr                   ->SetAddress(&evt);
  tree                    ->SetBranchAddress("fDefaultReco.", &evt);
  int                nevt = (int)tree -> GetEntries();
  cout << "Total # of events = " << nevt <<endl;
  BeamInfoSummary*          beamsum;

  ofstream wfile("ING_GoodSpill.txt");
  for(int ievt = 0; ievt < nevt; ++ievt){
    if(ievt%10000==0)
      cout << "check event# :" << ievt
	   << "\tpot:"         << total_pot
	   << endl;
  
    evt  -> Clear();
    tree -> GetEntry(ievt);
  
    beamsum    = (BeamInfoSummary*) ( evt -> GetBeamSummary( 0 ) );
    wfile << evt->run     << "\t"
	  << beamsum->spillnum   << "\t"
	  << endl;
    total_pot += beamsum->ct_np[4][0];
  } 
  
  rfile -> Close();
 

  cout << "----- Summary ----------------------------------"    << endl;
  cout << "\tTotal pot measured by CT05 = " << total_pot << endl;
  cout << "------------------------------------------------"    << endl;

}
 
