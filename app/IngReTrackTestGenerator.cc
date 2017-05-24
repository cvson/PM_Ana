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
#include "IngridTrackSummary.h"
#include "IngridConstants.h"

const static int   TestEreaStartPln =  1;
const static int   TestEreaEndPln   = 10;
const static float TestEreaStart    = 1.0 * TestEreaStartPln * ( IronThick + PlnThick ) + 3;
const static float TestEreaEnd      = 1.0 * TestEreaEndPln   * ( IronThick + PlnThick ) - 3;

int main(int argc,char *argv[]){

  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);

  Int_t c=-1;  char FileName[300];
  int run_number;
  int sub_run_number=0;
  while ((c = getopt(argc, argv, "r:s:f:")) != -1) {
    switch(c){
    case 'r':
      run_number=atoi(optarg);
      sprintf(FileName,"%s/ingrid_%08d_%04d_track.root",
	      dst_data,run_number,sub_run_number);
      break;
    case 's':
      sub_run_number=atoi(optarg);
      sprintf(FileName,"%s/ingrid_%08d_%04d_track.root",
	      dst_data,run_number,sub_run_number);
      break;
    case 'f':
      sprintf(FileName,"%s",optarg);
      run_number=77;
      break;

    }
  }
  
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

  //#### make rootfile after analysis #####
  sprintf(FileName,"%s/ingrid_%08d_%04d_retrack.root",
	  dst_data,
	  run_number,
	  sub_run_number);
  TFile*            wfile = new TFile(FileName,"recreate");
  TTree*            wtree = new TTree("tree","tree");
  IngridEventSummary* wsummary = new IngridEventSummary(); 
  wtree              -> Branch   ("fDefaultReco.","IngridEventSummary", 
				 &wsummary,  64000,   99);


  IngridHitSummary*         inghitsum;
  IngridBasicReconSummary*  ingbasrec;
  BeamInfoSummary*               info;
 

  TRef  fIngridHit[INGRIDHIT_MAXHITS];
  //for( int ievt = 0; ievt < nevt; ievt++  ){
  for( int ievt = 0; ievt < 1000; ievt++  ){

    if(ievt%100==0)cout << "analyze event# " << ievt<<endl;
    evt      -> Clear();
    wsummary -> Clear();
    tree     -> GetEntry(ievt);

    int nbrec = evt -> NIngridBasicRecons();
    for( int ibrec=0; ibrec < nbrec; ibrec++ ){
      ingbasrec   = (IngridBasicReconSummary*)( evt -> GetBasicRecon(ibrec) );

      if( ingbasrec -> hastrk ){
	IngridTrackSummary* trkx = ingbasrec -> GetTrack(0); 
	IngridTrackSummary* trky = ingbasrec -> GetTrack(1); 
	//###########################################################################
	//### Select the event for re-tracking test #################################
	//###########################################################################
	//###########################################################################
	bool flag =
	  ( trkx -> vtxf[2] > TestEreaEnd    )  &&  //### last Z point
	  ( trky -> vtxf[2] > TestEreaEnd    )  &&  //### last Z point
	  ( trkx -> vtxi[2] < TestEreaStart  )  &&  //### first Z point
	  ( trky -> vtxi[2] < TestEreaStart  );     //### first Z point

	if(flag){
	  int nrefhit = ingbasrec -> nhits;

	  for(int k=0; k<nrefhit; k++){
	    IngridHitSummary* b = ingbasrec -> GetIngridHit(k);
	    wsummary -> AddIngridModHit( b, b->mod, b->cyc );

	  }

	  /*
	    for(int mod=0; mod<14; mod++){
	    for(int cyc=0; cyc<23; cyc++){
	    int                   nhit = evt -> NIngridModHits(mod, cyc);
	    for(int i=0; i<nhit; i++){
	    inghitsum = (IngridHitSummary*)( evt -> GetIngridModHit(i, mod, cyc) );
	    wsummary         -> AddIngridHit(inghitsum);
	    t                -> AddIngridHit(inghitsum);
	    }
	    }//cyc
	    }//mod
	  */
	}
      }//hastrack
    }//ibrec
    wtree -> Fill();
  }//ievt
  wtree -> Write();
  wfile -> Write();
  wfile -> Close();

}//main
 

