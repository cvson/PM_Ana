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

#include "TApplication.h"

//experimental setup

#include "setup.hxx"

int main(int argc,char *argv[]){
  TROOT root("GUI","GUI");
  TApplication theApp("App",&argc,argv);
  struct stat sta;
  gStyle->SetOptFit(1);
  gStyle->SetNdivisions(012,"X");
  gStyle->SetNdivisions(012,"Y");
  gStyle->SetTitleOffset(1.0,"X");
  gStyle->SetTitleOffset(1.0,"Y");
  gStyle->SetLabelSize(0.06,"X");
  gStyle->SetLabelSize(0.06,"Y");
  gStyle->SetTitleSize(0.06,"X");
  gStyle->SetTitleSize(0.06,"Y");
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetPadGridX(1);
  gStyle->SetPadGridY(1);
  gStyle->SetCanvasColor(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadColor(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetStatColor(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetTitleFillColor(0);
  Double_t temp;

  //read setup  
  cout<<"runnumber?"<<endl;
  Int_t run_number;
  cin>>run_number;
  char root_file_name[400];
  sprintf(root_file_name,"%s/ingrid_%08d_0000.daq.mid.pe.new.root",root_file_pe_folder,run_number);
  TFile *root_file = new TFile(root_file_name,"read");
  TTree *IngEvt = (TTree*)root_file->Get("IngEvt");
  Double_t pe[NumMod][NumTFB][NumCh][NumCyc];
  Long_t Tdc[NumMod][NumTFB][NumCh][NumCyc];
  Double_t pe_thre[NumMod][NumTFB][NumCyc];
  Int_t Nevent=IngEvt->GetEntries();
  IngEvt->SetBranchAddress("pe[1][15][64][23]",pe);
  IngEvt->SetBranchAddress("Tdc[1][15][64][23]",Tdc);

  for(Int_t nummod=0;nummod<NumMod;nummod++){
    for(Int_t numtpl=0;numtpl<NumTFB;numtpl++){
      for(Int_t ch=0;ch<NumCh;ch++){
	pe_thre[nummod][numtpl][ch]=100;
      }//ch
    }//nummod
  }//numtpl
  
  char result_file_name[400];
  sprintf(result_file_name,"%s/ingrid_%08d_result.txt",dac_calib_folder,run_number);
  ofstream result_file(result_file_name);

  for(Int_t event=0;event<Nevent;event++){
    IngEvt->GetEntry(event);
    if(event%1000==0)cout<<event<<endl;
    for(Int_t nummod=0;nummod<NumMod;nummod++){
      for(Int_t numtpl=0;numtpl<NumTFB;numtpl++){
	for(Int_t numch=0;numch<NumCh;numch++){
	  for(Int_t numcyc=0;numcyc<NumCyc;numcyc++){
	    if(Tdc[nummod][numtpl][numch][numcyc]<10000000){
	      if(pe_thre[nummod][numtpl][numch]>pe[nummod][numtpl][numch][numcyc]&&pe[nummod][numtpl][numch][numcyc]>-3){
		pe_thre[nummod][numtpl][numch]=pe[nummod][numtpl][numch][numcyc];
	      }//if(Adc)
	    }//if(Tdc)
	  }//numcyc
	}//numch
      }//numtpl
    }//nummod
  }//event


  for(Int_t nummod=0;nummod<NumMod;nummod++){
    for(Int_t numtpl=0;numtpl<11;numtpl++){
      for(Int_t ch=0;ch<NumCh;ch++){
	result_file<<pe_thre[nummod][numtpl][ch]<<"\t";
	cout<<numtpl+1<<"\t"<<ch<<"\t"<<pe_thre[nummod][numtpl][ch]<<endl;
      }
      result_file<<endl;
    }
  }
  result_file<<endl;
    
  root_file->Close();

  result_file.close();

}

    
