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
#include <TBox.h>
#include <TLatex.h>
#include <TString.h>
#include <TSystem.h>

#include "TApplication.h"

//#include "root_setup.hxx"
#include "INGRIDEVENTSUMMARY.h"
#include "IngridHitSummary.h"
#include "IngridSimHitSummary.h"
#include "IngridSimVertexSummary.h"
#include "IngridSimParticleSummary.h"
#include "BeamInfoSummary.h"
#include "IngridBasicReconSummary.h"



#include "IngBasicRecon.cxx"
#include "IngEvtDisp.cxx"
#include "IngridConstants.h"

int main(int argc,char *argv[]){


  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);

  Int_t c=-1;  char FileName[300];
  char Output[300];
  bool rename = false;
  bool usecyc[8];
  Int_t nMod=14;
  Int_t Scyc=4;
  Int_t Ncyc=12;
  Int_t ppbbin=-1;
  Float_t lppb, hppb;

  while ((c = getopt(argc, argv, "r:f:o:")) != -1) {
    switch(c){
    case 'r':
      ppbbin=atoi(optarg);
      break;
    case 'f':
      sprintf(FileName, "%s",optarg);
      rename = true;
      break;
    case 'o':
      sprintf(Output, "%s",optarg);
      rename = true;
      break;
    

    }
  }

  if(ppbbin<0||ppbbin>10){
    cout<<"r must be 0 to 10"<<endl;
    exit(1);
  }


  lppb=2e12+1e12*ppbbin;
  hppb=lppb+1e12;

  
  FileStat_t fs;
  ifstream timing;
  //######## read root file #############################
  //#####################################################
  cout<<"reading "<<FileName<<"....."<<endl;
  if(gSystem->GetPathInfo(FileName,fs)){
    cout<<"Cannot open file "<<FileName<<endl;
    exit(1);
  }
  IngridEventSummary* evt = new IngridEventSummary();
  TFile*            rfile = new TFile(FileName,"read");
  TTree*             tree = (TTree*)rfile -> Get("tree");
  TBranch*          EvtBr = tree->GetBranch("fDefaultReco.");
  EvtBr                   ->SetAddress(&evt);
  tree                    ->SetBranchAddress("fDefaultReco.", &evt);
  int                nevt = (int)tree -> GetEntries();
  cout << "Total # of events = " << nevt <<endl;

  IngridHitSummary*    inghitsum;
  BeamInfoSummary* binfo;

  TFile*            wfile = new TFile(Output,"recreate");
  TTree*            wtree = new TTree("tree","tree");
  IngridEventSummary* wsummary = new IngridEventSummary(); 
  wtree              -> Branch   ("fDefaultReco.","IngridEventSummary", 
				 &wsummary,  64000,   99);


  for( int ievt = 0; ievt < nevt; ievt++  ){
    //for( int ievt = 0; ievt < 1000; ievt++  ){
    if(ievt%100==0)cout << "analyze event# " << ievt<<endl;
    evt      -> Clear();
    wsummary -> Clear();
    tree -> GetEntry(ievt);

    binfo =  (BeamInfoSummary*)( evt -> GetBeamSummary(0) );

    memset(usecyc,false,sizeof(usecyc));
    for(int i=0;i<8;i++){
      if(binfo->ct_np[5][i]>lppb&&binfo->ct_np[5][i]<hppb)
	usecyc[i]=true;
    }


    wsummary -> AddBeamSummary(binfo);
    
    for( int mod=0; mod<nMod; mod++ ){   //### Module Loop                     
      for( int cyc=Scyc; cyc<Ncyc; cyc++ ){  //### Cycle Loop
	if((!usecyc[cyc-4]))continue;

        int ninghit = evt -> NIngridModHits(mod, cyc);
        for(int i=0; i<ninghit; i++){
          inghitsum = (IngridHitSummary*) (evt -> GetIngridModHit(i, mod, cyc));
	}

      }
      
    }



    // wsummary -> Write();
    wtree    -> Fill();
  }//Event Loop


  //######## Write and Close ####################
  wtree  -> Write();
  wfile  -> Write();
  wfile  -> Close();
  rfile  -> Close();
}
 

