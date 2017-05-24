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
#include "IngridSimHitSummary.h"
#include "IngridSimVertexSummary.h"
#include "IngridSimParticleSummary.h"
#include "BeamInfoSummary.h"
#include "IngridBasicReconSummary.h"
FileStat_t fs;

int main(int argc,char *argv[]){

  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  Int_t run_number;
  Int_t sub_run_number=0;
  Int_t c=-1;  char FileName[300], buff1[300];
  char Output[300]; bool rename = false;
  Int_t Scyc =  0;
  Int_t Ncyc = 23;
  bool  cosmic = false;
  while ((c = getopt(argc, argv, "r:s:f:b:co:")) != -1) {
    switch(c){
    case 'r':
      run_number=atoi(optarg);
      break;
    case 'o':
      sprintf(Output,"%s",optarg);
      rename  = true;
      break;
    case 's':
      sub_run_number=atoi(optarg);

      break;

    case 'f':
      sprintf(buff1,"%s",optarg);
      run_number=77;
      break;
    case 'c':
      Scyc = 14;
      Ncyc = 16;
      cosmic = true;
      break;

    }
  }

  //######## read root file #############################
  //#####################################################
  //cout<<"reading "<<buff1<<"....."<<endl;
  if(gSystem->GetPathInfo(buff1,fs)){
    cout<<"Cannot open file "<<buff1<<endl;
    exit(1);
  }
 
  IngridEventSummary* evt = new IngridEventSummary();
  //TFile*            rfile = new TFile(Form("/home/daq/data/dst/ingrid_%08d_%04d_processed.root", 
  TFile*            rfile = new TFile(buff1,
				      "read");
  TTree*             tree = (TTree*)rfile -> Get("tree");
  TBranch*          EvtBr = tree->GetBranch("fDefaultReco.");
  EvtBr                   ->SetAddress(&evt);
  tree                    ->SetBranchAddress("fDefaultReco.", &evt);
  int                nevt = (int)tree -> GetEntries();
  //cout << "Total # of events = " << nevt <<endl;
  if(nevt==0)exit(1);
  //#### make rootfile after analysis #####
  //#######################################
  //sprintf(buff1,"/home/daq/data/dst/ingrid_%08d_0000_tclster.root",run_number);
  //if( !rename ){
  //sprintf(Output, "%s/ingrid_%08d_%04d_tclster.root", 
  //    dst_data, run_number, sub_run_number); 
  //}
  //if(cosmic){
  //sprintf(Output, "%s/ingrid_%08d_%04d_tclster.root", 
  //    cosmic_data, run_number, sub_run_number); 
  //}


  IngridHitSummary*    inghitsum;
  IngridHitSummary*  inghitsum_t; 
  IngridBasicReconSummary* recon = new IngridBasicReconSummary();
  Hit                        hit;
  //for(int ievt=0; ievt<nevt; ievt++){
  for(int ievt=0; ievt<5; ievt++){
    evt      -> Clear();
    tree     -> GetEntry(ievt);
    int nhitoff=0;

    for( int mod=14; mod<=15; mod++ ){   //### Module Loop
       for( int cyc=Scyc; cyc<Ncyc; cyc++ ){  //### Cycle Loop
	 int ninghit = evt -> NIngridModHits(mod, cyc);
	 nhitoff+=ninghit;
	 
       }
     }
    if(nhitoff==0){
      cout << "***************caution!!*****************" << endl;
      cin.get();
    }
    //cout << "------------- " << ievt <<"\t" << nhitoff<< " --------------" << endl;
  }

}
