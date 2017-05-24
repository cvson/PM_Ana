#include<iostream>
#include<sstream>
#include<fstream>
#include<unistd.h>
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


#include"setup.hxx"
#include"root_setup.hxx"
#include"ana_MPPC.cxx"
#include"ana_beam.cxx"
#include"plot.cxx"

static const double threshold = 2.5;
int main(int argc,char *argv[]){
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  root_setup froot_setup;
  char buff1[300],buff2[300];



  char FileName[300],root_file_pe_name[300];
  Int_t run_number;
  Int_t c=-1;

  Long_t tdc_cut=12;
  Double_t pe_cut=6.5;
  Bool_t normal_run=false;
  Bool_t beam_run=false;
  while ((c = getopt(argc, argv, "b:hr:p:t:s")) != -1) {
    switch(c){
    case 'r':
      normal_run=true;
      run_number=atoi(optarg);
      
      sprintf(FileName,"%s/ingrid_%08d_0000.daq.mid.pe.new.root",root_file_pe_folder,run_number);
      break;
      
    case 'p':

      pe_cut=atof(optarg);
      break;

    case 't':
      tdc_cut=atoi(optarg);
      break;

    case 's':
      //sprintf(FileName,"%s/beam_merged_tdccut_divide.root",beam_folder,run_number);
      //sprintf(FileName,"%s/beam_merged_tdccut.root",beam_folder,run_number);
      sprintf(FileName,"%s/beam_merged.root",beam_folder,run_number);
      
      break;

    case 'b':
      run_number=atoi(optarg);
      sprintf(FileName,"%s/ingrid_%08d_0000.beam.root",beam_folder,run_number);
      beam_run=true;
      break;

    case 'h':
      cout<<"r"<<"\t"<<"normal run"<<endl;
      cout<<"b"<<"\t"<<"beam data"<<endl;
      cout<<"p"<<"\t"<<"pe cut-0.5"<<endl;
      cout<<"t"<<"\t"<<"tdc cut"<<endl;
      cout<<"opt h"<<endl;
      exit(1);

    }
  }


  FileStat_t fs;
  cout<<beam_folder<<endl;
  cout<<FileName<<endl;
  if(gSystem->GetPathInfo(FileName,fs)){
    cerr << __LINE__ << endl;
    cout<<"Cannot open file "<<FileName<<endl;
    exit(1);
  }

  TFile *f = new TFile(FileName,"read");
  TTree *IngEvt = (TTree*)f->Get("IngEvt");
  Int_t Adc[NumMod][NumTFB][NumCh][NumCyc];
  Double_t pe[NumMod][NumTFB][NumCh][NumCyc];
  Int_t LoAdc[NumMod][NumTFB][NumCh][NumCyc];
  Long_t Tdc[NumMod][NumTFB][NumCh][NumCyc];
  Long_t Time[NumMod][NumTFB][NumCh][NumCyc];
  Int_t EvtNum;
  Long64_t UTime;
  Int_t TType;
  //if(normal_run){
  //sprintf(buff1,"Adc[%d][%d][%d][%d]",NumMod,NumTFB,NumCh,NumCyc);
  //IngEvt->SetBranchAddress(buff1,Adc);
  //}
  //if(beam_run){
  sprintf(buff1,"pe[%d][%d][%d][%d]",NumMod,NumTFB,NumCh,NumCyc);
  IngEvt->SetBranchAddress(buff1,pe);
  //}
  sprintf(buff1,"LoAdc[%d][%d][%d][%d]",NumMod,NumTFB,NumCh,NumCyc);
  //IngEvt->SetBranchAddress(buff1,LoAdc);
  sprintf(buff1,"Tdc[%d][%d][%d][%d]",NumMod,NumTFB,NumCh,NumCyc);
  IngEvt->SetBranchAddress(buff1,Tdc);

  sprintf(buff1,"EvtNum");
  IngEvt->SetBranchAddress(buff1,&EvtNum);
 
  sprintf(buff1,"TType");
  IngEvt->SetBranchAddress(buff1,&TType);

  sprintf(buff1,"UTime");
  IngEvt->SetBranchAddress(buff1,&UTime);

  Int_t Nevent = IngEvt->GetEntries();
  cout<<"all event"<<Nevent<<endl;

  Double_t gain[NumMod][NumTFB][NumCh];
  Double_t noise[NumMod][NumTFB][NumCh];
  Double_t pedestal[NumMod][NumTFB][NumCh];
  Double_t pedestal_sigma[NumMod][NumTFB][NumCh];
  Double_t temp;
  


  Int_t number_of_hit[23];
  for(Int_t numcyc=0;numcyc<NumCyc;numcyc++){
    number_of_hit[numcyc]=0;
  }

  for(Int_t nevent=0;nevent<Nevent;nevent++){
    IngEvt->GetEntry(nevent);
    //initialize number of hit of each cycle
    for(Int_t numcyc=0;numcyc<NumCyc;numcyc++){
      for(Int_t numtfb=0;numtfb<UseNumTFB;numtfb++){
	for(Int_t numch=0;numch<UseNumCh;numch++){
	  if(pe[0][numtfb][numch][numcyc]>threshold){
	    number_of_hit[numcyc]++;
	  }//pe>threshold
	}//numch
      }//numtfb
    }//numcyc
  }//event

  for(Int_t numcyc=0;numcyc<NumCyc;numcyc++){
    cout<<number_of_hit[numcyc]<<endl;
  }

  f->Close();


}
