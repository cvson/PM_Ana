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
#include "setup.hxx"
#include "root_setup.hxx"

vector<double>*     pe;
vector<int>*      nsec;
vector<int>*      view;
vector<int>*       pln;
vector<int>*        ch;
Int_t             fMod;
Int_t            fTime;

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

  //sprintf(buff1,"/home/daq/data/test/ingrid.root");
  sprintf(buff1,"/home/daq/data/backgroudstudy/test.root");
  FileStat_t fs;
  if(gSystem->GetPathInfo(buff1,fs)){
    cout<<"Cannot open file "<<buff1<<endl;
    exit(1);
  }


  TFile*               f  = new TFile(buff1,"read");
  TTree*            tree  = (TTree*)f->Get("tree");
  Int_t        NumClster  = tree->GetEntries();
  cout<<"---- # of Clsters = "<<NumClster<<"---------"<<endl;
  cin.get();
  pe   = 0;
  nsec = 0;
  view = 0;
  pln  = 0;
  ch   = 0;
  tree->SetBranchAddress("pe"   ,&pe   );
  tree->SetBranchAddress("nsec" ,&nsec );
  tree->SetBranchAddress("view" ,&view );
  tree->SetBranchAddress("pln"  ,&pln  );
  tree->SetBranchAddress("ch"   ,&ch   );
  tree->SetBranchAddress("fMod" ,&fMod );
  tree->SetBranchAddress("fTime",&fTime);

  TFile*              rf  = new TFile("/home/daq/data/backgroudstudy/ana.root","recreate");
  TTree*            ModTree;
  Int_t             nActPln = 0;
  Float_t           fMeanPe = 0;
  Int_t            nlActPln[10];
  ModTree =  new TTree("ModTree","ModTree");
  ModTree -> Branch("fMod"        ,&fMod    ,"fMod/I"        );
  ModTree -> Branch("fTime"       ,&fTime   ,"fTime/I"       );
  ModTree -> Branch("nActPln"     ,&nActPln ,"nActPln/I"     );
  ModTree -> Branch("nlActPln[10]", nlActPln,"nlActPln[10]/I");
  ModTree -> Branch("fMeanPe"     ,&fMeanPe ,"fMeanPe/F"     );
  

  for(int n=0; n<NumClster; n++){
    tree->GetEntry(n);
    if(n%100==0)cout<<"-- analysis Clster # = "<<n<<" ---"<<endl;
    Int_t nHit = pe->size();

    //######### check active plane ################
    Int_t   trgbit  = 0;
    nActPln = 0;
    for(Int_t i=0;i<nHit;i++){
      for(Int_t j=i+1;j<nHit;j++){
	if(pln->at(i)==pln->at(j)){
	  if(view->at(i)!=view->at(j)){
	    trgbit=trgbit|((0x400)>>(10-pln->at(i)));
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
    for(Int_t i=0;i<nHit;i++){
      if(trgbit&& (  (0x400)>>(10-pln->at(i)) ) ){
	fMeanPe += pe->at(i);
	nEnt++;
      }
    }
    fMeanPe = fMeanPe/nEnt;


    //########## calculate number of activities  #######
    //########## with several threshold          #######
    for(Int_t thre=3; thre<10;thre++){
      nlActPln[thre]=0;
      double threshold = 0.5 + thre;
      trgbit=0;
      for(Int_t i=0;i<nHit;i++){
	for(Int_t j=i+1;j<nHit;j++){
	  if(pln->at(i)==pln->at(j)){
	    if(view->at(i)!=view->at(j)){
	      if(pe->at(i)>threshold && pe->at(j)>threshold){
		trgbit=trgbit|((0x400)>>(10-pln->at(i)));
	      }//high activity
	    }//view difference
	  }//pln coince
	}//j
      }//i
      for(Int_t i=1;i;i=i<<1){
	if(trgbit&i){
	  nlActPln[thre]++;
	}
      }
    }




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
    ModTree -> Fill();
    //h->Fill(n);
  }//clster loop



  //######## Write and Close ####################
  ModTree -> Write();
  rf      -> Write();
  rf      -> Close();
  f       -> Close();

}
 
