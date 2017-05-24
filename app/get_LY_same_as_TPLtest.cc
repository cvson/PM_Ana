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

#include"plot.cxx"

int main(int argc,char *argv[]){
  char buff1[200],buff2[200];
  TROOT root("GUI","GUI");
  TApplication theApp("App",&argc,argv);

  Int_t run_number_1,run_number_2;
  Int_t c=-1;
  char FileName[300];
  while ((c = getopt(argc, argv, "r:s:d")) != -1) {
    switch(c){
    case 'r':
      run_number_1=atoi(optarg);
      break;
    case 's':
      run_number_2=atoi(optarg);
      break;
    
    case 'd':
      cout<<"ooo"<<endl;
      break;
    }
  }

  hit h1;
  Double_t cut=6.5;
  sprintf(buff1,"%s/ingrid_%08d_00ly.daq.mid.pe.new.root",cosmic_LY_folder,run_number_1);
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

  Int_t Nevent = IngEvt->GetEntries();
  Int_t hit_channel_x[NumTFB],hit_channel_y[NumTFB];


  sprintf(buff1,"%s/ingrid_%08d_0000.daq.mid.pe.ly.root",cosmic_LY_folder,run_number_1);
  TFile *f_new = new TFile(buff1,"recreate");
  TH1F *ly[NumMod][NumTFB][NumCh];


  for(Int_t nummod=0;nummod<NumMod;nummod++){
    for(Int_t numtfb=0;numtfb<NumTFB;numtfb++){
      for(Int_t numch=0;numch<NumCh;numch++){
	sprintf(buff1,"M%02dT%02dC%02d",nummod+1,numtfb+1,numch+1);
	ly[nummod][numtfb][numch]= new TH1F(buff1,buff1,120,-10,120);
      }//numch
    }//numtfb
  }//nummod


  for(Int_t nevent=0;nevent<Nevent;nevent++){
    if((nevent+1)%1000==0)cout<<nevent+1<<"\t end"<<endl;
    IngEvt->GetEntry(nevent);
    for(Int_t numcyc=0;numcyc<NumCyc;numcyc++){
      for(Int_t nummod=0;nummod<NumMod;nummod++){
	for(Int_t numtfb=0;numtfb<NumTFB;numtfb++){
	  for(Int_t numch=0;numch<NumCh;numch++){
	    if(h1.three_hit_AND_side_layer(&pe[0][0][0][0],nummod,numtfb,numch,numcyc,cut)==1&&pe[nummod][numtfb][numch][numcyc]>cut){
	      ly[nummod][numtfb][numch]->Fill(pe[nummod][numtfb][numch][numcyc]);
	    }
	  }//numch
	}//numtfb
      }//nummod
    }//numcyc
  }//nevent

  //get each mean LY and entry
  TGraph *mean_LY[NumMod][NumTFB];
  Double_t mean[NumMod][NumTFB][NumCh];
  TGraph *cosmic_entry[NumMod][NumTFB];
  Double_t entry[NumMod][NumTFB][NumCh];
  Double_t ch[NumMod][NumTFB][NumCh];
  for(Int_t nummod=0;nummod<NumMod;nummod++){
    for(Int_t numtfb=0;numtfb<NumTFB;numtfb++){
      for(Int_t numch=0;numch<NumCh;numch++){
	mean[nummod][numtfb][numch]=ly[nummod][numtfb][numch]->GetMean();
	entry[nummod][numtfb][numch]=ly[nummod][numtfb][numch]->GetEntries();
	ch[nummod][numtfb][numch]=numch+1;
      }//numch
      mean_LY[nummod][numtfb]=new TGraph(NumCh,ch[nummod][numtfb],mean[nummod][numtfb]);
      cosmic_entry[nummod][numtfb]=new TGraph(NumCh,ch[nummod][numtfb],entry[nummod][numtfb]);
      sprintf(buff1,"LY_M%02dT%02d",nummod+1,numtfb+1);
      mean_LY[nummod][numtfb]->SetName(buff1);
      mean_LY[nummod][numtfb]->Write();
      sprintf(buff1,"Ent_M%02dT%02d",nummod+1,numtfb+1);
      cosmic_entry[nummod][numtfb]->SetName(buff1);
      cosmic_entry[nummod][numtfb]->Write();


    }//numtfb
  }//nummod


  //Save histgram
  for(Int_t nummod=0;nummod<NumMod;nummod++){
    for(Int_t numtfb=0;numtfb<NumTFB;numtfb++){
      for(Int_t numch=0;numch<NumCh;numch++){
	sprintf(buff1,"M%02dT%02dC%02d",nummod+1,numtfb+1,numch+1);
	ly[nummod][numtfb][numch]->SetName(buff1);
	ly[nummod][numtfb][numch]->Write();
      }//numch
    }//numtfb
  }//nummod

  f->Close();
  f_new->Write();
  f_new->Close();

}
