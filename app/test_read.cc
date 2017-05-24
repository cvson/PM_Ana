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

int main(int argc,char *argv[]){
  TROOT        root ("GUI","GUI");
  TApplication app  ("App",0,0);


  Int_t run_number;
  Int_t c=-1;  char FileName[300];
  while ((c = getopt(argc, argv, "r:f:")) != -1) {
    switch(c){
    case 'r':
      run_number=atoi(optarg);
      sprintf(buff1,"/home/daq/data/dst/ingrid_%08d_0000_processed.root",run_number);
      break;
    case 'f':
      sprintf(buff1,"%s",optarg);
      run_number=77;
      break;

    }
  }
  FileStat_t fs;
  ifstream timing;
  //######## read root file #############################
  //#####################################################


  cout<<"reading "<<buff1<<"....."<<endl;
  if(gSystem->GetPathInfo(buff1,fs)){
    cout<<"Cannot open file "<<buff1<<endl;
    exit(1);
  }



  IngridEventSummary* evt = new IngridEventSummary();
  TFile*            rfile = new TFile("test.root","read");
  TTree*             tree = (TTree*)rfile -> Get("tree");
  TBranch*          EvtBr = tree->GetBranch("fDefaultReco.");
  EvtBr                   ->SetAddress(&evt);
  tree                    ->SetBranchAddress("fDefaultReco.", &evt);

  int                nevt = (int)tree -> GetEntries();
  cout << "Total # of events = " << nevt <<endl;
  for(int ievt = 0; ievt < nevt; ++ievt){
    evt  -> Clear();
    tree -> GetEntry(ievt);
    //##### Read IngridHitSummary Class #######
    cout << evt -> run <<endl;
    IngridHitSummary* inghitsum;
    for(int cyc=0; cyc<23; cyc++){
      for(int mod=0; mod<16; mod++){
        int ninghit = evt -> NIngridModHits(mod, cyc);
        cout<<"Mod# "<<mod<<"\t Hit ="<<ninghit<<endl;
        for(int i=0; i<ninghit; i++){
          inghitsum   = (IngridHitSummary*) (evt -> GetIngridModHit(i, mod, cyc\
) );
          cout << "Event# :" << ievt
               << "Cyc# :"   << cyc
               << "Mod# :"   << mod
               << "view :"   << inghitsum -> view
               << endl;
        }
      }
    }
  }

  rfile -> Close();
  //app.Run();
}
 
