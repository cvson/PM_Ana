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
#include <TClonesArray.h>
#include <TObject.h>
#include <TEventList.h>
#include <TBranch.h>
#include <TSystem.h>
#include <TBrowser.h>
#include <TMath.h>
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

  //#####
  TFile*  wfile = new TFile("temp.root","recreate");
  int hitmod;
  float ax;
  float ay;
  float trueangle;
  float dir[3];
  float angle;
  int   inttype;
  double norm;
  double weight;
  double totcrsne;
  int    rec;
  int    nactpln;

  TTree*   wtree = new TTree("tree","tree");
  wtree->Branch("ax",&ax,"ax/F");
  wtree->Branch("ay",&ay,"ay/F");
  wtree->Branch("dir",dir,"dir[3]/F");
  wtree->Branch("trueangle",&trueangle,"trueangle/F");
  wtree->Branch("angle",&angle,"angle/F");
  wtree->Branch("inttype",&inttype,"inttype/I");
  wtree->Branch("norm",    &norm,    "norm/D");
  wtree->Branch("weight",  &weight,  "weight/D");
  wtree->Branch("totcrsne",&totcrsne,"totcrsne/D");
  wtree->Branch("rec",     &rec,     "rec/I");
  wtree->Branch("nactpln", &nactpln, "nactpln/I");

  IngridSimParticleSummary*        simpar;
  IngridSimVertexSummary*          simver;
  IngridBasicReconSummary*         brec;

  for(int ievt = 0; ievt < nevt; ++ievt){

    if( ievt%1000 == 0 )cout<<ievt<<endl;
    evt  -> Clear();
    tree -> GetEntry(ievt);

    simver = (IngridSimVertexSummary*)evt->GetSimVertex(0);
    if( simver -> inttype > 30 ) //### get CC
      continue;
    inttype = simver->inttype;
    

    int nsim = evt -> NIngridSimParticles();
    for(int i=0; i<nsim; i++){
      simpar = (IngridSimParticleSummary*)evt->GetSimParticle(i);
      if( simpar->pdg != 13 ) //### get muon
	continue;
      dir[0]  = simpar -> dir[0];
      dir[1]  = simpar -> dir[1];
      dir[2]  = simpar -> dir[2];
    
      double a = sqrt(dir[1]*dir[1]+dir[0]*dir[0])/dir[2];
      if( a >= 0 )
	trueangle = TMath::ATan( a ) / TMath::Pi() * 180;
      if( a <  0 )
	trueangle = 180 + TMath::ATan( a ) / TMath::Pi() * 180 ;
    }
      
    for(int i=0;i<evt->NIngridBasicRecons(); i++){
      brec = (IngridBasicReconSummary*)evt->GetBasicRecon(i);
      if(brec->hastrk && brec->matchtrk && 
	 !(brec->vetowtracking) && !(brec->edgewtracking) && brec->nactpln>5){
	ax = brec->thetax;
	ay = brec->thetay;
	angle = brec->angle;
	nactpln = brec->nactpln;
	rec      = 1;

      }
      else{
	rec      = 0;
      }
      weight   = simver->weight;
      norm     = simver->norm;
      totcrsne = simver->totcrsne;
      wtree -> Fill();
    }



  }



  wtree->Write();
  wfile->Write();
  wfile->Close();
  rfile -> Close();

}
 
