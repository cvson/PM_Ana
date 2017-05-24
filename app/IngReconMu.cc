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
#include "setup.hxx"
//#include "INGRID_Dimension.hxx"
//#include "root_setup.hxx"
#include "INGRIDEVENTSUMMARY.h"
#include "IngridHitSummary.h"
#include "IngridSimHitSummary.h"
#include "IngridSimVertexSummary.h"
#include "IngridSimParticleSummary.h"
#include "BeamInfoSummary.h"
#include "IngridBasicReconSummary.h"
#include "IngridTrackSummary.h"

//#include "ReconTrack.cxx"
#include "ReconTrack.cxx"
#include "IngEvtDisp.cxx"

ReconTrack* frecontrack;
IngEvtDisp* fingevtdisp;

int main(int argc,char *argv[]){
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  frecontrack = new ReconTrack();
  fingevtdisp = new IngEvtDisp();

  char buff1[300];
  int run_number;
  int n;
  Int_t c=-1;  char FileName[300];
  while ((c = getopt(argc, argv, "r:f:b:")) != -1) {
    switch(c){
    case 'r':
      run_number=atoi(optarg);
      sprintf(buff1,"/home/daq/data/dst/ingrid_%08d_0000_1streduc.root",run_number);
      break;

    case 'f':
      sprintf(buff1,"%s",optarg);
      run_number=77;
      break;

    }
  }
  FileStat_t fs;
  ifstream timing;




  //######## read root file #############################
  //#####################################################


  cout<<"reading "<<buff1<<"....."<<endl;
  if(gSystem->GetPathInfo(buff1,fs)){
    cout<<"Cannot open file "<<buff1<<endl;
    exit(1);
  }
  IngridEventSummary* evt = new IngridEventSummary();
  TFile*            rfile = new TFile(buff1,"read");
  TTree*             tree = (TTree*)rfile -> Get("tree");
  TBranch*          EvtBr = tree->GetBranch("fDefaultReco.");
  EvtBr                   ->SetAddress(&evt);
  tree                    ->SetBranchAddress("fDefaultReco.", &evt);
  int                nevt = (int)tree -> GetEntries();
  cout << "Total # of events = " << nevt <<endl;

  //#### make rootfile after analysis #####
  sprintf(buff1,"/home/daq/data/dst/ingrid_%08d_0000_recon.root",run_number);
  TFile*            wfile = new TFile(buff1,"recreate");
  TTree*            wtree = new TTree("tree","tree");
  IngridEventSummary* wsummary = new IngridEventSummary(); 
  wtree              -> Branch   ("fDefaultReco.","IngridEventSummary", 
				 &wsummary,  64000,   99);


  IngridHitSummary*          inghitsum;
  IngridHitSummary*        inghitsum_t; 
  IngridBasicReconSummary*  basicrecon;
  TCanvas* evtdisp = new TCanvas("c1","c1",10,10,600,800);
  TCanvas* TChough = new TCanvas("c2","c2",10,600,600,800);

  TRef  fIngridHit[INGRIDHIT_MAXHITS];
  //fingevtdisp -> Draw_Module(*evtdisp);
  for(int ievt=0; ievt<nevt; ievt++){
    cout<<"event:"<<ievt<<endl;
     //if(ievt%100==0)cout<<ievt<<endl;
    evt      -> Clear();
    tree     -> GetEntry(ievt);
    IngridSimVertexSummary* simver = (IngridSimVertexSummary*)evt->GetSimVertex(0);
    int inttype = simver->inttype;

 
    int nbrecon  = evt -> NIngridBasicRecons();
    for(int i=0; i<nbrecon; i++){
      basicrecon = (IngridBasicReconSummary*)evt->GetBasicRecon(i);
      int   nactpln   = basicrecon -> nactpln;
      float layerpe   = basicrecon -> layerpe;
      int   actinarow = basicrecon -> actinarow;
      if( nactpln    >   1 && 
	  layerpe    > 6.5 &&
	  actinarow  >   3
	  ){  //Basic Track Selection
	evtdisp -> Clear();
	fingevtdisp -> Draw_Module ( *evtdisp );
	fingevtdisp -> Draw_Hit_A  ( *basicrecon,0.1 );
	frecontrack -> Reset();
	frecontrack -> HoughTrans( *basicrecon );

	fingevtdisp->reset_track();
	for(int i=0;i<frecontrack -> Ntrack_x();i++){
	  TF1* fx = (TF1*) frecontrack -> Track_x(i);
	  fingevtdisp -> Draw_Line( *fx, 0 ,0,120);
	}
	for(int i=0;i<frecontrack -> Ntrack_y();i++){
	  TF1* fy = (TF1*) frecontrack -> Track_y(0);
	  fingevtdisp -> Draw_Line( *fy, 1 ,0,120);
	}
	TChough -> Clear();
	frecontrack -> Draw_Hough(*TChough);

	evtdisp     -> Update();
	TChough     -> Update();
	cin.get();

      }//like
    }

  }//Event Loop


  //######## Write and Close ####################


}
 
