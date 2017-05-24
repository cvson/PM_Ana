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
//#include <TRandom3.h>
#include <TMath.h>
//##### INGRID Library #########
#include "INGRIDEVENTSUMMARY.h"
#include "IngridHitSummary.h"
#include "IngridSimHitSummary.h"
//##### INGRID Software ########
//#include "INGRID_Dimension.cxx"


//const static int NDUMMY= 5; //### Mean Value of Poisson

int main(int argc,char *argv[]){
  TROOT        root ("GUI","GUI");
  TApplication app  ("App",0,0);

  //int run_number;
  //int calib_run_number;
  int c=-1;
  //int sub_run_number;
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


  //TRandom3 r; //### random number sheed
  //TF1*     fHtime = new TF1("fHtime","-0.096*x*x+133.3*x-33159", 320, 750);
  for(int ievt = 0; ievt < nevt; ++ievt){
    //for(int ievt = 0; ievt <1000; ++ievt){
  
    if(ievt%1000==0)cout<<"\tadd noise event:"<<ievt<<endl;
    summary -> Clear();
    wsummary-> Clear();
    tree    -> GetEntry(ievt);
    int ninghit=0;
    for(int mod=0; mod<17; mod++){
      for(int cyc=0; cyc<23; cyc++){
	ninghit += summary -> NIngridModHits(mod, cyc);
      }
    }
    if(ninghit==0)continue;    
    wsummary = summary;
    wtree -> Fill();


  }

  wtree -> Write();
  wfile -> Write();
  wfile -> Close();
  //app.Run();
}
 
