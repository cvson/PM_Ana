#include<iostream>
#include<vector>
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
#include <TString.h>
#include <TSystem.h>

#include "TApplication.h"
#include "analysis_cosmic.hxx"
//#include "analysis_co.hxx"
#include "setup.hxx"
//#include "root_setup.hxx"
#include "INGRIDEVENTSUMMARY.h"
#include "IngridHitSummary.h"
#include "IngridSimHitSummary.h"
#include "IngridSimVertexSummary.h"
#include "IngridSimParticleSummary.h"
#include "BeamInfoSummary.h"
#include "IngridBasicReconSummary.h"

Int_t             fMod;
Long_t            fTime;
Int_t            Cycle;
vector<int>*       adc;
vector<int>*      view;
vector<int>*       pln;
vector<int>*        ch;
vector<double>*  posxy;
vector<double>*   posz;
vector<double>*     pe;
vector<int>*      nsec;
 

vector<double>  anape;
vector<int>   anaview;
vector<int>    anapln;
vector<int>     anach;
IngridEventSummary* evt = new IngridEventSummary      ();

int main(int argc,char *argv[]){
  fINGRID_Dimension = new INGRID_Dimension();
  FileStat_t fs;
  vector<Hit> allhit;
  char buff1[200],buff2[200];
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  gStyle->SetOptStat(0);
 
  Int_t run_number;
  Int_t c=-1;  char FileName[300];
  bool sflag=false;
  bool automode=false;
  while ((c = getopt(argc, argv, "b:r:f:a")) != -1) {
    switch(c){
    case 'r':
      run_number=atoi(optarg);
      sprintf(FileName,"/home/daq/data/dst/ingrid_%08d_0000_processed.root",run_number);
      break;
    case 'b':
      run_number=atoi(optarg);
      sprintf(FileName,"/home/daq/data/dst/ingrid_%08d_0000_calib.root",run_number);
      break;

    case 'f':
      sprintf(FileName,"%s",
	      optarg);
      break;
    case 'a':
      automode = true;
      break;
    }
  }

  //#### Read ROOT File with INGRID-j data structure #####
  //######################################################
  cout << "reading "<< FileName
       << "....."   << endl;
  if( gSystem -> GetPathInfo( FileName, fs ) ){
    cout<<"Cannot open file "<<FileName<<endl;
    exit(1);
  }
  evt = new IngridEventSummary      ();
  TFile*            rfile = new TFile                   (FileName,"read");
  TTree*             tree = (TTree*)rfile -> Get        ("tree");
  TBranch*          EvtBr = tree          -> GetBranch  ( "fDefaultReco.");
  EvtBr                   -> SetAddress      ( &evt );
  tree                    -> SetBranchAddress( "fDefaultReco." , &evt );
  int                nevt = (int)tree -> GetEntries();
  cout << "Total # of events = " << nevt <<endl;


  //#### Event Loop  ####
  //#####################
  Int_t n=1;
  int mod = 0;
  int cyc = 0;
  while(n>=0){
    //### Input event#, Module#, cycle#  ###
    //######################################
    evt      -> Clear();
    if( !automode ){

    cout << "Entry? " ; 
    cin  >> n;
    cout << "Module? ";
    cin  >> mod;
    cout << "Cyc? "   ;
    cin  >> cyc;
    }
    if( automode ){
      n++;
      mod++;if(mod>13)mod=0;
      cyc++;if(cyc>23)cyc=0;
    }
    tree -> GetEntry(n);

    BeamInfoSummary*  bsd = (BeamInfoSummary*) ( evt -> GetBeamSummary(0) ); //### usually there is only one 
                                                                             //### beam summary in each event
    //##### put the IngridModHit class to simple Hit class for analysis ######
    //########################################################################
    //IngridHitSummary* inghitsum;
    allhit.clear();
    int ninghit = evt -> NIngridModHits(mod, cyc);
    for(int i=0; i<ninghit; i++){
      IngridHitSummary* inghitsum   = (IngridHitSummary*) (evt -> GetIngridModHit(i, mod, cyc) );
      Hit hit  ;
      hit.id    = i;
      hit.pe    = inghitsum -> pe;
      hit.time  = inghitsum -> time;
      hit.view  = inghitsum -> view;
      hit.pln   = inghitsum -> pln;
      hit.ch    = inghitsum -> ch;
      hit.posxy = inghitsum -> xy;
      hit.posz  = inghitsum -> z;
      allhit.push_back(hit);
    }
    Int_t nSpill = evt->nd280nspill;

    //#### Make Time clustring #####
    //##############################
    fSortTime(allhit);
    vector<Hit> hitclster;
    while(fFindTimeClster(allhit, hitclster, fTime)){
    //hitclster = allhit;
       fSortTime(hitclster);
       Int_t nCls = hitclster.size();
      //######### check active plane ################
      //#############################################
      Int_t   trgbit  = 0;
      Int_t nActPln = 0;
      for( Int_t i = 0 ; i < nCls ; i++){
	for( Int_t j=i+1 ; j<nCls ; j++){
	  if( hitclster[i].pln == hitclster[j].pln){
	    if(hitclster[i].view != hitclster[j].view){
	      trgbit=trgbit|((0x400)>>(10-hitclster[i].pln));
	    }//view difference
	  }//pln coince
	}//j
      }//i
      for(Int_t i=1;i;i=i<<1){
	if(trgbit&i){
	  nActPln++;
	}
      }
      //######### calculate mean p.e. ###############
      //#############################################
      Float_t fMeanPe          = 0;
      Int_t      nEnt  = 0;
      for(Int_t i = 0 ; i < nCls ; i++){
	if( trgbit & ( ( 0x400 ) >> ( 10 - hitclster[i].pln ) )  ){
	  fMeanPe += hitclster[i].pe;
	  nEnt++;
	}
      }
      if( nEnt > 0 )
	fMeanPe = 1.0 * fMeanPe/(nActPln * 2);
      else if( nEnt == 0 )
	fMeanPe = 0;

      int inttype = 0;
      float nuE = 0;
      float verx   = -1.e-5;
      float very   = -1.e-5;
      float verz   = -1.e-5;
      //if( evt ->  )
      IngridSimVertexSummary* simvertex = evt -> GetSimVertex( 0 );// usually only 1
      if( simvertex ){
	inttype = simvertex -> inttype;
	nuE     = simvertex -> nuE;
	verx    = simvertex -> x;
	very    = simvertex -> y;
	verz    = simvertex -> z;
      }
      //if( !automode ||
      //  ( fMeanPe>6.5 && nActPln>=2 )
      //  ){
      //#### Draw Event Display #########
      //#################################
	Draw_Module();
	Draw_Info  ( fMod, Cycle, nSpill, fTime, inttype, nuE);  
	Draw_Hit   ( hitclster );
	Print      ( hitclster );
	cout<<"Z:"<<verz+55<<endl;
	//cout<<"X:"<<((int)verx+510)%150<<endl;
	cout<<"X:"<< verx  <<endl;
	cout<<"Y:"<< very  <<endl;

	if(!automode)cin.get();
	if(automode)sleep(5);
    

      cin.get();
      hitclster.clear();
      }
    cout << "found all time cluster of this event" << endl; 
    cout << "-----------------------------------------------" << endl; 


  }//Event Loop End

}
