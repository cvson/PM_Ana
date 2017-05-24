
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



int main(int argc,char *argv[]){
  TROOT root("GUI","GUI");
  //TApplication theApp("App",&argc,argv);
  TApplication theApp("App",0,0);
  root_setup froot_setup;
  char buff1[300],buff2[300];



  char FileName[300],root_file_pe_name[300];
  Int_t run_number;
  Int_t c=-1;

  Long_t tdc_cut=12;
  Double_t pe_cut=2.5;
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
      sprintf(FileName,"%s/beam_merged_tdccut.root",beam_folder,run_number);
      
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
  
  ana_beam fana_beam;
  if(!fana_beam.set_pedestal_and_gain(1644)){
    cout<<"error"<<endl;
    exit(1);
  }
  Int_t temp_count=0;
  // plot p1(1,1);
  for(Int_t nevent=0;nevent<Nevent;nevent++){
    IngEvt->GetEntry(nevent);
    for(Int_t numcyc=0;numcyc<NumCyc;numcyc++){
      fana_beam.set_pe(&pe[0][0][0][0],numcyc);
      fana_beam.set_Tdc(&Tdc[0][0][0][0],numcyc);
      Bool_t hit_event=false;

      Bool_t flag_plane_activity[NumTPL];
      Int_t NumActPlane=0;//count number of active scintillator plane and if number>3, print 
      for(Int_t numtpl=0;numtpl<NumTPL;numtpl++){
	flag_plane_activity[numtpl]=false;
	if(fana_beam.ana_plane_activity(numtpl,numcyc,pe_cut,tdc_cut)){
	  flag_plane_activity[numtpl]=true;
	  //if(numcyc==10)cout<<"event:"<<nevent<<" pl:"<<numtpl<<" cyc:"<<numcyc<<endl;
	  cout<<numcyc<<"\t"<<fana_beam.Tdc_x[0][numtpl][numcyc]-65536<<endl;
	  
	}
      }

      Int_t max_number_of_activity;
      bool debug;
      for(Int_t numtpl=0;numtpl<NumTPL;numtpl++){
	if(flag_plane_activity[numtpl]){
	  max_number_of_activity=1;
	  debug=false;
	  if(fana_beam.Pe_x[0][NumTPL][numtpl]>20)debug=true;
	  for(Int_t numtpl2=numtpl+1;numtpl2<NumTPL;numtpl2++){
	    if(flag_plane_activity[numtpl2]){
	      if(fabs(fana_beam.Tdc_x[0][numtpl][numcyc]-fana_beam.Tdc_x[0][numtpl2][numcyc])<tdc_cut){
		max_number_of_activity++;
		flag_plane_activity[numtpl]=false;
		flag_plane_activity[numtpl2]=false;
	      }//Tdc[numtpl]-Tdc[numtpl2]
	    }//flag_plane_activity[numtpl2]
	  }//numtpl2
	  //cout<<max_number_of_activity<<endl;
	  if(max_number_of_activity==1){
	    if(debug){
	      cout<<nevent<<"\t"<<numcyc<<endl;
	    }
	  }
	}//flag_plane_activity[numtpl]
      }//numtpl


      /*
      if(NumActPlane>=2&&numcyc==10){
	cout<<"-----"<<endl;
	cout<<"Event"<<EvtNum<<"("<<nevent<<")"<<"\tcyc."<<numcyc<<"\t";
	cout<<"# of active plane:"<<NumActPlane<<endl;
      }
      */
      /*
      if(numcyc==10){
	cout<<NumActPlane<<endl;
      }
      */


      Bool_t flag_top_veto_activity=false;
      if(fana_beam.top_veto_activity(numcyc,pe_cut)){
	flag_top_veto_activity=true;
      }
      Bool_t flag_bottom_veto_activity=false;
      if(fana_beam.bottom_veto_activity(numcyc,pe_cut)){
	flag_bottom_veto_activity=true;
      }
      Bool_t flag_left_veto_activity=false;
      if(fana_beam.left_veto_activity(numcyc,pe_cut)){
	flag_left_veto_activity=true;
      }
      //if(flag_plane_activity&&(!flag_top_veto_activity)&&(!flag_bottom_veto_activity)&&(!flag_left_veto_activity)){
      //if(flag_plane_activity&&TType==1){




    }//numcyc
    //if(nevent==98||nevent==138)p1.evdisp(&pe[0][0][0][0]);
  }//event


  f->Close();


}
