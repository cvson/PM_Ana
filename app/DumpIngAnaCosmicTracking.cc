
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
#include "ReconTrackBasic.cxx"
#include "IngEvtDisp.cxx"
#include "IngridConstants.h"
#include "INGRID_Dimension.hxx"
#include "INGRID_BadCh_mapping.cxx"
#include "INGRID_Ch_config.cxx"
INGRID_BadCh_mapping* badmap;
INGRID_Ch_config* chmap;

int main(int argc,char *argv[]){
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  badmap = new INGRID_BadCh_mapping();
  chmap  = new INGRID_Ch_config();
  int run_number;
  int sub_run_number=0;
  char FileName[300];
  char Output[300]; 
  int c=-1;
  while ((c = getopt(argc, argv, "r:s:")) != -1) {
    switch(c){
    case 'r':
      run_number=atoi(optarg);
      break;
    case 's':
      sub_run_number=atoi(optarg);
      break;
    }
  }
  FileStat_t fs;
  ifstream timing;


  sprintf(FileName,"/home/daq/data/cosmic/ingrid_%08d_%04d_cosmicana.root",
	  run_number,
	  sub_run_number);
  sprintf(Output,"/home/daq/data/cosmic/ingrid_%08d_%04d_cosmicana.txt",
	  run_number,
	  sub_run_number);


  Int_t  MissCosmic[nMod][nView][nPln][nCh]={0};
  Int_t  HitCosmic [nMod][nView][nPln][nCh]={0};
  //######## read root file #############################
  //#####################################################
  cout<<"reading "<<FileName<<"....."<<endl;
  if(gSystem->GetPathInfo(FileName,fs)){
    cout<<"Cannot open file "<<FileName<<endl;
    exit(1);
  }
  ofstream wf(Output);
  IngridEventSummary* evt = new IngridEventSummary();
  TFile*            rfile = new TFile(FileName,"read");
  TTree*             tree = (TTree*)rfile -> Get("tree");
  TBranch*          EvtBr = tree->GetBranch("fDefaultReco.");
  EvtBr                   ->SetAddress(&evt);
  tree                    ->SetBranchAddress("fDefaultReco.", &evt);
  int                nevt = (int)tree -> GetEntries();
  cout << "Total # of events = " << nevt <<endl;


  IngridHitSummary*          inghitsum;
  for(int ievt=0; ievt<nevt; ievt++){
    if(ievt%100==0) cout << "analysis " << ievt << endl;
    evt                -> Clear();
    tree               -> GetEntry(ievt);
    for(int imod=0; imod<nMod;imod++){
      for(int icyc=0; icyc<nCyc;icyc++){
	for(int ihit=0; ihit<evt->NIngridModHits(imod, icyc); ihit++){
	  inghitsum = (IngridHitSummary*)evt->GetIngridModHit(ihit, imod, icyc);
	  int  mod   = inghitsum->mod;
	  int  view  = inghitsum->view;
	  int  pln   = inghitsum->pln;
	  int  ch    = inghitsum->ch;
	  bool hit   = inghitsum->hitcosmic;
	  bool miss  = !(inghitsum->hitcosmic)&&(inghitsum->gocosmic);
	  if( !badmap->badchannel(mod, view, pln, ch) ){
	    if( hit )
	      HitCosmic[mod][view][pln][ch]++;
	    if( miss )
	      MissCosmic[mod][view][pln][ch]++;
	  }//bad
	}//ihit
      }//icyc
    }//imod
  }//ievt

  int tmp;
  for(int imod=0; imod<nMod; imod++){
    for(int iview=0; iview<nView; iview++){
      for(int ipln=0; ipln<nPln; ipln++){
	for(int ich=0; ich<nCh; ich++){

	  if(chmap->get_global_ch(imod, iview, ipln, ich, &tmp)){
	    wf << imod  << "\t" 
	       << iview << "\t" 
	       << ipln  << "\t" 
	       << ich   << "\t" 
	       << HitCosmic[imod][iview][ipln][ich] << "\t"
	       << MissCosmic[imod][iview][ipln][ich]+HitCosmic[imod][iview][ipln][ich] <<endl;

	  }
	}
      }
    }
  }
  wf.close();




}
 
