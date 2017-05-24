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
#include <TSpectrum.h>
#include <TF1.h>
#include <TMath.h>
#include <TSpectrum.h>
#include <TString.h>
#include <TSystem.h>
#include <TPostScript.h>

#include "TApplication.h"

#include"setup.hxx"
#include"root_setup.hxx"
#include"ana_MPPC.cxx"



int main(int argc,char *argv[]){
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  root_setup froot_setup;
  char buff1[300],buff2[300];



  char FileName[300],root_file_pe_name[300];
  Int_t run_number;
  Int_t c=-1;
  Double_t without_sigma=3.5;
  while ((c = getopt(argc, argv, "r:h")) != -1) {
    switch(c){
    case 'r':
      run_number=atoi(optarg);
      sprintf(FileName,"%s/ingrid_%08d_0000.daq.mid.new.root",root_file_folder,run_number);
      cout<<"file name is :"<<FileName<<endl;
      break;
    case 'h':
      cout<<"r:[runnumber]"<<endl;
      break;
    }
  }

  FileStat_t fs;
  cout<<FileName<<endl;
  if(gSystem->GetPathInfo(FileName,fs)){
    cout<<"Cannot open file "<<FileName<<endl;
    exit(1);
  }

  TFile *f = new TFile(FileName,"read");
  TTree *IngEvt = (TTree*)f->Get("IngEvt");
  Int_t Adc[NumMod][NumTFB][NumCh][NumCyc];
  sprintf(buff1,"Adc[%d][%d][%d][%d]",NumMod,NumTFB,NumCh,NumCyc);
  IngEvt->SetBranchAddress(buff1,Adc);
  Int_t Nevent = IngEvt->GetEntries();
  Double_t gain[NumMod][NumTFB][NumCh];
  Double_t noise[NumMod][NumTFB][NumCh];
  Double_t pedestal[NumMod][NumTFB][NumCh];
  Double_t pedestal_sigma[NumMod][NumTFB][NumCh];
  Double_t temp;
  
  //Fill histgram
  TH1F *mppc[NumMod][NumTFB][NumCh];
  for(Int_t nummod=0;nummod<NumMod;nummod++){
    for(Int_t numtfb=0;numtfb<NumTFB;numtfb++){
      for(Int_t numch=0;numch<NumCh;numch++){
	sprintf(buff1,"Mod%02dTPL%02dCh%02d",nummod,numtfb,numch);
	mppc[nummod][numtfb][numch]=new TH1F(buff1,buff1,HIST_MAX-HIST_MIN+1,HIST_MIN,HIST_MAX);
      }
    }
  }

  cout<<"Fill histgram"<<endl;
  for(Int_t nevent=0;nevent<Nevent;nevent++){

    if((nevent+1)%100==0)cout<<nevent+1<<"\t end"<<endl;
    IngEvt->GetEntry(nevent);
    for(Int_t nummod=0;nummod<NumMod;nummod++){
      for(Int_t numtfb=0;numtfb<NumTFB;numtfb++){
	for(Int_t numch=0;numch<NumCh;numch++){
	  for(Int_t numcyc=0;numcyc<NumCyc;numcyc++){
	    mppc[nummod][numtfb][numch]->Fill(Adc[nummod][numtfb][numch][numcyc]);
	  }//numcyc
	}//numch
      }//numtfb
    }//nummod
  }//event


  //analysis and culculate gain
  cout<<"analysis and culculate gain"<<endl;
  TCanvas *c1 = new TCanvas("c1","c1",10,10,500,500);
  for(Int_t nummod=0;nummod<14;nummod++){
    for(Int_t numtfb=0;numtfb<11;numtfb++){
      for(Int_t numch=0;numch<48;numch++){
	ana_MPPC ana_mppc;
	ana_mppc.set_without_sigma(without_sigma);
	if(numtfb<11&&numch<48){
	  ana_mppc.analysis_old_version(mppc[nummod][numtfb][numch]);
	 
	}




	gain[nummod][numtfb][numch]=ana_mppc.get_gain();

	if( fabs ( gain[nummod][numtfb][numch] - 11 ) > 6 ){
	  cout<<"Mod :"   << nummod
	      <<"TFB :"   << numtfb
	      <<"Ch :"    << numch
	      <<"Mod :"   << 	gain[nummod][numtfb][numch]
	      <<endl;
	  mppc[nummod][numtfb][numch] -> Draw();
	  c1->Update();
	  cin.get();
	}
	/*
	pedestal[nummod][numtfb][numch]=ana_mppc.get_pedestal();
	pedestal_sigma[nummod][numtfb][numch]=ana_mppc.get_pedestal_sigma();
	noise[nummod][numtfb][numch]=ana_mppc.get_noise(Nevent_for_analysis*NumCyc);
	*/
      }//numch
    }//numtfb
  }//nummod

} 
