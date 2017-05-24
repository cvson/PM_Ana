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
#include "PMReconSummary.h"


#include "IngBasicRecon.cxx"
#include "IngEvtDisp.cxx"
#include "IngridConstants.h"

int main(int argc,char *argv[]){


  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);

  Int_t c=-1;  char FileName[300];
  char Output[300];
  int run_number;
  int sub_run_number=0;
  bool rename = false;
  while ((c = getopt(argc, argv, "r:s:f:o:")) != -1) {
    switch(c){
    case 'r':
      run_number=atoi(optarg);
      break;
    case 's':
      sub_run_number=atoi(optarg);
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
  if(!rename)
    sprintf(FileName,"%s/ingrid_%08d_%04d_done.root",
	    dst_data,run_number,sub_run_number);
  
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
  if( nevt==0 )
    exit(1);
  if(!rename)
    sprintf(Output,"/home/daq/data/dst/ingrid_%08d_%04d_onlybasic.root",
	    run_number, sub_run_number);


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
    //wsummary -> run = run_number ;
    IngridSimVertexSummary* simver =  (IngridSimVertexSummary*)( evt -> GetSimVertex(0) );
    wsummary -> AddSimVertex(simver);
    //BeamInfoSummary* binfo =  (BeamInfoSummary*)( evt -> GetBeamSummary(0) );
    //wsummary -> AddBeamSummary(binfo);

    for( int mod=0; mod<14; mod++ ){   //### Module Loop
      for( int cyc=4; cyc<12; cyc++ ){  //### Cycle Loop
	for( int view=0; view<2; view++ ){  //### View
	  int ningtrack = evt -> NPMModRecons(mod, cyc, view);
	  for(int i=0; i<ningtrack; i++){
	    PMReconSummary* recon   = (PMReconSummary*) (evt -> GetPMModRecon(i, mod, cyc, view) );
	    wsummary -> AddPMModRecon(recon, mod, cyc, view);
	  }
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
 

