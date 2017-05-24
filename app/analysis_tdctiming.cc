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
#include "setup.hxx"
//#include "root_setup.hxx"
Int_t             fMod;
Long_t            fTime;
Int_t            Cycle;
vector<int>*       adc;
vector<long>*      tdc;
vector<int>*      view;
vector<int>*       pln;
vector<int>*        ch;
vector<double>*     pe;
vector<int>*      nsec;
 

vector<double>  anape;
vector<int>   anaview;
vector<int>    anapln;
vector<int>     anach;


int main(int argc,char *argv[]){
  fINGRID_Dimension = new INGRID_Dimension();

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
  sprintf(buff1,"/home/daq/data/root_new/ingrid_%08d_0000.root",run_number);
  FileStat_t fs;
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
  pe   = 0;
  nsec = 0;
  view = 0;
  pln  = 0;
  ch   = 0;
  tree->SetBranchAddress("NumEvt",&NumEvt   );
  tree->SetBranchAddress("adc"  ,&adc   );
  tree->SetBranchAddress("pe"   ,&pe   );
  tree->SetBranchAddress("tdc"  ,&tdc  );
  tree->SetBranchAddress("nsec" ,&nsec );
  tree->SetBranchAddress("view" ,&view );
  tree->SetBranchAddress("pln"  ,&pln  );
  tree->SetBranchAddress("ch"   ,&ch   );
  tree->SetBranchAddress("fMod" ,&fMod );
  tree->SetBranchAddress("Cycle",&Cycle);

  sprintf(buff1,"/home/daq/data/root_new/ingrid_%08d_0128_timing.root",run_number);
  TFile*              rf  = new TFile(buff1,"recreate");
  TTree*           rtree  = new TTree("tree","for timing analysis");
  Long_t  ltdc[11][2];
  rtree->Branch("ltdc[11][2]",     &ltdc,    "ltdc[11][2]/L");
  rtree->Branch("fMod"       ,     &fMod,    "fMod/I");
  TH2F*           fHdifftdcx = new TH2F("fHdifftdcx","fHdifftdcx", 151, 0, 151, 81, -40.5, 40.5);
  TH2F*           fHdifftdcy = new TH2F("fHdifftdcy","fHdifftdcy", 151, 0, 151, 81, -40.5, 40.5);
  vector<Hit> allhit;
  Int_t nPenet=0;
  for(int n=0; n<NEvt; n++){
    // if(n==1000000)break;
    tree->GetEntry(n);
    if(Cycle==17){
      if(n%1000==0)cout<<"-- analysis Evt # = "<<n<<" ---"<<endl;
      //cout<<"-- analysis Evt # = "<<n<<" ---"<<endl;
      Int_t nHit = pe->size();
 
      //######### fill hit class ####################
      allhit.clear();
      for(Int_t i=0;i<nHit;i++){
	Hit hit  ;
	hit.pe   = pe  ->at(i);
	hit.time = nsec->at(i);
	hit.tdc  = tdc ->at(i);
	hit.view = view->at(i);
	hit.pln  = pln ->at(i);
	hit.ch   = ch  ->at(i);
	allhit.push_back(hit);
      }
      //fSortTime(allhit);

      //######### check active plane ################
      //######### for easy cut       ################
      Int_t     trgbit  = 0;
      Int_t     nActPln = 0;
      for(Int_t i=0;i<nHit;i++){
	for(Int_t j=i+1;j<nHit;j++){
	  if(allhit[i].pln==allhit[j].pln){
	    if(allhit[i].view!=allhit[j].view){
	      trgbit=trgbit|((0x400)>>(10-allhit[i].pln));
	    }//view difference
	  }//pln coince
	}//j
      }//i
      for(Int_t i=1;i<=1024;i=i<<1){
	if(trgbit&i){
	  nActPln++;
	}
      }

      if(nActPln==11){// all tracking planes are active
	nPenet++;
	Double_t  Mpe[11][2];
	for(Int_t i=0;i<11;i++){
	  for(Int_t j=0;j<2;j++){
	    Mpe[i][j]=0;;
	  }
	}
	for(Int_t i=0; i<nHit; i++){
	  Int_t p = allhit[i].pln;
	  Int_t v = allhit[i].view;
	  if( p < 11 ){//for tracking plane
	    if(Mpe[p][v]<allhit[i].pe){
	      ltdc[p][v] = allhit[i].tdc;
	      Mpe [p][v] = allhit[i].pe;
	    }//
	  }
	}

	//
	long basex = ltdc[5][0];
	long basey = ltdc[5][1];
	for(int i=0; i<11; i++){
	  fHdifftdcx->Fill(fMod*11 + i, ltdc[i][0]-basex);
	  fHdifftdcy->Fill(fMod*11 + i, ltdc[i][1]-basey);
	}

	rtree->Fill();
      }
    }//Cycle 17
  }//Event loop
  //######## analysis ###########################
  ofstream  wf("test.dat");
  ofstream  wf2("test2.dat");
  TCanvas*  c1 = new TCanvas("c1","c1",10,10,600,400);
  TH1F*      h;

  TF1*    gaus = new TF1("gaus","gaus",-50,50);
  for(Int_t mod=0;mod<14;mod++){
    for(Int_t view=0;view<2;view++){
      for(Int_t pln=0;pln<11;pln++){
	sprintf(buff1,"(ltdc[5][0]-ltdc[%d][%d])*2.5>>h",pln,view);
	//sprintf(buff1,"(ltdc[5][1]-ltdc[%d][%d])>>h",pln,view);
	sprintf(buff2,"fMod==%0d",mod);
	//h     =  new TH1F("h","h",40,-50,50);
	h     =  new TH1F("h","h",41,-52.5,52.5);
	rtree -> Draw(buff1,buff2);
	h     -> Fit("gaus","q","",-40,40);
	sprintf(buff1,"TPL05-TPL%02d[nsec]",pln);
	sprintf(buff2,"Mod%02d View%d",mod,view);
	h     -> SetXTitle(buff1);
	h     -> SetYTitle("# of events");
	h     -> SetName  (buff2);
	h     -> SetTitle ("TDC timing");
	c1    -> Update();
	if(mod==0&&view==0&&pln==1)
	  c1  ->  Print("test.ps(");
	else    
	  c1    -> Print("test.ps");
	Double_t rms  = gaus->GetParameter(2);
	Double_t mean = gaus->GetParameter(1);
	if(pln==5&&view==0){rms=0;mean=0;}
	if(fabs(rms)>25||fabs(mean)>25){
	cout<<mod  <<"\t"
	    <<view <<"\t"
	    <<pln  <<"\t"
	    <<mean <<"\t"
	    <<rms  <<endl;
	cin.get();
	}
	wf  <<mod  <<"\t"
	    <<view <<"\t"
	    <<pln  <<"\t"
	    <<mean <<"\t"
	    <<rms  <<endl;

	h     -> Reset();
	delete h;
      }//pln
    }//view
  }//mod


  for(Int_t mod=0;mod<14;mod++){
    for(Int_t pln=1;pln<11;pln++){
      sprintf(buff1,"(ltdc[%d][1]-ltdc[%d][0])*2.5>>h",pln,pln);
      sprintf(buff2,"fMod==%0d",mod);
      h     =  new TH1F("h","h",41,-52.5,52.5);
      rtree -> Draw(buff1,buff2);
      h     -> Fit("gaus","q","",-40,40);
      sprintf(buff1,"TPL%02d",pln);
      sprintf(buff2,"Mod%02d",mod);
      h     -> SetXTitle(buff1);
      h     -> SetYTitle("# of events");
      h     -> SetName  (buff2);
      h     -> SetTitle ("TDC timing");
      c1    -> Update();
      if(mod==0&&pln==1)
	c1  ->  Print("test2.ps(");
      else    
	c1    -> Print("test2.ps");
      Double_t rms  = gaus->GetParameter(2);
      Double_t mean = gaus->GetParameter(1);
      if(fabs(rms)>25||fabs(mean)>25){
	cout<<mod  <<"\t"
	    <<pln  <<"\t"
	    <<mean <<"\t"
	    <<rms  <<endl;
	cin.get();
	}
      wf2 <<mod  <<"\t"
	  <<pln  <<"\t"
	  <<mean <<"\t"
	  <<rms  <<endl;
      //cin.get();
      h     -> Reset();
      delete h;
    }//pln
  }//mod

  c1->Clear();
  c1           ->Print("test.ps)");
  c1           ->Print("test2.ps)");
  //######## Write and Close ####################
  fHdifftdcx->Write();
  fHdifftdcy->Write();

  rtree   -> Write();
  rf      -> Write();
  rf      -> Close();
  f       -> Close();
  cout<<"---penetrate number = "<<nPenet<<"  ---------"<<endl; 
}
 
