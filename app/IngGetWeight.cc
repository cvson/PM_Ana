//#define TEST 1
//##### Standard C++ lib. ######
#include<iostream>
#include<sstream>
#include<fstream>
using namespace std;
#include <iomanip.h>
#include <sys/stat.h>
#include "math.h"
//##### Root Library ###########
#include <TROOT.h>
#include <TStyle.h>
#include <TH1.h>
#include <TH2.h>
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
#include "BeamInfoSummary.h"


int main(int argc,char *argv[]){
  TROOT        root ("GUI","GUI");
  TApplication app  ("App",0,0);

  Int_t c=-1;
  char FileName[300], Output[300], WeightFile[300];
  int Nweights;
  float Nevt[100];
  IngridBasicReconSummary* ingbasic;
  sprintf(Output,  "nevent.txt");
  sprintf(WeightFile,"weight.root");
  sprintf(FileName,"temp.root");
  Nweights=0;
  memset(Nevt,0,sizeof(Nevt));
  while ((c = getopt(argc, argv, "f:w:o:")) != -1) {
    switch(c){
    case 'f':
      sprintf(FileName,"%s",optarg);
      break;
    case 'w':
      sprintf(WeightFile,"%s",optarg);
      break;
    case 'o':
      sprintf(Output,"%s",optarg);
      break;
    }
  }

  FileStat_t fs;
  cout<<"reading and processsing"<<FileName<<"....."<<endl;
  if(gSystem->GetPathInfo(FileName,fs)){
    cout<<"Cannot open file "<<FileName<<endl;
    exit(1);
  }

  //#### Read ROOT File after basic reconstruction ######
  //#####################################################
  TFile*            rfile = new TFile(FileName,"read");
  TTree*             tree = (TTree*)rfile -> Get("tree");
  TBranch*          EvtBr = tree->GetBranch("fDefaultReco.");
  IngridEventSummary* evt = new IngridEventSummary();
  EvtBr                   ->SetAddress(&evt);
  tree                    ->SetBranchAddress("fDefaultReco.", &evt);
  int                nevt = (int)tree -> GetEntries();


  TFile*            wfile = new TFile(WeightFile,"read");
  TTree*      weightstree = (TTree*)wfile -> Get("weightstree");
  int            nweights;
  TArrayF        *weights;
  TBranch     *b_nweights;
  TBranch      *b_weights;
  weightstree            -> SetBranchAddress("nweights", &nweights, &b_nweights);
  weightstree            -> SetBranchAddress("weights", &weights, &b_weights);
  int               nevt2 = (int)weightstree -> GetEntries();


  if(nevt!=nevt2){
    cout<<"Number of events it does not match."<<endl;
    exit(1);
  }

  weightstree -> GetEntry(0);
  Nweights = nweights;

  for(int ievt=0; ievt<nevt; ievt++){
    if(ievt%10000==0)cout << ievt << endl;
    evt         -> Clear();
    tree        -> GetEntry(ievt);
    weightstree -> GetEntry(ievt);

    for(int ibasic = 0; ibasic < evt->NIngridBasicRecons(); ibasic++){
      ingbasic     = (IngridBasicReconSummary*)(evt->GetBasicRecon(ibasic));

      if(/*ingbasic->hastrk && ingbasic->matchtrk &&*/ 
	 /*ingbasic->ontime &&*/ !(ingbasic->vetowtracking) &&
				 !(ingbasic->edgewtracking) ){	
	for(int i=0; i<Nweights; i++){
	  Nevt[i]+=weights->At(i);
	}
      }      
    }
  }//ievt


  ofstream ofile(Output);
  for(int i=0; i<Nweights; i++){
    ofile << Nevt[i] << endl;
  }


}
