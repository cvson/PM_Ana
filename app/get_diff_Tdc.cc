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

  Int_t Nevent = IngEvt->GetEntries();
  cout<<"all event "<<Nevent<<endl;

  sprintf(buff1,"%s/ingrid_%08d_0000.daq.mid.diff_Tdc.new.root",diff_Tdc_folder,run_number_1);
  TFile *f_new = new TFile(buff1,"recreate");
  TH1F *diff_Tdc[NumMod][NumTFB][NumCh][NumCyc];
  for(Int_t nummod=0;nummod<NumMod;nummod++){
    for(Int_t numtfb=0;numtfb<NumTFB;numtfb++){
      for(Int_t numch=0;numch<NumCh;numch++){
	for(Int_t numcyc=0;numcyc<NumCyc;numcyc++){
	  sprintf(buff1,"N%02dT%02dC%02dY%02d",nummod+1,numtfb+1,numch+1,numcyc+1);
	  diff_Tdc[nummod][numtfb][numch][numcyc]= new TH1F(buff1,buff1,400,-500,500);
	}//numcyc
      }//numch
    }//numtfb
  }//nummod

  for(Int_t nevent=0;nevent<Nevent;nevent++){
    if((nevent+1)%1000==0)cout<<nevent+1<<"\tend"<<endl;
    IngEvt->GetEntry(nevent);
    for(Int_t numcyc=0;numcyc<NumCyc;numcyc++){
      for(Int_t nummod_1=0;nummod_1<NumMod;nummod_1++){
	for(Int_t numtfb_1=0;numtfb_1<NumTFB;numtfb_1++){
	  for(Int_t numch_1=0;numch_1<NumCh;numch_1++){
	    Int_t flag_1=0;
	    flag_1 = h1.exist_Tdc(&Tdc[0][0][0][0],nummod_1,numtfb_1,numch_1,numcyc);
	    if(flag_1==1){
      	      for(Int_t nummod_2=0;nummod_2<NumMod;nummod_2++){
		for(Int_t numtfb_2=0;numtfb_2<NumTFB;numtfb_2++){
		  for(Int_t numch_2=0;numch_2<NumCh;numch_2++){
		    Int_t flag_2=0;
		    flag_2 = h1.exist_Tdc(&Tdc[0][0][0][0],nummod_2,numtfb_2,numch_2,numcyc);

		    if(flag_2==1){
		      Long_t diff = Tdc[nummod_1][numtfb_1][numch_1][numcyc]-Tdc[nummod_2][numtfb_2][numch_2][numcyc];
		      diff_Tdc[nummod_1][numtfb_1][numch_1][numcyc]->Fill(diff);

		    }//flag_2==1
		  }//numch_2
		}//numtfb_2
	      }//nummod_2
	    }//flag_1==1
	  }//numch_1
	}//numtfb_1
      }//nummod_1
    }//numcyc
  }//nevent

  f->Close();
  for(Int_t nummod=0;nummod<NumMod;nummod++){
    for(Int_t numtfb=0;numtfb<NumTFB;numtfb++){
      for(Int_t numch=0;numch<NumCh;numch++){
	for(Int_t numcyc=0;numcyc<NumCyc;numcyc++){
	  diff_Tdc[nummod][numtfb][numch][numcyc]->Write();
	}//numcyc
      }//numch
    }//numtfb
  }//nummod


  f_new->Write();
  f_new->Close();

}
