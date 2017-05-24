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
#include"ana_MPPC_Mass.cxx"



int main(int argc,char *argv[]){
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  root_setup froot_setup;
  char buff1[300],buff2[300];


  char FilePath[300], FileName[300];
  Int_t run_number;
  Int_t c=-1;
  Bool_t draw_flag=false;
  while ((c = getopt(argc, argv, "r:dh")) != -1) {
    switch(c){
    case 'r':
      run_number=atoi(optarg);
      sprintf(FilePath,"/home/daq/MPPC_Mass_Test/Run%03d_1",run_number);
      cout<<"input file name is :"<<FilePath<<endl;
      break;
    case 'd':
      draw_flag=true;
      break;
    case 'h':
      cout<<"r:[runnumber]"<<endl;
      break;
    }
  }
  TH1F*     fHadc    [62];
  for(Int_t gch=0; gch<62; gch++){
    sprintf(buff1,"ADC_Ch%02d",gch);
    fHadc[gch] = new TH1F(buff1,buff1,HIST_MAX-HIST_MIN,HIST_MIN,HIST_MAX);
  }

  TFile*    fTFile;
  TTree*    fTree;
  Float_t   adc             [128];
  Float_t   gain_wo_led     [62][3][20]; //[ch][temperature][voltage]
  Float_t   gain_w_led      [62][3][20]; //[ch][temperature][voltage]
  Float_t   pedestal_wo_led [62][3][20];
  Float_t   pedestal_w_led  [62][3][20];
  Float_t   onepe_wo_led    [62][3][20];
  Float_t   onepe_w_led     [62][3][20];
  Float_t   noise           [62][3][20];
  Float_t   canda           [62][3][20];
  ana_MPPC* fana_MPPC = new ana_MPPC();
  ofstream  resultana;

  //############ analysis loop ######################################
  //#################################################################
  for(Int_t itemp=0; itemp<3; itemp++){
    Int_t temp = 15 + itemp * 5;
    for(Int_t vol=0; vol<20; vol++){

      //############ analysis darknoise data ########################
      //#############################################################
      //############ check file exist or not ########################
      //#############################################################
      sprintf(FileName,"%s/noise%02d_%d.root",FilePath,temp, vol);
      FileStat_t fs;
      cout<<FileName<<endl;
      if(gSystem->GetPathInfo(FileName,fs)){
	cout<<"Cannot open file "<<FileName<<endl;
	exit(1);
      }
      cout<<"read...  "<<FileName<<endl;

      //############ check file exist or not ########################
      //#############################################################
      fTFile     =   new TFile(FileName,"read");
      fTree      =  (TTree*)fTFile->Get("h11");
      fTree      -> SetBranchAddress("adc", adc);
      Int_t nEvt =  fTree->GetEntries();
      cout<<"all event ="<<nEvt<<endl;

      //############ Fill histgram  #################################
      //#############################################################
      for(Int_t n=0; n<nEvt; n++){
	fTree    ->GetEntry(n);
	Int_t gch=0;
	for(Int_t ch=1 ; ch<32; ch++){
	  fHadc[gch] -> Fill(adc[ch]);
	  gch++;
	}
	for(Int_t ch=65; ch<96; ch++){
	  fHadc[gch] -> Fill(adc[ch]);
	  gch++;
	}
      }
      fTFile->Close();
      //fTree-> Reset();

      //############ analysis #######################################
      //#############################################################
      for(Int_t gch=0 ; gch<62; gch++){
	fana_MPPC                ->  analysis_old_version(fHadc[gch]);
	noise   [gch][itemp][vol]  =   fana_MPPC->get_noise(nEvt);
	gain_wo_led      [gch][itemp][vol]  =   fana_MPPC->get_gain();
	pedestal_wo_led  [gch][itemp][vol]  =   fana_MPPC->get_num_pedestal();
	onepe_wo_led     [gch][itemp][vol]  =   fana_MPPC->get_num_onepe();



      }
 

      //############ Reset   ########################################
      //#############################################################
      for(Int_t gch=0; gch<62; gch++){
	fHadc[gch]->Reset();
      }

      //############ analysis light injection data ##################
      //#############################################################
      //############ check file exist or not ########################
      //#############################################################
      sprintf(FileName,"%s/light%02d_%d.root",FilePath,temp, vol);
      cout<<FileName<<endl;
      if(gSystem->GetPathInfo(FileName,fs)){
	cout<<"Cannot open file "<<FileName<<endl;
	exit(1);
      }
      cout<<"read...  "<<FileName<<endl;

      //############ check file exist or not ########################
      //#############################################################
      fTFile     =   new TFile(FileName,"read");
      fTree      =  (TTree*)fTFile->Get("h11");
      fTree      -> SetBranchAddress("adc", adc);
      nEvt =  fTree->GetEntries();
      cout<<"all event ="<<nEvt<<endl;

      //############ Fill histgram  #################################
      //#############################################################
      for(Int_t n=0; n<nEvt; n++){
	fTree    ->GetEntry(n);
	Int_t gch=0;
	for(Int_t ch=1 ; ch<32; ch++){
	  fHadc[gch] -> Fill(adc[ch]);
	  gch++;
	}
	for(Int_t ch=65; ch<96; ch++){
	  fHadc[gch] -> Fill(adc[ch]);
	  gch++;
	}
      }
      fTFile->Close();

      //############ analysis #######################################
      //#############################################################
      for(Int_t gch=0 ; gch<62; gch++){
	fana_MPPC                ->  analysis_old_version(fHadc[gch]);
	canda           [gch][itemp][vol]  =   fana_MPPC->get_crosstalk_and_afterpulse(nEvt);
	gain_w_led      [gch][itemp][vol]  =   fana_MPPC->get_gain();
	pedestal_w_led  [gch][itemp][vol]  =   fana_MPPC->get_num_pedestal();
	onepe_w_led     [gch][itemp][vol]  =   fana_MPPC->get_num_onepe();
	//############ drawing  #######################################
	//#############################################################
	if(draw_flag){
	//if(gch==0){
	  TCanvas *c2 = new TCanvas("c2","c2",10,10,500,500);
	  c2         -> Clear();
	  fHadc[gch] -> Draw();
	  c2->Update();

	  cout<<"draw histogram"<<endl;
	  cout<<noise[gch][itemp][vol]<<endl;
	  cout<<canda[gch][itemp][vol]<<endl;
	  cout<<fana_MPPC->get_num_pedestal()<<endl;
	  cout<<fana_MPPC->get_num_onepe()<<endl;
	  cin.get();
	}//if draw

      }



      //############ Reset   ########################################
      //#############################################################
      for(Int_t gch=0; gch<62; gch++){
	fHadc[gch]->Reset();
      }


    }//voltage loop
  }//temperature loop

  //############ writing   #######################################
  //#############################################################
  sprintf(buff1,"/home/daq/MPPC_Mass_Test/anaRun%03d_1/result_analysis_0.txt",run_number);
  ofstream  resultana1(buff1);
  for(Int_t gch=0; gch<32; gch++){
    for(Int_t itemp=0; itemp<3; itemp++){
      for(Int_t vol=0; vol<20; vol++){
	resultana1<<gch     <<"\t"
		  <<itemp   <<"\t"
		  <<vol     <<"\t"
		  <<gain_w_led[gch][itemp][vol]*36447   <<"\t"
		  <<pedestal_w_led[gch][itemp][vol]      <<"\t"
		  <<onepe_w_led[gch][itemp][vol]         <<"\t"
		  <<pedestal_wo_led[gch][itemp][vol]     <<"\t"
		  <<onepe_wo_led[gch][itemp][vol]        <<"\t"
		  <<"0"		  <<endl;

		
      }//vol
    }//itemp
  }//gch
  resultana1.close();
  sprintf(buff1,"/home/daq/MPPC_Mass_Test/anaRun%03d_1/result_analysis_1.txt",run_number);
  ofstream  resultana2(buff1);
  for(Int_t gch=32; gch<64; gch++){
    for(Int_t itemp=0; itemp<3; itemp++){
      for(Int_t vol=0; vol<20; vol++){
	resultana2<<gch     <<"\t"
		  <<itemp   <<"\t"
		  <<vol     <<"\t"
		  <<gain_w_led[gch][itemp][vol]*36447   <<"\t"
		  <<pedestal_w_led[gch][itemp][vol]      <<"\t"
		  <<onepe_w_led[gch][itemp][vol]         <<"\t"
		  <<pedestal_wo_led[gch][itemp][vol]     <<"\t"
		  <<onepe_wo_led[gch][itemp][vol]        <<"\t"
		  <<"0"		  <<endl;
      }//vol
    }//itemp
  }//gch
  resultana2.close();

  sprintf(buff1,"/home/daq/MPPC_Mass_Test/anaRun%03d_1/result_calc_0.txt",run_number);
  ofstream  resultcalc1(buff1);
  for(Int_t gch=0; gch<32; gch++){
    for(Int_t itemp=0; itemp<3; itemp++){
      for(Int_t vol=0; vol<20; vol++){
	resultcalc1<<gch     <<"\t"
		  <<itemp   <<"\t"
		  <<vol     <<"\t"
		  <<gain_wo_led[gch][itemp][vol]*36447  <<"\t"
		  <<noise      [gch][itemp][vol]        <<"\t"
		  <<canda      [gch][itemp][vol]        <<"\t"
		  <<"0"		  <<"\t"  //PDE
		  <<"0"		  <<"\t"  //Vbr
		  <<"0"		  <<"\t"  //Capacitance
		  <<"0"		  <<"\t"  //Voperation
		  <<endl;
      }//vol
    }//itemp
  }//gch
  resultcalc1.close();

  sprintf(buff1,"/home/daq/MPPC_Mass_Test/anaRun%03d_1/result_calc_1.txt",run_number);
  ofstream  resultcalc2(buff1);
  for(Int_t gch=32; gch<64; gch++){
    for(Int_t itemp=0; itemp<3; itemp++){
      for(Int_t vol=0; vol<20; vol++){
	resultcalc2<<gch     <<"\t"
		  <<itemp   <<"\t"
		  <<vol     <<"\t"
		  <<gain_wo_led[gch][itemp][vol]*36447  <<"\t"
		  <<noise      [gch][itemp][vol]        <<"\t"
		  <<canda      [gch][itemp][vol]        <<"\t"
		  <<"0"		  <<"\t"  //PDE
		  <<"0"		  <<"\t"  //Vbr
		  <<"0"		  <<"\t"  //Capacitance
		  <<"0"		  <<"\t"  //Voperation
		  <<endl;
      }//vol
    }//itemp
  }//gch
  resultcalc2.close();

}
