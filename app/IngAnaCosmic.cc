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

#include "IngEvtDisp.cxx"
IngEvtDisp* fingevtdisp;

#include "IngAnaCosmic.cxx"
IngAnaCosmic* fanacosmic;

const static float thr = 6.5;

int main(int argc,char *argv[]){
  char buff1[200],buff2[200];
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  fingevtdisp = new IngEvtDisp();
  TCanvas* evtdisp = new TCanvas("c1","c1",10,10,600,800);
  fanacosmic = new IngAnaCosmic();
  Int_t run_number, sub_run_number;
  Int_t c=-1;  char FileName[300];
  bool anarockmu = false;
  while ((c = getopt(argc, argv, "r:f:b:s:m")) != -1) {
    switch(c){
    case 'r':
      run_number=atoi(optarg);
      break;

    case 's':
      sub_run_number=atoi(optarg);
      break;
    case 'm':
      anarockmu = true;
      break;

    case 'f':
      sprintf(buff1,"%s",optarg);
      run_number=77;
      break;

    }
  }
  FileStat_t fs;
  ifstream timing;
  if( !anarockmu ){
    sprintf(buff1,"/home/daq/data/cosmic/ingrid_%08d_%04d_tclster.root",
	    run_number, sub_run_number);
  }
  else if( anarockmu ){
    sprintf(buff1,"/home/daq/data/rock_muon/ingrid_%08d_%04d_rmu.root",
	    run_number, sub_run_number);
  }
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


  IngridHitSummary*               hit;
  IngridBasicReconSummary* basicrecon;
  TRef  fIngridHit[INGRIDHIT_MAXHITS];

  int ncosmic[11]={0};
  int nhit[11]={0};

  for(int ievt=0; ievt<nevt; ievt++){
  //for(int ievt=0; ievt<1000; ievt++){


    if(ievt%100==0)cout << "analyze event# " << ievt<<endl;
    evt      -> Clear();
    tree     -> GetEntry(ievt);

    for(int ibasic=0; ibasic < evt -> NIngridBasicRecons(); ibasic++){
      basicrecon = (IngridBasicReconSummary*)evt -> GetBasicRecon(ibasic);
      int hitmod = basicrecon -> hitmod;


      if(basicrecon->Nhits()>5){ 
	for( int ihit=0; ihit<INGRIDHIT_MAXHITS; ihit++ ){
	  fIngridHit[ihit] = 0;
	  fIngridHit[ihit] = TRef( (IngridHitSummary*)basicrecon->GetIngridHit(ihit) );
	}
	fanacosmic  -> SetHit(fIngridHit);

	for(int pln=1; pln<10; pln++){ //### analysis plane by plane
	  vector<int> trigger1 = fanacosmic -> fTrigger(0,pln-1,thr);
	  vector<int> trigger2 = fanacosmic -> fTrigger(0,pln+1,thr);
	  int ntrigger1        = trigger1.size();
	  int ntrigger2        = trigger2.size();
	  if( ntrigger1==0 || ntrigger2 == 0 )continue;

	  ncosmic[pln]++;
	  vector<int> hitch    = fanacosmic -> fTdcHit(0, pln);
	  int nhitch = hitch.size();
	  bool ok = false;
	  for(int j=0; j<nhitch; j++){
	    for(int k=0; k<ntrigger1; k++){
	      for(int l=0; l<ntrigger2; l++){
		if( ( trigger1[k] <= hitch[j] && hitch[j] <= trigger2[l] )||
		    ( trigger2[l] <= hitch[j] && hitch[j] <= trigger1[k] )
		    )
		  ok = true;
	      }
	    }
	  }
	  if(ok)
	    nhit[pln]++;

	  else{

	    cout<<"pln:" << pln << " is ineff." << endl;
	    cout << basicrecon -> clstime << endl;

	    int nlhits = evt -> NIngridModHits(hitmod, 14);
	    cout << "-- view 0" << endl;
	    for(int ih = 0; ih<nlhits; ih++){
	      hit = (IngridHitSummary*)evt->GetIngridModHit(ih, hitmod, 14);
	      if( hit->view == 0 )
		cout <<" pln:" << hit -> pln 
		     <<" ch :" << hit -> ch 
		     <<" pe :" << hit -> pe 
		     <<" tdc :"<< hit -> tdc
		     <<" tdc :"<< hit -> time
		     <<endl;
	    }
	    cout << "-- view 1" << endl;
	    for(int ih = 0; ih<nlhits; ih++){
	      hit = (IngridHitSummary*)evt->GetIngridModHit(ih, hitmod, 14);
	      if( hit->view == 1 )
		cout <<" pln:" << hit -> pln 
		     <<" ch :" << hit -> ch 
		     <<" pe :" << hit -> pe 
		     <<" tdc :"<< hit -> tdc
		     <<" tdc :"<< hit -> time
		     <<endl;
	    }

	    fingevtdisp -> Draw_Module ( *evtdisp );
	    fingevtdisp -> Draw_Hit_A  ( *basicrecon,0.1 );
	    evtdisp->Update();
	    cin.get();
	  }

	}

      }
    }


  }//Event Loop

  for(int i=0; i<11; i++){
    cout << ncosmic[i] << "\t"
	 << nhit[i]    << "\t"
	 << 1.0 * nhit[i] / ncosmic[i] << "\t"
	 << endl;
  }
}
 
