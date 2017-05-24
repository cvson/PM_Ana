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
#include <TRandom3.h>
#include <TMath.h>
//##### INGRID Library #########
#include "INGRIDEVENTSUMMARY.h"
#include "IngridHitSummary.h"
#include "IngridSimHitSummary.h"
//##### INGRID Software ########
#include "INGRID_Dimension.cxx"

INGRID_Dimension* fINGRID_Dimension;
const static int NDUMMY= 5; //### Mean Value of Poisson

int main(int argc,char *argv[]){
  TROOT        root ("GUI","GUI");
  TApplication app  ("App",0,0);
  fINGRID_Dimension = new INGRID_Dimension();
  int run_number;
  int calib_run_number;
  int c=-1;
  int sub_run_number;
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


  TRandom3 r; //### random number sheed
  TF1*     fHtime = new TF1("fHtime","-0.096*x*x+133.3*x-33159", 320, 750);
  for(int ievt = 0; ievt < nevt; ++ievt){
    //for(int ievt = 0; ievt <1000; ++ievt){
  
    if(ievt%1000==0)cout<<"Run#"<<run_number<<"\tadd noise event:"<<ievt<<endl;
    summary -> Clear();
    wsummary-> Clear();
    tree    -> GetEntry(ievt);
    for(int mod=0; mod<16; mod++){
      for(int cyc=0; cyc<23; cyc++){
	
	int ninghit = summary -> NIngridModHits(mod, cyc);
        for(int i=0; i<ninghit; i++){
          IngridHitSummary *inghitsum;
          inghitsum   = (IngridHitSummary*) (summary -> GetIngridModHit(i, mod, cyc) );
	  inghitsum  -> dummy = false;
	}
	
	//#### generate dummy noise hit ####
	//##################################
	int ndummyhit = r.Poisson( NDUMMY );
	for(int idummyhit = 0; idummyhit < ndummyhit; idummyhit++){
	  IngridHitSummary *inghitsum = new IngridHitSummary();
	  int tpe   = r.Exp(-1./TMath::Log(0.11)) + 3 ;
	  int ttime = fHtime->GetRandom();

	  inghitsum -> pe    = tpe;                    //### p.e.
	  inghitsum -> time  = ttime + 580 * cyc;      //### time

	  int view, pln, ch;                           //### channel ID
	  int tch   = r.Uniform(0, 616-(1e-11) );
	  if(tch < 528){
	    pln  =   tch / (48);
	    view = ( tch / (24) )%2;
	    ch   = ( tch / (22) )%24;
	  }
	  else{
	    tch = tch - 528;
	    if( tch < 44 ){
	      pln  =  tch/(22) + 11;
	      view = 1;
	      ch   = (tch)%22;

	    }
	    else{
	      tch  = tch -44;
	      pln  = tch/(22) + 13;
	      view = 0;
	      ch   = (tch)%22;
	    }
	  }
	  inghitsum -> pln   = pln;
	  inghitsum -> view  = view;
	  inghitsum -> ch    = ch;
	  inghitsum -> mod   = mod;
	  inghitsum -> cyc   = cyc;
	  double xy, z;
	  fINGRID_Dimension -> get_posXY( mod, view, pln, ch,
					  &xy, &z);
	  inghitsum -> xy    = xy;
	  inghitsum ->  z    =  z;

	  inghitsum -> dummy = true;                   //### dummy flag

	  //wsummary -> AddIngridModHit(inghitsum,mod, cyc);
	  summary   -> AddIngridModHit(inghitsum,mod, cyc);
	}
      }
    }
    wsummary = summary;
    wtree -> Fill();
  }


  wtree -> Write();
  wfile -> Write();
  wfile -> Close();
  //app.Run();
}
 
