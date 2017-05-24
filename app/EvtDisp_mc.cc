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
//##### from MC data
int            intmode; 
float              Enu; 
float           vertex[3]; 

vector<double>  anape;
vector<int>   anaview;
vector<int>    anapln;
vector<int>     anach;

int main(int argc,char *argv[]){
  fINGRID_Dimension = new INGRID_Dimension();

  char buff1[200],buff2[200];
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  gStyle->SetOptStat(0);
 
  Int_t run_number;
  Int_t c=-1;  char FileName[300];
  bool cosmicflag=false;
  bool clustring=false;
  bool sflag=false;
  while ((c = getopt(argc, argv, "sr:hc")) != -1) {
    switch(c){
    case 's':
      sflag = true;
      break;
    case 'r':
      run_number=atoi(optarg);
      break;
    case 'c':
      clustring=true;
      break;
    case 'h':
      cout<<"help menu"<<endl;
      cout<<"-r [run_number]"<<endl;
      break;
    }
  }

  //######### beam timing adjustment ####################
  //#####################################################
  FileStat_t fs;
  double cTModif[50000];
  for(int i=0; i<50000; i++)cTModif[i]=-777;
  sprintf(buff1,"/home/daq/0911NovBeam/beam_timing_modification_at_INGRID.txt");
  //#### beam timing is changed largely when high power trial
  //#### so read file for special timing modification file
  cout<<"reading "<<buff1<<"....."<<endl;
  if(gSystem->GetPathInfo(buff1,fs)){
    cout<<"Cannot open file "<<buff1<<endl;
    exit(1);
  }
  ifstream FileTiming(buff1);
  int    spill;
  double modif;
  while(FileTiming>>spill>>modif){ 
    cTModif[spill]=modif;
  }
  for(int i=16622; i<=16993; i++){if(cTModif[i]==-777)cTModif[i] =  0;}  //#### 11/20 commissioning
  for(int i=31337; i<=31920; i++){if(cTModif[i]==-777)cTModif[i] = 10;}  //#### 11/21 commissioning
  for(int i=43272; i<=46004; i++){if(cTModif[i]==-777)cTModif[i] = 30;}  //#### 11/22 commissioning
  for(int i=28329; i<=28372; i++){if(cTModif[i]==-777)cTModif[i] = 50;}  //#### 11/25 commisssioning



  //sprintf(buff1,"/home/daq/data/test/ingrid.root");
  sprintf(buff1,"/home/daq/data/root_new/ingrid_%08d_0000.root",run_number);
  if(sflag){
    sprintf(buff1,"/home/daq/data/root_new/0912beam.root");
  }

  if(gSystem->GetPathInfo(buff1,fs)){
    cout<<"Cannot open file "<<buff1<<endl;
    exit(1);
  }

  TFile*               f  = new TFile(buff1,"read");
  TTree*            tree  = (TTree*)f->Get("tree");
  Int_t        NEvt       = tree->GetEntries();
  cout<<"---- # of Evt    = "<<NEvt<<"---------"<<endl;
  cin.get();
  Int_t           NumEvt;
  Int_t           nSpill;
  Long_t          UTime;
  pe   = 0;
  nsec = 0;
  view = 0;
  pln  = 0;
  ch   = 0;

  tree->SetBranchAddress("NumEvt",&NumEvt   );
  tree->SetBranchAddress("adc"  ,&adc   );
  tree->SetBranchAddress("pe"   ,&pe   );
  tree->SetBranchAddress("nsec" ,&nsec );
  tree->SetBranchAddress("view" ,&view );
  tree->SetBranchAddress("pln"  ,&pln  );
  tree->SetBranchAddress("ch"   ,&ch   );
  tree->SetBranchAddress("posxy",&posxy  );
  tree->SetBranchAddress("posz"   ,&posz   );
  tree->SetBranchAddress("fMod" ,&fMod );
  tree->SetBranchAddress("Cycle",&Cycle);
  tree->SetBranchAddress("nSpill",&nSpill);
  tree->SetBranchAddress("UTime",&UTime);
  tree->SetBranchAddress("Enu"  ,&UTime);
  tree->SetBranchAddress("intmode",&intmode);
  tree->SetBranchAddress("fvertex[3]"  ,vertex);

  vector<Hit> allhit;
  Int_t n=1;
  while(n>=0){
  
    cout<<"Entry?"<<endl;
    cin>>n;
    tree->GetEntry(n);
    cout<<"-----------------    "
	<<"Event # :"<<NumEvt<<endl
	<<"Spill # :"<<nSpill<<endl
	<<"UTime : " <<UTime <<endl
	<<"Mod  # : "<<fMod <<endl
	<<"Cycle  : "<<Cycle <<endl
	<<"    -----------------"<<endl;
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
     hit.posxy= posxy->at(i);
     hit.posz = posz ->at(i);
     allhit.push_back(hit);
   }

   fSortTime(allhit);
   if(clustring){
     vector<Hit> hitclster;
     while(fFindTimeClster(allhit, hitclster, fTime)){
       if(fTime>1000000000)fTime = fTime - 1000000000;
       fSortTime(hitclster);
       Int_t nCls = hitclster.size();
       //fTime = fTime + cTModif[nSpill];
    
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
	if(trgbit&& (  (0x400)>>(10 - hitclster[i].pln) ) ){
	  fMeanPe += hitclster[i].pe;
	  nEnt++;
	}
      }
      if(nEnt>0)fMeanPe = fMeanPe/nEnt;
      else if(nEnt==0)fMeanPe=0;
        fSortTime(allhit);
	if(nActPln>=2&&fMeanPe>7.5){
	  Draw_Module();
	  Draw_Info(fMod, Cycle, nSpill, fTime);  
	  Draw_Hit(hitclster);
	  Print(hitclster);
	  cout<<"interaction mode: "<<intmode   <<endl;
	  cout<<"nu energy       : "<<Enu       <<endl;
	  cout<<"Vertex X        : "<<vertex[0] <<endl;
	  cout<<"Vertex Y        : "<<vertex[1] <<endl;
	  cout<<"Vertex Z        : "<<vertex[2] <<endl;
	}
	  hitclster.clear();
     }
   }//clustring

   else{
     Draw_Module();
     Draw_Info(fMod, Cycle, nSpill, fTime);  
     Draw_Hit(allhit);
     Print(allhit);

   }
  }//
  f->Close();
}
