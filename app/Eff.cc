#define TEST 1
//##### Standard C++ lib. ######
#include<iostream>
#include<sstream>
#include<fstream>
using namespace std;
#include <iomanip.h>
#include <sys/stat.h>
#include "math.h"
//##### Root Library ###########
#include <TROOT.h>
#include <TStyle.h>
#include <TH1.h>
#include <TH2.h>
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
#include "BeamInfoSummary.h"
#include "NeutInfoSummary.h"
IngridBasicReconSummary* ingbasic;
//BeamInfoSummary*         beaminfo;
IngridSimVertexSummary*  simver;
NeutInfoSummary*         neutinfo;

#define POTBIN 20
#define POTMAX 10

float weight,Enu;
bool CC;

TH1F *atot = new TH1F("atot","atot",POTBIN,0,POTMAX);
TH1F *acc  = new TH1F("acc", "acc", POTBIN,0,POTMAX);
TH1F *anc  = new TH1F("anc", "anc", POTBIN,0,POTMAX);

TH1F *dtot = new TH1F("dtot","dtot",POTBIN,0,POTMAX);
TH1F *dcc  = new TH1F("dcc", "dcc", POTBIN,0,POTMAX);
TH1F *dnc  = new TH1F("dnc", "dnc", POTBIN,0,POTMAX);

TH1F *htot = new TH1F("htot","htot",POTBIN,0,POTMAX);
TH1F *hcc  = new TH1F("hcc", "hcc", POTBIN,0,POTMAX);
TH1F *hnc  = new TH1F("hnc", "hnc", POTBIN,0,POTMAX);



void  fill_true_info(){
  float xnu,ynu;
  int x,y;
  xnu = simver -> xnu;
  ynu = simver -> ynu;
  x=(int)(xnu+75)/150;
  y=(int)(ynu+75)/150;
  xnu-=x*150;
  ynu-=y*150;

  if(fabs(xnu)<50&&fabs(ynu)<50){
    atot->Fill(Enu,weight);
    if(CC)
      acc->Fill(Enu,weight);
    else
      anc->Fill(Enu,weight);
  }
}


void fill_ing(){


  if(ingbasic->hastrk && ingbasic->matchtrk && 
     /*ingbasic->ontime &&*/ !(ingbasic->vetowtracking) &&
     !(ingbasic->edgewtracking) ){
    
    int mod=ingbasic->hitmod;
    //int cyc=ingbasic->hitcyc;
    if(mod<14){

      dtot->Fill(Enu,weight);
      if(CC)
	dcc->Fill(Enu,weight);
      else
	dnc->Fill(Enu,weight);

    }

  }
  
  
}



void out(){

  htot->Divide(atot,dtot);
  /*
  for(int i=0;i<POTBIN;i++){
    if(htot -> GetBinContent(i+1)==0)
      htot->SetBinError(i+1,0);
    else
      htot->SetBinError(i+1,sqrt( hnu->GetBinContent(i+1) )/hpot -> GetBinContent(i+1));
  }
  */
  TCanvas *c1= new TCanvas("c1","c1",0,0,600,400);
  //hrat->Draw("E");
  htot->Draw();
  c1->Print("evtrate.C");
}

int main(int argc,char *argv[]){
  TROOT        root ("GUI","GUI");
  TApplication app  ("App",0,0);

  Int_t c=-1;  char FileName[300]; 
  sprintf(FileName,"temp.root");
  while ((c = getopt(argc, argv, "f:")) != -1) {
    switch(c){
    case 'f':
      sprintf(FileName,"%s",optarg);
      break;
      //case 'o':
      //sprintf(Output,"%s",optarg);
      //break;
    }
  }

  FileStat_t fs;
  cout<<"reading and processsing"<<FileName<<"....."<<endl;
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

  //clear();
  for(int ievt=0; ievt<nevt; ievt++){
    if(ievt%10000==0)cout << ievt << endl;
    evt       -> Clear();
    tree      -> GetEntry(ievt);

    //beaminfo  = (BeamInfoSummary*)(evt->GetBeamSummary(0));

    simver = (IngridSimVertexSummary*)(evt->GetSimVertex(0));
    weight = (simver->norm)*(simver->totcrsne);
    Enu    = (simver->nuE);

    neutinfo  = (NeutInfoSummary*)( evt -> GetNeut(0) );
    if((neutinfo->Mode)<30)CC=true;
    else                   CC=false;

    fill_true_info();

    for(int ibasic = 0; ibasic < evt->NIngridBasicRecons(); ibasic++){
      ingbasic     = (IngridBasicReconSummary*)(evt->GetBasicRecon(ibasic));
      fill_ing();
    }
  }//ievt
  out();

}
