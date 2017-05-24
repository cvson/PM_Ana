//#######################################################
//#######################################################
//##### Plot the result after basic reconstruction ######
//##### p.e./active layer and # of active planes   ######
//#######################################################
//#######################################################


//##### Standard C++ lib. ######
#include<iostream>
#include<sstream>
#include<fstream>
#include<math.h>
using namespace std;
#include <iomanip.h>
#include <sys/stat.h>
//##### Root Library ###########
#include <TROOT.h>
#include <TStyle.h>
#include <TH1F.h>
#include <TApplication.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TMath.h>
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

int main(int argc,char *argv[]){
  TROOT        root ("GUI","GUI");
  TApplication app  ("App",0,0);

  Int_t c=-1;  char FileName[300]; char Output[300];

  sprintf(Output,"temp.root");
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
  double totalpot=0; 
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

  //#### Create temporary ROOT File and finnaly  ########
  //#### Display it with TBrowser  ######################
  TFile* f          = new TFile(Output,"recreate");
  TTree*      wtree = new TTree("tree","tree");
  float             Emu;
  float             Enu;
  float             thetamu;
  bool              rec;
  int             inttype;
  int              npar;
  bool              fid;

  wtree -> Branch("Enu",    &Enu,     "Enu/F");
  wtree -> Branch("Emu",    &Emu,     "Emu/F");
  wtree -> Branch("thetamu",&thetamu, "theta/F");
  wtree -> Branch("rec",    &rec,     "rec/O");
  wtree -> Branch("fid",    &fid,     "fid/O");
  wtree -> Branch("inttype",&inttype, "inttype/I");
  wtree -> Branch("npar"   ,&npar,    "npar/I");


  IngridBasicReconSummary*   basicrecon;
  IngridSimParticleSummary*  simpar;
  IngridSimVertexSummary*    simver;
  
  for(int ievt = 0; ievt < nevt; ++ievt){
    //for(int ievt = 0; ievt < 10000; ++ievt){
  
    if( ievt%10000 == 0 )cout<<ievt<<endl;
    evt  -> Clear();
    tree -> GetEntry(ievt);
    int intmod;
    simver = (IngridSimVertexSummary*)(evt->GetSimVertex(0));
    if( simver -> inttype > 30 ) //only select CC
      continue;

    intmod = simver->mod;
    Enu = simver->nuE;
    fid = false;
    if(

       (simver->mod<7&&( simver->x+525-150*simver->mod )>25&&( simver->x+525-150*simver->mod )<125&&simver->y>-50&&simver->y<50&&simver->z<33.5)||
       
       (simver->mod>=7&&( simver->y+525-150*(simver->mod-7) )>25&&( simver->y+525-150*(simver->mod-7) )<125&&simver->x>-50&&simver->x<50&&simver->z<-366.5 )
       ){
      fid = true;
    }

    inttype = simver->inttype;
    npar = evt -> NIngridSimParticles();

    for(int ipar=0; ipar < evt->NIngridSimParticles(); ipar++ ){
      simpar = (IngridSimParticleSummary*)(evt->GetSimParticle(ipar));
      if( simpar -> pdg == 13 ) //Get Muon
	break;
    }
    //#### debug
    /*
    cout << "=============================" << endl;
    cout << "momentum[0]  : " << simpar->momentum[0] << endl;
    cout << "momentum[1]  : " << simpar->momentum[1] << endl;
    cout << "momentum[2]  : " << simpar->momentum[2] << endl;
    cout << "momentum[3]  : " << simpar->momentum[3] << endl;
    cout << " ********    : " << 
      sqrt( pow( simpar->momentum[0], 2 ) + 
	    pow( simpar->momentum[1], 2 ) + 
	    pow( simpar->momentum[2], 2 ) + 
	    pow( 0.106, 2 )  
	    ) - 0.106 << endl;
    */


    Emu      = simpar->momentum[3] + 0.106;
    thetamu  = fabs(TMath::ACos(simpar->dir[2]))/TMath::Pi()*180;
    rec      = false;

    int   hitmod;
    for( int ibas=0; ibas < evt->NIngridBasicRecons(); ibas++ ){
      basicrecon = (IngridBasicReconSummary*) ( evt -> GetBasicRecon( ibas ) );
      if( basicrecon -> hastrk   &&
	  basicrecon -> matchtrk &&
	  !basicrecon -> vetowtracking &&
	  !basicrecon -> edgewtracking 
	  ){
	rec = true;
	hitmod = basicrecon->hitmod;
	break;
      }
    }//ibas


    //##Print
    if( !rec &&fid&& inttype==1 && thetamu < 25 && Emu*cos(thetamu/180*3.14)<0.5 ){
      cout << "evt :" << ievt 
	   << "\tmodule :" << intmod
	   << "\tEmu * cos :" << Emu*cos(thetamu/180*3.14)
	   << "\tangle :" << thetamu
	   << endl;
    }
    /*
    if(rec){
      cout << "recevt :" << ievt 
	   << "\tmodule :" << hitmod
	   << "\tEmu * cos :" << Emu*cos(thetamu/180*3.14)
	   << endl;
	   }
    */
    
    wtree -> Fill();	
  }

  wtree -> Write();
  f     -> Write();
  f     -> Close();

}
 
