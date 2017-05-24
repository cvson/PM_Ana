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

#include "INGRIDEVENTSUMMARY.h"
#include "IngridHitSummary.h"
#include "IngridSimHitSummary.h"
#include "IngridSimVertexSummary.h"
#include "IngridSimParticleSummary.h"
#include "BeamInfoSummary.h"
#include "IngridBasicReconSummary.h"
#include "IngridTrackSummary.h"

#include "IngEvtDisp.cxx"


IngEvtDisp*               fingevtdisp;
IngridBasicReconSummary*  basicrecon;
int ievt;
int tmod;
int tevt;
int tcyc;
bool Display(IngridBasicReconSummary* brec){
  /* 
  if( brec -> hastrk &&
      brec -> matchtrk &&
      !brec -> vetowtracking &&
      !brec -> edgewtracking 
      )
    return true;
 */
  if(ievt==tevt&&brec->hitmod==tmod&&brec->hitcyc==tcyc)
    return true;
 
  return false;
}

int main(int argc,char *argv[]){
  int itry=0, iok=0;
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  fingevtdisp = new IngEvtDisp();

  char buff1[300], Output[300];
  int run_number;
  int sub_run_number=0;
  int n;
  Int_t c=-1;  char FileName[300];


  while ((c = getopt(argc, argv, "r:s:f:i:m:c:")) != -1) {
    switch(c){
    case 'r':
      run_number=atoi(optarg);
      sprintf(FileName,"%s/ingrid_%08d_%04d_track.root",
	      dst_data,
	      run_number,
	      sub_run_number);
      break;
    case 'c':
      tcyc=atoi(optarg);
      break;
    case 's':
      sub_run_number=atoi(optarg);
      sprintf(FileName,"%s/ingrid_%08d_%04d_track.root",
	      dst_data,
	      run_number,
	      sub_run_number);
      break;
    case 'f':
      sprintf(FileName,"%s",optarg);
      run_number=77;
      break;
    case 'i':
      tevt=atoi(optarg);
      break;
    case 'm':
      tmod=atoi(optarg);
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
  tree                    ->SetBranchAddress("fDefaultReco.", &evt);  int                nevt = (int)tree -> GetEntries();
  cout << "Total # of events = " << nevt <<endl;


  BeamInfoSummary*            beaminfo;


  TCanvas* evtdisp = new TCanvas("c1","c1",10,10,700*1.25,500*1.25);

  TRef  fIngridHit[INGRIDHIT_MAXHITS];
  int ntrkok    = 0;
  int nretrkok0 = 0;
  int nretrkok1 = 0;

  int highnuE   = 0;

    
  for(ievt=tevt; ievt<nevt; ievt++){
    if(ievt%1000==0)cout<<"-----EVENT # "<<ievt<<endl;
   
    cout<<"-----EVENT # "<<ievt<<endl;
    evt                -> Clear();
    tree               -> GetEntry(ievt);

    int nbasicrecons=evt->NIngridBasicRecons();
    cout << nbasicrecons << endl;
    if( evt->NIngridBasicRecons() > 0 ){
    //if(!( evt->NIngridBasicRecons() > 0) ){
     
    //if(1){

    for(int ibas=0; ibas<evt->NIngridBasicRecons(); ibas++){
    
      basicrecon = (IngridBasicReconSummary*)evt->GetBasicRecon( ibas );
  
      //if( Display(basicrecon)  ){
      if(1){
	cout << "display" << endl;
	IngridTrackSummary* trk0 = (IngridTrackSummary*) basicrecon->GetTrack(0);
	IngridTrackSummary* trk1 = (IngridTrackSummary*) basicrecon->GetTrack(1);
	evtdisp     -> Clear();
	fingevtdisp -> Draw_Module    ( *evtdisp );
	//fingevtdisp -> Draw_Hit_A     ( *basicrecon,0.06 );
	fingevtdisp -> Draw_Hit_A     ( *basicrecon,0.25 );
	fingevtdisp -> Print_Hit_A    ( *basicrecon );
	//### other hit 1
	IngridBasicReconSummary* brec1 = new IngridBasicReconSummary();
	int hitmod = basicrecon -> hitmod;
	int hitcyc = basicrecon -> hitcyc;
	cout << "------- all hit --------" << endl;
	for(int ihit = 0; ihit < evt->NIngridModHits( hitmod, hitcyc ); ihit++){
	  IngridHitSummary* inghit = (IngridHitSummary*)(evt->GetIngridModHit(ihit, hitmod, hitcyc));

	  //if(inghit->cyc==-2)
	    brec1 -> AddIngridHit(inghit);
	}
	//fingevtdisp -> Draw_Hit_A     ( *brec1,0.02, 6, 0 );
	
	/*
	IngridSimParticleSummary* simpar;

	for(int ipar=0; ipar < evt->NIngridSimParticles(); ipar++){
	  simpar= (IngridSimParticleSummary*)(evt->GetSimParticle(ipar));
	  if(simpar->pdg==13)
	    break;
	}
	*/

	beaminfo = (BeamInfoSummary*)evt->GetBeamSummary(0);
	//fingevtdisp->Draw_BeamInfo(*beaminfo);

	cout << "--- generated particle ------------------" <<endl;
	/*
	for(int ipar=0; ipar < evt->NIngridSimParticles(); ipar++){
	  simpar= (IngridSimParticleSummary*)(evt->GetSimParticle(ipar));
	  
	  cout << simpar->pdg << "\t"
	       << simpar->ipos[2] << "\t"
	       << simpar->fpos[2] <<endl;

	}
	*/

	//fingevtdisp -> Print_Hit_A    ( *brec1 );
	cout << "------------------------" << endl;

	IngridBasicReconSummary* breca = new IngridBasicReconSummary();
	for(int ihit = 0; ihit < evt->NIngridHits(  ); ihit++){
	  IngridHitSummary* inghit = (IngridHitSummary*)(evt->GetIngridHit(ihit));
	  //IngridSimHitSummary* ingsimhit = (IngridSimHitSummary*)(evt->GetIngridSimHit(ihit));

	  if(inghit->mod==hitmod){
	    cout << "view: " << inghit->view   << "\t"
		 << "pln : " << inghit->pln    << "\t"
		 << "ch  : " << inghit->ch     << "\t"
		 << "pe  : " << inghit->pe     << "\t"
		 << "time: " << inghit->time   << "\t"
		 << "dumm: " << inghit->dummy  << "\t"
		 << "cyc : " << inghit->cyc    << "\t";
	    //if(ingsimhit!=0)
	    //cout	      << "pdg : " << ingsimhit->pdg << "\t"
	    //<< endl;
	    //else
	    //cout << endl;
	  }

	  if(inghit->mod == basicrecon->hitmod)
	    breca -> AddIngridHit(inghit);
	}
	//fingevtdisp -> Draw_Hit_A     ( *brec1,0.02, 6, 0 );
	//fingevtdisp -> Draw_Hit_A     ( *breca     ,0.06, 6, 0 );
	fingevtdisp -> Draw_Hit_A     ( *basicrecon,0.06 );


	if(basicrecon->hastrk){
	  TF1* ff0    =  new TF1("ff0", "pol1",0, 130);
	  ff0         -> SetParameter(0, trk0->etx);
	  ff0         -> SetParameter(1, trk0->tx);
	  fingevtdisp -> Draw_Line(*ff0, 0, trk0->vtxi[2], trk0->vtxf[2],1,3);
	  TF1* ff1    =  new TF1("ff1", "pol1",0, 130);
	  ff1         -> SetParameter(0, trk1->etx);
	  ff1         -> SetParameter(1, trk1->tx);
	  fingevtdisp -> Draw_Line(*ff1, 1, trk1->vtxi[2], trk1->vtxf[2],1,3);
	}
	/*
	if(evt->NIngridSimVertexes()>0){
	IngridSimVertexSummary* simver = (IngridSimVertexSummary*)(evt->GetSimVertex(0));
	cout << "true vertex   z      : " << (simver->z) << endl;
	cout << "true vertex   z[pln#]: " << (int)((simver->z+52)/10.7) << endl;
	//cout << "true vertex   x:" << (simver->x) << endl;
	//cout << "true vertex   y:" << (simver->y) << endl;
	cout << "true vertex   x      : " << (simver->x)-(simver->mod-3)*150+60 << endl;
	cout << "true vertex   y      : " << (simver->y)+60 << endl;
	cout << "recon vertex  z      : " << basicrecon->vertexxz << "\t"
	     << endl;

	cout  << "mu energy : "  << simpar->momentum[3]+0.106 << endl
	      << "theta x   : "  << TMath::ATan(simpar->dir[0]/simpar->dir[2])*180/3.14 << endl
	      << "theta y   : "  << TMath::ATan(simpar->dir[1]/simpar->dir[2])*180/3.14 << endl

	      << endl;


	cout << "inttype     :" << simver->inttype << endl;


	//fingevtdisp -> Draw_Muon    ( *evt );
	fingevtdisp -> Print_Muon    ( *evt );
	}
	*/
	evtdisp     -> Update();
	evtdisp     -> Print("evtdisp.png");
	cin.get();
      }

    }//BasicRecon
    }//NIngridBasicRecon
    else{
      IngridBasicReconSummary* brec  = new IngridBasicReconSummary();
      IngridBasicReconSummary* brec1 = new IngridBasicReconSummary();
      for(int ihit = 0; ihit < evt->NIngridModHits( tmod, 4 ); ihit++){
	IngridHitSummary* inghit = (IngridHitSummary*)(evt->GetIngridModHit(ihit, tmod, 4));
	if(inghit->cyc==-2)
	  brec1 -> AddIngridHit(inghit);
	else
	  brec  -> AddIngridHit(inghit);
      }
      evtdisp     -> Clear();
      fingevtdisp -> Draw_Module    ( *evtdisp );
      fingevtdisp -> Draw_Hit_A     ( *brec,0.02 );
      fingevtdisp -> Draw_Hit_A     ( *brec1,0.02, 6, 0 );
      evtdisp     -> Update();
      cin.get();

    }
  }//ievt

}
 
