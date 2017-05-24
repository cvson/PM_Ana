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
  Int_t draw_flag=0,gain_and_pedestal_file_flag=0,noise_file_flag=0;
  Int_t zero_sup_flag=0,using_pedestal_and_gain_file,wait=0;
  Bool_t pedestal_shift_check=false;
  Bool_t convert_flag=false;
  Double_t without_sigma=3.5;
  while ((c = getopt(argc, argv, "r:")) != -1) {
    switch(c){
    case 'r':
      run_number=atoi(optarg);

      sprintf(root_file_pe_name,"%s/ingrid_%08d_cosmic.root",root_file_folder,run_number);
      //sprintf(root_file_pe_name,"%s/ingrid_%08d_0000.daq.mid.pe.new.root",root_file_pe_folder,run_number);
      cout<<"out put file name is :"<<root_file_pe_name<<endl;
      break;
    }
  }

  FileStat_t fs;
  if(gSystem->GetPathInfo(root_file_pe_name,fs)){
    cout<<"Cannot open file "<<root_file_pe_name<<endl;
    exit(1);
  }

  TFile *f = new TFile(root_file_pe_name,"read");
  TTree *IngEvt = (TTree*)f->Get("IngEvt");
  Double_t pe[NumMod][NumTFB][NumCh][NumCyc];
  sprintf(buff1,"pe[%d][%d][%d][%d]",NumMod,NumTFB,NumCh,NumCyc);
  IngEvt->SetBranchAddress(buff1,pe);
  Int_t Nevent = IngEvt->GetEntries();
  
  TFile *f_new = new TFile("test.root","recreate");
  TTree *IngEvt_new = new TTree("IngEvt","IngEvt");
  Double_t pe_all[NumCyc];
  sprintf(buff1,"pe_all[%d]",NumCyc);
  sprintf(buff2,"pe_all[%d]/D",NumCyc);
  IngEvt_new->Branch(buff1,&pe_all,buff2);
  Int_t hit[NumCyc];
  sprintf(buff1,"hit[%d]",NumCyc);
  sprintf(buff2,"hit[%d]/I",NumCyc);
  IngEvt_new->Branch(buff1,&hit,buff2);

  Int_t count=0;
  static const double threshold=9.5;
  for(Int_t nevent=0;nevent<Nevent;nevent++){
    if((nevent+1)%100==0)cout<<nevent+1<<"\t end"<<endl;
    IngEvt->GetEntry(nevent);
    for(Int_t numcyc=0;numcyc<NumCyc;numcyc++){
      pe_all[numcyc]=0;
      hit[numcyc]=0;
    }
    for(Int_t numcyc=0;numcyc<NumCyc;numcyc++){
      for(Int_t nummod=0;nummod<NumMod;nummod++){
	for(Int_t numtfb=0;numtfb<UseNumTFB;numtfb++){
	  bool flag=false;
	  for(Int_t numch=0;numch<24;numch++){
	    if(pe[nummod][numtfb][numch][numcyc]>threshold){
	      for(Int_t numch2=24;numch2<UseNumCh;numch2++){
		if(pe[nummod][numtfb][numch][numcyc]>threshold){
		  flag=true;

		  pe_all[numcyc]=pe_all[numcyc]+pe[nummod][numtfb][numch][numcyc];
		}
	      }	    
	    }
	  
	  }//numch
	  if(flag)hit[numcyc]++;
	}//numtfb
      }//nummod
    }//numcyc
    if(hit[0]>2){
      cout<<nevent<<endl;
    }
    //if(hit[18]<2&&hit[19]<2){
    if(hit[20]<2){

    //if(pe_all[18]<100&&pe_all[19]<100){
      //cout<<nevent<<endl;
      count++;
    }

    IngEvt_new->Fill();
  }//event
  cout<<count<<" purity"<<endl;
  IngEvt_new->Write();
  f_new->Write();
  f_new->Close();
}
