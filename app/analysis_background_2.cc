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
#include <TString.h>
#include <TSystem.h>

#include "TApplication.h"
#include "analysis_background.hxx"
#include "setup.hxx"
#include "root_setup.hxx"

vector<int>*       adc;
vector<double>*     pe;
vector<int>*      nsec;
vector<int>*      view;
vector<int>*       pln;
vector<int>*        ch;
Int_t             fMod;
Long_t            fTime;
Int_t            Cycle;

int main(int argc,char *argv[]){
  char buff1[200],buff2[200];
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  Int_t run_number;
  Int_t c=-1;  char FileName[300];
  bool cosmicflag=false;
  while ((c = getopt(argc, argv, "r:h")) != -1) {
    switch(c){
    case 'r':
      run_number=atoi(optarg);
      break;
    case 'h':
      cout<<"help menu"<<endl;
      cout<<"-r [run_number]"<<endl;
      break;
    }
  }


  sprintf(buff1,"/home/daq/data/root_new/ingrid_%08d_0000.root",run_number);
  FileStat_t fs;
  if(gSystem->GetPathInfo(buff1,fs)){
    cout<<"Cannot open file "<<buff1<<endl;
    exit(1);
  }


  TFile*               f  = new TFile(buff1,"read");
  TTree*            tree  = (TTree*)f->Get("tree");
  Int_t        Evt        = tree->GetEntries();
  cout<<"---- # of Evt    = "<<Evt<<"---------"<<endl;
  cin.get();
  pe   = 0;
  nsec = 0;
  view = 0;
  pln  = 0;
  ch   = 0;
  Int_t  NumEvt;
  Int_t  nSpill;
  Long_t UTime;

  tree->SetBranchAddress("NumEvt",&NumEvt   );
  tree->SetBranchAddress("adc"   ,&adc   );
  tree->SetBranchAddress("pe"    ,&pe   );
  tree->SetBranchAddress("nsec"  ,&nsec );
  tree->SetBranchAddress("view"  ,&view );
  tree->SetBranchAddress("pln"   ,&pln  );
  tree->SetBranchAddress("ch"    ,&ch   );
  tree->SetBranchAddress("fMod"  ,&fMod );
  tree->SetBranchAddress("Cycle" ,&Cycle);
  tree->SetBranchAddress("nSpill",&nSpill);
  tree->SetBranchAddress("UTime" ,&UTime);

  sprintf(buff1,"/home/daq/data/backgroudstudy/ingrid_%08d_0001.root",run_number);
  TFile*              rf  = new TFile(buff1,"recreate");
  TTree*            ModTree;
  Int_t             nActPln = 0;
  Float_t           fMeanPe = 0;
  Int_t            nlActPln[15];
  Float_t          flMeanPe[15];
  ModTree =  new TTree("ModTree","ModTree");
  ModTree -> Branch("NumEvt"      ,&NumEvt  ,"NumEvt/I"        );
  ModTree -> Branch("nSpill"      ,&nSpill  ,"nSpill/I"        );
  ModTree -> Branch("UTime"       ,&UTime   ,"UTime/L"        );
  ModTree -> Branch("fMod"        ,&fMod    ,"fMod/I"        );
  ModTree -> Branch("fTime"       ,&fTime   ,"fTime/L"       );
  ModTree -> Branch("nActPln"     ,&nActPln ,"nActPln/I"     );
  ModTree -> Branch("nlActPln[15]", nlActPln,"nlActPln[15]/I");
  ModTree -> Branch("fMeanPe"     ,&fMeanPe ,"fMeanPe/F"     );
  ModTree -> Branch("flMeanPe[15]", flMeanPe,"flMeanPe[15]/F");
  Int_t  Entry;
  ModTree -> Branch("Entry"       ,&Entry   ,"Entry/I"     );

  for(int n=0; n<Evt; n++){
    tree->GetEntry(n);
    Entry = n;
    if(n%10000==0)cout<<"-- analysis Clster # = "<<n<<" ---"<<endl;
    Int_t nHit = pe->size();

    //######### fill hit class ####################
    allhit.clear();
    for(Int_t i=0;i<nHit;i++){
      Hit hit  ;
      hit.pe   = pe  ->at(i);
      hit.time = nsec->at(i);
      hit.view = view->at(i);
      hit.pln  = pln ->at(i);
      hit.ch   = ch  ->at(i);
      allhit.push_back(hit);
    }
    fSortTime(allhit);
    //cout<<"--event:"<<Evt<<" Cycle:"<<Cycle<<" Mod:"<<fMod<<endl;

    vector<Hit> hitclster;
    while(fFindTimeClster(allhit, hitclster, fTime)){
      if(fTime>1000000000)fTime=fTime-1000000000;
      fSortTime(hitclster);
      Int_t nCls = hitclster.size();
      //Print(hitclster);
      //######### check active plane ################
      Int_t   trgbit  = 0;
      nActPln = 0;
      for(Int_t i=0;i<nCls;i++){
	for(Int_t j=i+1;j<nCls;j++){
	  if(hitclster[i].pln==hitclster[j].pln){
	    if(hitclster[i].view!=hitclster[j].view){
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
      fMeanPe          = 0;
      Int_t      nEnt  = 0;
      for(Int_t i=0;i<nCls;i++){
	if(trgbit&& (  (0x400)>>(10 - hitclster[i].pln) ) ){
	  fMeanPe += hitclster[i].pe;
	  nEnt++;
	}
      }
      fMeanPe = fMeanPe/nEnt;

      //########## calculate number of activities  #######
      //########## with several threshold          #######
      //########## also calculate mean p.e.        #######
      for(Int_t thre=2; thre<15;thre++){
	nlActPln[thre]=0;
	double threshold = 0.5 + thre;
	trgbit=0;
	for(Int_t i=0;i<nCls;i++){
	  for(Int_t j=i+1;j<nCls;j++){
	    if(hitclster[i].pln==hitclster[j].pln){
	      if(hitclster[i].view!=hitclster[j].view){
		if(hitclster[i].pe>threshold&&hitclster[j].pe>threshold){
		  trgbit=trgbit|((0x400)>>(10-hitclster[i].pln));
		}
	      }//view difference
	    }//pln coince
	  }//j
	}//i
	for(Int_t i=1;i;i=i<<1){
	  if(trgbit&i){
	    nlActPln[thre]++;
	  }
	}
	flMeanPe[thre]          = 0;
	nEnt  = 0;
	for(Int_t i=0;i<nCls;i++){
	  if(trgbit&& (  (0x400)>>(10 - hitclster[i].pln) ) ){
	    flMeanPe[thre] += hitclster[i].pe;
	    nEnt++;
	  }
	}
	flMeanPe[thre] = flMeanPe[thre]/nEnt;
      }//thre


      //Print active>=2 && fMeanPe>6.5
      if( ( fMeanPe>6.5&&nActPln>1 ) ){
	cout<<"**** atc>=2, Mean>6.5 **** Spill = "<<nSpill
	    <<" nEvnt  ="                 <<n
	    <<" Cycle  ="                 <<Cycle
	    <<" fMod   ="                 <<fMod
	    <<" ActPln ="                 <<nActPln
	    <<" Mean   ="                 <<fMeanPe<<endl;
      }

      //Print active>=1 && fMeanPe>9.5
      /*
      if( ( flMeanPe[6]>9.5&&nlActPln[6]>0 ) ){
	cout<<"**** atc>=1, Mean>9.5 **** Spill = "<<nSpill
	    <<" nEvnt  ="                 <<n
	    <<" Cycle  ="                 <<Cycle
	    <<" fMod   ="                 <<fMod
	    <<" ActPln ="                 <<nlActPln[6]
	    <<" Mean   ="                 <<flMeanPe[6]
	    <<endl;
      }
      */


      /*
      bool temp[11];
      cout<<"-----------"<<endl;
      for(int i=0;i<11;i++){
	if(i==0)cout<<"a";
	else cout<<10-i;

	temp[i]=trgbit%2;
	trgbit=trgbit/2;
      }
      cout<<endl;
      cout<<"-----------"<<endl;
      for(int i=10;i>=0;i--){
	cout<<temp[i];
      }
      cout<<endl;
      cin.get();
      */


      hitclster.clear();
      ModTree -> Fill();
    }

    /*



    */



    //########## debug ############################
    /*
    if(fMeanPe>50){
      cout<<"----- Mod:" << fMod<<endl;
      for(Int_t i=0;i<nHit;i++){
	cout<<" view:"<<view->at(i)
	    <<" pln:" <<pln ->at(i)
	    <<" ch:"  <<ch  ->at(i)
	    <<" pe:"  <<pe  ->at(i)
	    <<endl;
      }
    }
    */

    //######### Fill ##############################
    
    
  }//clster loop



  //######## Write and Close ####################
  ModTree -> Write();
  rf      -> Write();
  rf      -> Close();
  f       -> Close();

}
 
