//##### Standard C++ lib. ######
#include<iostream>
#include<sstream>
#include<fstream>
using namespace std;
#include <iomanip.h>
#include <sys/stat.h>
#include <math.h>
//##### Root Library ###########
#include <TROOT.h>
#include <TStyle.h>
#include <TH1F.h>
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
#include "IngridSimVertexSummary.h"
#include "IngridBasicReconSummary.h"

const int   ntest   =10;
const int   nbinang =12;
const float angbin  = 8;
 
int main(int argc,char *argv[]){
  TROOT        root ("GUI","GUI");
  TApplication app  ("App",0,0);

  Int_t c=-1;  char FileName[300];
  char Output[300];

  while ((c = getopt(argc, argv, "f:o:")) != -1) {
    switch(c){
    case 'f':
      sprintf(FileName,"%s",optarg);
      break;
    case 'o':
      sprintf(Output,"%s",optarg);
      break;
    }
  }
  FileStat_t fs;
  cout<<"reading "<<FileName<<"....."<<endl;
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
  cout << "Total # of events = " << nevt <<endl;


  TFile*            wfile = new TFile(Output,"recreate");
  TTree*            wtree = new TTree("tree","tree");
  bool              retrk;
  float             angle;
  int               niron;
  float             weight;
  float             norm;
  float             totcrsne;
  wtree -> Branch("retrk", &retrk, "retrk/O");
  wtree -> Branch("angle", &angle, "angle/F");
  wtree -> Branch("niron", &niron, "niron/I");
  wtree -> Branch("weight",&weight,"weight/F");
  wtree -> Branch("norm",  &norm,  "norm/F");
  wtree -> Branch("totcrsne",&totcrsne,"totcrsne/F");

  IngridBasicReconSummary*  basicrecon;

  for(int ievt = 0; ievt < nevt; ++ievt){
  //for(int ievt = 0; ievt < 10000; ++ievt){
    //if( ievt%1000 == 0 )cout<<ievt<<endl;
      int i2print = int(nevt/100.);
      if( ievt%i2print == 0 )cout<<"Processing "<<int(ievt*100/float(nevt))<<"% of events"<<endl;
    evt  -> Clear();
    tree -> GetEntry(ievt);

    weight=1; norm=1; totcrsne=1;
    if(evt->NIngridSimVertexes()>0){
      IngridSimVertexSummary* simver = (IngridSimVertexSummary*)evt->GetSimVertex(0);
      weight   = simver->weight;
      norm     = simver->norm;
      totcrsne = simver->totcrsne;
    }

    for( int ibrec=0; ibrec < evt -> NIngridBasicRecons(); ibrec++ ){
      basicrecon = (IngridBasicReconSummary*) ( evt -> GetBasicRecon( ibrec ) );


      if( basicrecon -> retracktest == 0 )
	continue;

      angle = basicrecon -> angle;
    
      for(int itest=0; itest<ntest; itest++){
	niron = itest + 2;
	int flag_test = ( basicrecon -> retracktest >> (itest*2)   )&0x1;
	if(flag_test==0)
	  continue;

	retrk = false;
	if( (( basicrecon -> retracktest >> (itest*2+1) )&0x1 )==1 )
	  retrk = true;

	wtree -> Fill();



      }//itest
    }//basicrecon
  }//ievt

  wtree -> Write();
  wfile -> Write();
  wfile -> Close();

}
 
