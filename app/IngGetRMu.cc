
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

long fcTime;

void Subs(IngridBasicReconSummary* bas,
	  IngridBasicReconSummary* nbas){
  nbas -> clstime = bas -> clstime;
  nbas -> nactpln = bas -> nactpln;
  nbas -> layerpe = bas -> layerpe;
  nbas -> hitmod  = bas -> hitmod;
  nbas -> hitcyc  = bas -> hitcyc;
}


int main(int argc,char *argv[]){

  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);

  Int_t c=-1;  char FileName[300];
  char Output[300]; bool rename = false;
  int run_number;
  int sub_run_number=0;
  while ((c = getopt(argc, argv, "r:s:f:o:")) != -1) {
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
  
  sprintf(Output,"/home/daq/data/rock_muon/ingrid_%08d_%04d_rmu.root",
	  run_number, sub_run_number);
 
  //######## new root file #########
  //################################
  TFile*            wfile = new TFile(Output,"recreate");
  TTree*            wtree = new TTree("tree","tree");
  IngridEventSummary* wsummary = new IngridEventSummary(); 
  wtree              -> Branch   ("fDefaultReco.","IngridEventSummary", 
				 &wsummary,  64000,   99);
  wtree -> SetMaxTreeSize(5000000000);

  IngridBasicReconSummary* basicrecon;
  IngridBasicReconSummary* new_basicrecon = new IngridBasicReconSummary();
  for( int ievt = 0; ievt < nevt; ievt++  ){
    if(ievt%100==0)cout << "analyze event# " << ievt<<endl;
    evt      -> Clear();
    wsummary -> Clear();
    tree -> GetEntry(ievt);

    for( int ibrec=0; ibrec < evt -> NIngridBasicRecons(); ibrec++ ){
      basicrecon   = (IngridBasicReconSummary*)( evt -> GetBasicRecon(ibrec) );

      bool rock = ( basicrecon -> vetowtracking ) || ( basicrecon -> edgewtracking );

      if(rock){ //### this is rock muon

	new_basicrecon  -> Clear();
	for(int ihit = 0; ihit < basicrecon -> nhits; ihit++ ){
	  IngridHitSummary* inghit = (IngridHitSummary*)(basicrecon -> GetIngridHit(ihit));
	  wsummary       -> AddIngridModHit( inghit, basicrecon->hitmod, basicrecon->hitcyc );

	  //IngridHitSummary* thit   = (IngridHitSummary*)(wsummary -> GetIngridModHit(ihit, basicrecon->hitmod, basicrecon->hitcyc));
	  //new_basicrecon -> AddIngridHit   ( inghit );
	}

	//Subs( basicrecon, new_basicrecon );
	//wsummary -> AddBasicRecon( new_basicrecon );
	//wsummary -> AddBasicRecon( basicrecon );

      }//### rock or not
    }
    wtree    ->Fill();
  }//Event Loop


  //######## Write and Close ####################
  wtree  -> Write();
  wfile  -> Write();
  wfile  -> Close();

}
 

