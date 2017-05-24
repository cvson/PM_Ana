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
#include"hit.cxx"


int main(int argc,char *argv[]){
  char buff1[200],buff2[200];
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);

  //read run number
  Int_t run_number_1;
  Int_t c=-1;
  char FileName[300];
  while ((c = getopt(argc, argv, "r:h")) != -1) {
    switch(c){
    case 'r':
      run_number_1=atoi(optarg);
      break;
    case 'h':
      cout<<"help menu"<<endl;
      cout<<"-r [run_number]"<<endl;
      break;
    }
  }

  hit h1;
  Double_t cut=7.5;//cut threshold
  sprintf(buff1,"%s/ingrid_%08d_0000.daq.mid.pe.new.root",root_file_pe_folder,run_number_1);
  FileStat_t fs;
  if(gSystem->GetPathInfo(buff1,fs)){
    cout<<"Cannot open file "<<buff1<<endl;
    exit(1);
  }

  TFile *f = new TFile(buff1,"read");
  TTree *IngEvt = (TTree*)f->Get("IngEvt");
  Double_t pe[NumMod][NumTFB][NumCh][NumCyc];
  sprintf(buff1,"pe[%d][%d][%d][%d]",NumMod,NumTFB,NumCh,NumCyc);
  IngEvt->SetBranchAddress(buff1,pe);
  Long_t Tdc[NumMod][NumTFB][NumCh][NumCyc];
  sprintf(buff1,"Tdc[%d][%d][%d][%d]",NumMod,NumTFB,NumCh,NumCyc);
  IngEvt->SetBranchAddress(buff1,Tdc);
  Long_t Time[NumMod][NumTFB][NumCh][NumCyc];
  sprintf(buff1,"Time[%d][%d][%d][%d]",NumMod,NumTFB,NumCh,NumCyc);
  IngEvt->SetBranchAddress(buff1,Time);

  Int_t Nevent = IngEvt->GetEntries();
  Int_t hit_channel_x[NumTFB],hit_channel_y[NumTFB];

  //create new root file 
  sprintf(buff1,"%s/ingrid_%08d_0000.daq.mid.pe.new.root",penetrate_cosmic_folder,run_number_1);
  TFile *f_new = new TFile(buff1,"recreate");
  Int_t hit_event;
  Int_t hit_cyc;
  TTree *Cosmic_x = new TTree("Cosmic_x","Cosmic_x");
  sprintf(buff1,"pe[%d][%d][%d][%d]",NumMod,NumTFB,NumCh,NumCyc);
  sprintf(buff2,"pe[%d][%d][%d][%d]/D",NumMod,NumTFB,NumCh,NumCyc);
  Cosmic_x->Branch(buff1,pe,buff2);
  sprintf(buff1,"Tdc[%d][%d][%d][%d]",NumMod,NumTFB,NumCh,NumCyc);
  sprintf(buff2,"Tdc[%d][%d][%d][%d]/L",NumMod,NumTFB,NumCh,NumCyc);
  Cosmic_x->Branch(buff1,Tdc,buff2);
  sprintf(buff1,"Time[%d][%d][%d][%d]",NumMod,NumTFB,NumCh,NumCyc);
  sprintf(buff2,"Time[%d][%d][%d][%d]/L",NumMod,NumTFB,NumCh,NumCyc);
  Cosmic_x->Branch(buff1,Time,buff2);
  Cosmic_x->Branch("hit_event",&hit_event,"hit_event/I");
  Cosmic_x->Branch("hit_cyc",&hit_cyc,"hit_cyc/I");
  sprintf(buff1,"hit_channel_x[%d]",NumTFB);
  sprintf(buff2,"hit_channel_x[%d]/I",NumTFB);
  Cosmic_x->Branch(buff1,hit_channel_x,buff2);
  TTree *Cosmic_y = new TTree("Cosmic_y","Cosmic_y");
  sprintf(buff1,"pe[%d][%d][%d][%d]",NumMod,NumTFB,NumCh,NumCyc);
  sprintf(buff2,"pe[%d][%d][%d][%d]/D",NumMod,NumTFB,NumCh,NumCyc);
  Cosmic_y->Branch(buff1,pe,buff2);
  sprintf(buff1,"Tdc[%d][%d][%d][%d]",NumMod,NumTFB,NumCh,NumCyc);
  sprintf(buff2,"Tdc[%d][%d][%d][%d]/L",NumMod,NumTFB,NumCh,NumCyc);
  Cosmic_y->Branch(buff1,Tdc,buff2);
  sprintf(buff1,"Time[%d][%d][%d][%d]",NumMod,NumTFB,NumCh,NumCyc);
  sprintf(buff2,"Time[%d][%d][%d][%d]/L",NumMod,NumTFB,NumCh,NumCyc);
  Cosmic_y->Branch(buff1,Time,buff2);
  Cosmic_y->Branch("hit_event",&hit_event,"hit_event/I");
  Cosmic_y->Branch("hit_cyc",&hit_cyc,"hit_cyc/I");
  sprintf(buff1,"hit_channel_y[%d]",NumTFB);
  sprintf(buff2,"hit_channel_y[%d]/I",NumTFB);
  Cosmic_y->Branch(buff1,hit_channel_y,buff2);

  TTree *Cosmic = new TTree("Cosmic","Cosmic");
  sprintf(buff1,"pe[%d][%d][%d][%d]",NumMod,NumTFB,NumCh,NumCyc);
  sprintf(buff2,"pe[%d][%d][%d][%d]/D",NumMod,NumTFB,NumCh,NumCyc);
  Cosmic->Branch(buff1,pe,buff2);
  sprintf(buff1,"Tdc[%d][%d][%d][%d]",NumMod,NumTFB,NumCh,NumCyc);
  sprintf(buff2,"Tdc[%d][%d][%d][%d]/L",NumMod,NumTFB,NumCh,NumCyc);
  Cosmic->Branch(buff1,Tdc,buff2);
  sprintf(buff1,"Time[%d][%d][%d][%d]",NumMod,NumTFB,NumCh,NumCyc);
  sprintf(buff2,"Time[%d][%d][%d][%d]/L",NumMod,NumTFB,NumCh,NumCyc);
  Cosmic->Branch(buff1,Time,buff2);
  Cosmic->Branch("hit_event",&hit_event,"hit_event/I");
  Cosmic->Branch("hit_cyc",&hit_cyc,"hit_cyc/I");
  sprintf(buff1,"hit_channel_x[%d]",NumTFB);
  sprintf(buff2,"hit_channel_x[%d]/I",NumTFB);
  Cosmic->Branch(buff1,hit_channel_x,buff2);
  sprintf(buff1,"hit_channel_y[%d]",NumTFB);
  sprintf(buff2,"hit_channel_y[%d]/I",NumTFB);
  Cosmic->Branch(buff1,hit_channel_y,buff2);
 
 
  //analysis
  for(Int_t nevent=0;nevent<Nevent;nevent++){
    IngEvt->GetEntry(nevent);
    for(Int_t numcyc=0;numcyc<NumCyc;numcyc++){
      //all hit x layer
      Int_t flag_x=0;
      flag_x = h1.one_hit_every_one_layer_x(&pe[0][0][0][0],numcyc,cut,&hit_channel_x[0]);
      if(flag_x==1){
	hit_event=nevent;
	hit_cyc=numcyc;
	Cosmic_x->Fill();
      }

      //all hit y layer
      Int_t flag_y = h1.one_hit_every_one_layer_y(&pe[0][0][0][0],numcyc,cut,&hit_channel_y[0]);
      if(flag_y==1){
	hit_event=nevent;
	hit_cyc=numcyc;
	Cosmic_y->Fill();
      }

      //all hit layer
      if(flag_x&&flag_y){
	hit_event=nevent;
	hit_cyc=numcyc;
	Cosmic->Fill();
      }
      /*
      if(flag_x==1){
	for(Int_t numtfb=0;numtfb<UseNumTFB-1;numtfb++){
	  Long_t diff=Tdc[0][0][hit_channel_x[0]][numcyc]-Tdc[0][numtfb][hit_channel_x[numtfb]][numcyc];
	  if(abs(diff)>20){
	    if(Tdc[0][0][hit_channel_x[0]][numcyc]<10000000&&Tdc[0][numtfb][hit_channel_x[numtfb]][numcyc]<10000000){
	      cout<<"x "<<"event "<<nevent<<"\tcyc."<<numcyc<<"\tch. 1_"<<hit_channel_x[0]+1<<" \tch."<<numtfb+1<<"_"<<hit_channel_x[numtfb]+1<<"\t  diff"<<diff<<endl;
	      // p1.event_display(&pe[0][0][0][0],numcyc,cut);
	    }
	  }
	}
      }
      if(flag_y==1){
	for(Int_t numtfb=0;numtfb<UseNumTFB-1;numtfb++){
	  Long_t diff=Tdc[0][0][hit_channel_y[0]][numcyc]-Tdc[0][numtfb][hit_channel_y[numtfb]][numcyc];
	  if(abs(diff)>20){

	    if(Tdc[0][0][hit_channel_y[0]][numcyc]<10000000&&Tdc[0][numtfb][hit_channel_y[numtfb]][numcyc]<10000000){
	      cout<<"y "<<"event "<<nevent<<"\tcyc."<<numcyc<<"\tch. 1_"<<hit_channel_y[0]+1<<" \tch."<<numtfb+1<<"_"<<hit_channel_y[numtfb]+1<<"\t  diff"<<diff<<endl;
	      //p1.event_display(&pe[0][0][0][0],numcyc,cut);
	    }
	  }
	}
      }
      */
    }//numcyc
  }//nevent

  f->Close();
  Cosmic_x->Write();
  Cosmic_y->Write();
  f_new->Write();
  f_new->Close();

}
