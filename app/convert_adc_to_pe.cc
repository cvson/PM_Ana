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
  while ((c = getopt(argc, argv, "r:dtn:z:cphw:")) != -1) {
    switch(c){
    case 'r':
      run_number=atoi(optarg);
      sprintf(FileName,"%s/ingrid_%08d_0000.daq.mid.new.root",root_file_folder,run_number);
      cout<<"file name is :"<<FileName<<endl;
      sprintf(root_file_pe_name,"%s/ingrid_%08d_0000.daq.mid.pe.new.root",root_file_pe_folder,run_number);
      cout<<"out put file name is :"<<root_file_pe_name<<endl;
      break;
    case 'd':
      draw_flag=1;
      wait=1;
      cout<<"draw..."<<endl;
      break;
    case 't':
      gain_and_pedestal_file_flag=1;
      noise_file_flag=1;
      cout<<"save gain and pedestal data..."<<endl;
      break;

    case 'n':
      //noise_file_flag=atoi(optarg);
      noise_file_flag=1;
      cout<<"save noise data..."<<endl;
      break;
    case 'z':
      zero_sup_flag=1;
      using_pedestal_and_gain_file=atoi(optarg);
      cout<<"this run use zero suppression..."<<endl;
      break;
    case 'w':
      without_sigma=atof(optarg);
      cout<<"for analysis mppc gain parameter ..."<<endl;
      break;
    case 'c':
      convert_flag=true;
      cout<<"convert adc to pe..."<<endl;
      break;
    case 'p':
      pedestal_shift_check=true;
      cout<<"save (ADC mean)-(ADC pedestal)..."<<endl;
      break;
    case 'h':
      cout<<"this is the software to calculate pedestal and gain and convert adc to pe"<<endl;
      cout<<"r:[runnumber]"<<endl;
      cout<<"d:draw option"<<endl;
      cout<<"t:save pedestal and gain value"<<endl;
      cout<<"z:[runnumber] when converting adc to pe, use [run number]"<<endl;
      cout<<"n:save noise data"<<endl;
      cout<<"c:convert adc to pe"<<endl;
      cout<<"save (ADC mean)-(ADC pedestal)..."<<endl;
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
  Int_t LoAdc[NumMod][NumTFB][NumCh][NumCyc];
  Int_t TType;
  Long_t Tdc[NumMod][NumTFB][NumCh][NumCyc];
  Long_t Time[NumMod][NumTFB][NumCh][NumCyc];
  Long_t UTime[NumMod][NumTFB][NumCh][NumCyc];
  Int_t SNum;
  Int_t EvtNum;
  sprintf(buff1,"Adc[%d][%d][%d][%d]",NumMod,NumTFB,NumCh,NumCyc);
  IngEvt->SetBranchAddress(buff1,Adc);
  sprintf(buff1,"LoAdc[%d][%d][%d][%d]",NumMod,NumTFB,NumCh,NumCyc);
  IngEvt->SetBranchAddress(buff1,LoAdc);
  sprintf(buff1,"Tdc[%d][%d][%d][%d]",NumMod,NumTFB,NumCh,NumCyc);
  IngEvt->SetBranchAddress(buff1,Tdc);
  sprintf(buff1,"Time[%d][%d][%d][%d]",NumMod,NumTFB,NumCh,NumCyc);
  IngEvt->SetBranchAddress(buff1,Time);
  sprintf(buff1,"TType");
  IngEvt->SetBranchAddress(buff1,&TType);
  sprintf(buff1,"SNum");
  IngEvt->SetBranchAddress(buff1,&SNum);
  sprintf(buff1,"UTime",NumMod,NumTFB,NumCh,NumCyc);
  IngEvt->SetBranchAddress(buff1,UTime);
  sprintf(buff1,"EvtNum");
  IngEvt->SetBranchAddress(buff1,&EvtNum);

  Int_t Nevent_for_analysis =500;
  Int_t Nevent = IngEvt->GetEntries();
  if(Nevent<Nevent_for_analysis)Nevent_for_analysis=Nevent;

  Double_t gain[NumMod][NumTFB][NumCh];
  Double_t noise[NumMod][NumTFB][NumCh];
  Double_t pedestal[NumMod][NumTFB][NumCh];
  Double_t pedestal_sigma[NumMod][NumTFB][NumCh];
  Double_t temp;
  
  if(zero_sup_flag==0){//if not use zero suppression,caluculate pedestal and gain peak and gain
  //Fill histgram
  TH1F *mppc[NumMod][NumTFB][NumCh];
  for(Int_t nummod=0;nummod<NumMod;nummod++){
    for(Int_t numtfb=0;numtfb<NumTFB;numtfb++){
      for(Int_t numch=0;numch<NumCh;numch++){
	sprintf(buff1,"TPL%02dCh%02d",numtfb,numch);
	mppc[nummod][numtfb][numch]=new TH1F(buff1,buff1,HIST_MAX-HIST_MIN+1,HIST_MIN,HIST_MAX);
      }
    }
  }

  cout<<"Fill histgram"<<endl;
  //for(Int_t nevent=0;nevent<Nevent;nevent++){//change for speciall test 090418
  for(Int_t nevent=0;nevent<Nevent_for_analysis;nevent++){
    if(nevent==Nevent_for_analysis)break;
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
  Int_t flag_mppc;

  for(Int_t nummod=0;nummod<1;nummod++){
    for(Int_t numtfb=0;numtfb<UseNumTFB+2;numtfb++){
      for(Int_t numch=0;numch<NumCh;numch++){
	flag_mppc=false;
	ana_MPPC ana_mppc;
	ana_mppc.set_without_sigma(without_sigma);
	if(numtfb<11&&numch<48){
	  ana_mppc.analysis(mppc[nummod][numtfb][numch]);
	  flag_mppc=true;
	}

	else if(numtfb<11&&numch>=48){
	  ana_mppc.analysis_no_mppc(mppc[nummod][numtfb][numch]);
	  flag_mppc=false;
	}
	else if(numtfb==11&&numch<22){
	  ana_mppc.analysis(mppc[nummod][numtfb][numch]);
	  flag_mppc=true;
	}
	else if(numtfb==12&&numch<22){
	  ana_mppc.analysis(mppc[nummod][numtfb][numch]);
	  flag_mppc=true;
	}
	else if(numtfb==12&&(numch<46&&numch>23)){
	  ana_mppc.analysis(mppc[nummod][numtfb][numch]);
	  flag_mppc=true;
	}
	else if(numtfb==12&&numch>45){
	  ana_mppc.analysis_no_mppc(mppc[nummod][numtfb][numch]);
	  flag_mppc=false;
	}
	else{
	  ana_mppc.analysis_no_mppc(mppc[nummod][numtfb][numch]);
	  flag_mppc=false;
	}
	if(numch==0)cout<<"Mod.\t"<<nummod+1<<"\tTFB\t"<<numtfb+1<<"\tch\t"<<numch+1<<"\tpedestal\t"<<ana_mppc.get_pedestal()<<"\tonepe\t"<<ana_mppc.get_onepe()<<"\t gain\t"<<ana_mppc.get_gain()<<endl;


	gain[nummod][numtfb][numch]=ana_mppc.get_gain();
	pedestal[nummod][numtfb][numch]=ana_mppc.get_pedestal();
	pedestal_sigma[nummod][numtfb][numch]=ana_mppc.get_pedestal_sigma();
	noise[nummod][numtfb][numch]=ana_mppc.get_noise(Nevent_for_analysis*NumCyc);
	if(noise[nummod][numtfb][numch]>1000000)noise[nummod][numtfb][numch]=0;
	
	//debug 090603
	if(flag_mppc){
	  if(gain[nummod][numtfb][numch]<5||gain[nummod][numtfb][numch]>13){
	    cout<<"alarm tfb:"<<numtfb+1<<" ch:"<<numch<<" gain:"<<gain[nummod][numtfb][numch]<<endl;
	    gain[nummod][numtfb][numch]=44;
	    pedestal[nummod][numtfb][numch]=1000;
	    /*
	    TCanvas *c2 = new TCanvas("c2","c2",10,10,500,500);
	    mppc[nummod][numtfb][numch]->Draw();
	    mppc[nummod][numtfb][numch]->SetXTitle("Adc counts");
	    c2->Update();
	    cin>>temp;
	    delete c2;
	    */
  }
	}


      }//numch
    }//numtfb
  }//nummod

  //save ADCmean - pedestal and pedestal
  if(pedestal_shift_check){
    sprintf(buff1,"%s/ingrid_%08d_0000.daq.mid.noise.txt",pedestal_shift_folder,run_number);
    ofstream pedestal_shift_file(buff1);
    for(Int_t nummod=0;nummod<1;nummod++){
      for(Int_t numtfb=0;numtfb<UseNumTFB+2;numtfb++){
	for(Int_t numch=0;numch<NumCh;numch++){
	  pedestal_shift_file<<nummod<<"\t"<<numtfb<<"\t"<<numch<<"\t"<<(mppc[nummod][numtfb][numch]->GetMean()-pedestal[nummod][numtfb][numch])<<"\t"<<pedestal[nummod][numtfb][numch]<<endl;
	}//numch
      }//numtfb
    }//nummod
    pedestal_shift_file.close();
  }

 
  //draw histogram
  cout<<"draw histogram"<<endl;
  if(draw_flag==1){
    TCanvas *c2 = new TCanvas("c2","c2",10,10,500,500);
    sprintf(buff1,"%s/ingrid_%08d_noisehist.ps",pdf_folder,run_number);
    TPostScript ps(buff1);
    for(Int_t nummod=0;nummod<1;nummod++){
      for(Int_t numtfb=0;numtfb<UseNumTFB+2;numtfb++){
	for(Int_t numch=0;numch<UseNumCh;numch++){
	  //TH1F *frame = gPad->DrawFrame(HIST_MIN,0,HIST_MAX,3000);
	  //frame->SetXTitle("ADC counts");
	  //frame->Draw();

	  mppc[nummod][numtfb][numch]->Draw();
	  mppc[nummod][numtfb][numch]->SetXTitle("Adc counts");
	  c2->Update();
	  if(draw_flag==1&&wait==1)cin>>temp;
	}//numch
      }//numtfb
    }//nummod
    ps.Close();
  }//if





 }//zero sup flag==0


  if(zero_sup_flag==1){//if use zero suppression,use other run's pedestal and gain
  sprintf(buff1,"%s/ingrid_%08d_0000.daq.mid.gap.txt",gain_and_pedestal_folder,using_pedestal_and_gain_file);
  ifstream fusing_pedestal_and_gain_file(buff1);
  cout<<"use "<<buff1<<endl;
  
  for(Int_t nummod=0;nummod<1;nummod++){
  
    for(Int_t numtfb=0;numtfb<NumTFB;numtfb++){
  
      for(Int_t numch=0;numch<NumCh;numch++){
	
	fusing_pedestal_and_gain_file>>temp>>temp>>temp>>pedestal[nummod][numtfb][numch]>>temp>>gain[nummod][numtfb][numch];
	//cout<<pedestal[nummod][numtfb][numch]<<"\t"<<gain[nummod][numtfb][numch]<<endl;
      }//numch
    }//numtfb
  }//nummod
  fusing_pedestal_and_gain_file.close();
  }

  if(convert_flag){
  cout<<"convert adc to pe"<<endl;
  TFile *f_pe = new TFile(root_file_pe_name,"recreate");
  TTree *IngEvt_new = new TTree("IngEvt","IngEvt");
  Double_t pe[NumMod][NumTFB][NumCh][NumCyc];
  Double_t Lope[NumMod][NumTFB][NumCh][NumCyc];
  sprintf(buff1,"pe[%d][%d][%d][%d]",NumMod,NumTFB,NumCh,NumCyc);
  sprintf(buff2,"pe[%d][%d][%d][%d]/D",NumMod,NumTFB,NumCh,NumCyc);
  IngEvt_new->Branch(buff1,pe,buff2);
  sprintf(buff1,"Lope[%d][%d][%d][%d]",NumMod,NumTFB,NumCh,NumCyc);
  sprintf(buff2,"Lope[%d][%d][%d][%d]/D",NumMod,NumTFB,NumCh,NumCyc);
  IngEvt_new->Branch(buff1,Lope,buff2);
  sprintf(buff1,"Tdc[%d][%d][%d][%d]",NumMod,NumTFB,NumCh,NumCyc);
  sprintf(buff2,"Tdc[%d][%d][%d][%d]/L",NumMod,NumTFB,NumCh,NumCyc);
  IngEvt_new->Branch(buff1,Tdc,buff2);
  sprintf(buff1,"Time[%d][%d][%d][%d]",NumMod,NumTFB,NumCh,NumCyc);
  sprintf(buff2,"Time[%d][%d][%d][%d]/L",NumMod,NumTFB,NumCh,NumCyc);
  IngEvt_new->Branch(buff1,Time,buff2);
  sprintf(buff1,"UTime",NumMod,NumTFB,NumCh,NumCyc);
  sprintf(buff2,"UTime/L",NumMod,NumTFB,NumCh,NumCyc);
  IngEvt_new->Branch(buff1,&UTime,buff2);
  sprintf(buff1,"TType");
  sprintf(buff2,"TType/I");
  IngEvt_new->Branch(buff1,&TType,buff2);
  sprintf(buff1,"SNum");
  sprintf(buff2,"SNum/I");
  IngEvt_new->Branch(buff1,&SNum,buff2);
  sprintf(buff1,"EvtNum");
  sprintf(buff2,"EvtNum/I");
  IngEvt_new->Branch(buff1,&EvtNum,buff2);

  for(Int_t nevent=0;nevent<Nevent;nevent++){
    if((nevent+1)%1000==0)cout<<nevent+1<<"\t end"<<endl;
    IngEvt->GetEntry(nevent);
    for(Int_t nummod=0;nummod<1;nummod++){
      for(Int_t numtfb=0;numtfb<NumTFB;numtfb++){
	for(Int_t numch=0;numch<NumCh;numch++){
	  for(Int_t numcyc=0;numcyc<NumCyc;numcyc++){
	    pe[nummod][numtfb][numch][numcyc]=1.0*(Adc[nummod][numtfb][numch][numcyc]-pedestal[nummod][numtfb][numch])/gain[nummod][numtfb][numch];
	    if(numch>UseNumCh||numtfb>UseNumTFB){
	      pe[nummod][numtfb][numch][numcyc]=-70;
	    }
	    //Lope[nummod][numtfb][numch][numcyc]=1.0*(LoAdc[nummod][numtfb][numch][numcyc]-pedestal[nummod][numtfb][numch])/gain[nummod][numtfb][numch];
	    Lope[nummod][numtfb][numch][numcyc]=1.0*(LoAdc[nummod][numtfb][numch][numcyc]);
	  }//numcyc
	}//numch
      }//numtfb
    }//nummod
    IngEvt_new->Fill();
  }//nevent


  IngEvt->Write();
  f_pe->Write();
  f_pe->Close();
  f->Close();
  }

  //write gain and pedestal data
  if(gain_and_pedestal_file_flag==1){
    sprintf(buff1,"%s/ingrid_%08d_0000.daq.mid.gap.txt",gain_and_pedestal_folder,run_number);
    ofstream gain_and_pedestal_file(buff1);
    for(Int_t nummod=0;nummod<1;nummod++){
      for(Int_t numtfb=0;numtfb<NumTFB;numtfb++){
	for(Int_t numch=0;numch<NumCh;numch++){
	  gain_and_pedestal_file<<nummod+1<<"\t";
	  gain_and_pedestal_file<<numtfb+1<<"\t";
	  gain_and_pedestal_file<<numch+1<<"\t";
	  gain_and_pedestal_file<<pedestal[nummod][numtfb][numch]<<"\t";
	  gain_and_pedestal_file<<pedestal_sigma[nummod][numtfb][numch]<<"\t";
	  gain_and_pedestal_file<<gain[nummod][numtfb][numch]<<endl;
	}//numch
      }//numtfb
    }//nummod
  }//flag

  //write gain and pedestal data
  if(noise_file_flag==1){

    sprintf(buff1,"%s/ingrid_%08d_0000.daq.mid.noise.txt",noise_folder,run_number);
    ofstream noise_file(buff1);
    for(Int_t nummod=0;nummod<NumMod;nummod++){
      for(Int_t numtfb=0;numtfb<NumTFB;numtfb++){
	for(Int_t numch=0;numch<NumCh;numch++){
	  noise_file<<nummod+1<<"\t";
	  noise_file<<numtfb+1<<"\t";
	  noise_file<<numch+1<<"\t";
	  noise_file<<noise[nummod][numtfb][numch]<<endl;
	}//numch
      }//numtfb
    }//nummod
    noise_file.close();
  }//flag


  sprintf(root_file_pe_name,"touch %s/ingrid_%08d_0000.daq.mid.pe.new.root.pedone",root_file_pe_folder,run_number);
  system(root_file_pe_name);


}
