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
//#include "analysis_beam.hxx"
#include "IngTClster.hxx"

#include "INGRIDEVENTSUMMARY.h"
#include "IngridHitSummary.h"
#include "IngridBasicReconSummary.h"

#include "IngridConstants.h"

int main(int argc,char *argv[]){
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  Int_t run_number;
  Int_t sub_run_number=0;
  Int_t c=-1;  char FileName[300];
  char Output[300];

  while ((c = getopt(argc, argv, "f:o:")) != -1) {
    switch(c){
    case 'o':
      sprintf(Output,"%s",optarg);
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
    cout<<"Cannot open file "<< FileName <<endl;
    exit(1);
  }
  cout << FileName << endl;
  IngridEventSummary* evt = new IngridEventSummary();
  TFile*            rfile = new TFile(FileName,
				      "read");
  TTree*             tree = (TTree*)rfile -> Get("tree");
  TBranch*          EvtBr = tree->GetBranch("fDefaultReco.");
  EvtBr                   ->SetAddress(&evt);
  tree                    ->SetBranchAddress("fDefaultReco.", &evt);
  int                nevt = (int)tree -> GetEntries();
  cout << "Total # of events = " << nevt <<endl;

  //#### make rootfile after analysis #####
  //#######################################
  //sprintf(buff1,"/home/daq/data/dst/ingrid_%08d_0000_tclster.root",run_number);
  
  TFile*            wfile = new TFile(Output,
				      "recreate");
  TTree*            wtree = new TTree("tree","tree");
  wtree -> SetMaxTreeSize(5000000000);
  vector<int>  mod, view, pln, ch, cosmic, hit;
  vector<double> pe;
  wtree -> Branch("mod",    &mod);
  wtree -> Branch("view",   &view);
  wtree -> Branch("pln",    &pln);
  wtree -> Branch("ch",     &ch);
  wtree -> Branch("pe",     &pe);
  wtree -> Branch("cosmic", &cosmic);
  wtree -> Branch("hit",    &hit);

  IngridHitSummary* inghitsum;
  for(int ievt=0; ievt<nevt; ievt++){
    if(ievt%1000==0)cout << "analyze event# " << ievt<<endl;
    evt      -> Clear();
    tree     -> GetEntry(ievt);

    mod.    clear();
    view.   clear();
    pln.    clear();
    ch.     clear();
    pe.     clear();
    cosmic. clear();
    hit.    clear();


    for( int imod=0; imod<14; imod++ ){   //### Module Loop
      //for( int cyc=14; cyc<16; cyc++ ){  //### Cycle Loop
      for( int cyc=0; cyc<23; cyc++ ){  //### Cycle Loop
	for(int i=0; i< evt -> NIngridModHits(imod, cyc); i++){//### Hit Loop
	  inghitsum   = (IngridHitSummary*) (evt -> GetIngridModHit(i, imod, cyc) );
	  if( inghitsum->gocosmic ){

	    int  iview= inghitsum->view;
	    int  ipln = inghitsum->pln;
	    int  ich  = inghitsum->ch;
	    double ipe= inghitsum->pecorr;
	    bool icosmic=true;
	    bool ihit   =false;
	    if( inghitsum->hitcosmic )
	      ihit = true;
	    mod.    push_back(imod);
	    view.   push_back(iview);
	    pln.    push_back(ipln);
	    ch.     push_back(ich);
	    pe.     push_back(ipe);
	    cosmic. push_back(icosmic);
	    hit.    push_back(ihit);
	  }//gocosmic
	}//hit
      }//cyc
    }//mod
    wtree -> Fill();
  }//Event Loop
  //######## Write and Close ####################
  wtree  -> Write();
  wfile  -> Write();
  wfile  -> Close();

}
 
