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
  Int_t run_number=0;
  bool  mc = false;
  sprintf(Output,"temp.root");
  while ((c = getopt(argc, argv, "r:f:b:mo:")) != -1) {
    switch(c){
    case 'r':
      run_number=atoi(optarg);
      sprintf(FileName,"/home/daq/data/dst/ingrid_%08d_0000_recon.root",run_number);
      break;
    case 'f':
      sprintf(FileName,"%s",optarg);
      break;
    case 'm':
      mc = true;
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
  vector<int>  nhit;
  vector<int>  nmod;
  vector<int>  ncyc;
  int         ltime;
  int    nhitallmod;

  wtree -> Branch("nhit",   &nhit);
  wtree -> Branch("nmod",   &nmod);
  wtree -> Branch("ncyc",   &ncyc);
  wtree -> Branch("ltime",  &ltime, "ltime/I");
  wtree -> Branch("nhitallmod",&nhitallmod,"nhitallmod/I");

  for(int ievt = 0; ievt < nevt; ++ievt){
    //for(int ievt = 0; ievt < 1000; ++ievt){
    if( ievt%100 == 0 )cout<<ievt<<endl;
    evt  -> Clear();
    tree -> GetEntry(ievt);

    nhit.clear();
    nmod.clear();
    ncyc.clear();

    //BeamInfoSummary* beamsum = (BeamInfoSummary*)evt->GetBeamSummary(0);
    //ltime = beamsum -> trg_sec;
    ltime = evt -> time;
    nhitallmod = 0;
    for(int mod=0; mod<14; mod++){
      for(int cyc=0; cyc<23; cyc++){
	nhit.push_back( evt -> NIngridModHits(mod, cyc) );
	nmod.push_back( mod );
	ncyc.push_back( cyc );
	if(cyc==0)nhitallmod = nhitallmod + evt -> NIngridModHits(mod, cyc);
      }
    }
    wtree -> Fill();
  }
  wtree -> Write();
  f->Write();
  f->Close();

  rfile -> Close();
  //app.Run();
  cout<<totalpot<<endl;
}
 
