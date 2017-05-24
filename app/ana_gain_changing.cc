/////////////////////////
///for pedestal shift problem 090603
///ana_run_number:run_number voltage scan data
/////////////////////////
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
#include "setup.hxx"

//analyze run number
const static Int_t ana_number=8;
const static Int_t ana_run_number[ana_number]={1464,1465,1466,1467,1468,1469,1470,1471};

int main(int argc,char *argv[]){
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
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
  double temp;

  //read setup  
  char file_name[400];
  struct stat st;
  Int_t NFile=0;
  Int_t NumFile[1000];
  for(Int_t i=1800;i<1945;i++){
    sprintf(file_name,"%s/ingrid_%08d_0000.beam.root",beam_folder,i);
    if(stat(file_name,&st)==0){
      NumFile[NFile]=i;
      NFile++;
      cout<<"run "<<i<<" read"<<endl;
    }
  }
  Double_t pedestal[NumMod][NumTFB][NumCh][NFile];
  Double_t gain[NumMod][NumTFB][NumCh][NFile];
  Double_t pedestal_sigma[NumMod][NumTFB][NumCh][NFile];


  for(Int_t i=0;i<NFile;i++){
    sprintf(file_name,"%s/ingrid_%08d_0000.daq.mid.gap.txt",gain_and_pedestal_folder,NumFile[i]);
    ifstream file(file_name);
    cout<<"reading "<<file_name<<endl;
    for(Int_t nummod=0;nummod<NumMod;nummod++){
      for(Int_t numtfb=0;numtfb<13;numtfb++){
	for(Int_t numch=0;numch<NumCh;numch++){
	  file>>temp>>temp>>temp>>pedestal[nummod][numtfb][numch][i]>>pedestal_sigma[nummod][numtfb][numch][i]>>gain[nummod][numtfb][numch][i];

	  if(numch<48){
	    if(gain[nummod][numtfb][numch][i]>20||gain[nummod][numtfb][numch][i]<4){
	      cout<<"Run:"<<NumFile[i]<<"TFB:"<<numtfb+1<<" Ch:"<<numch+1<<endl;
	    }
	  }
	}//ch
      }//numtfb
    }//nummod
    file.close();
    cin>>temp;
  }//i



  char buff1[400];
  TCanvas *c1 = new TCanvas("c1","c1",10,10,500,500);
  TGraph *g[NumMod][NumTFB][NumCh];//(ADC mean - pedestal):x (pedestal):y
  Double_t x[NFile];
  for(int i=0;i<NFile;i++)x[i]=i+1;
  for(Int_t nummod=0;nummod<NumMod;nummod++){
    for(Int_t numtfb=0;numtfb<11;numtfb++){
      for(Int_t numch=0;numch<UseNumCh;numch++){

	sprintf(buff1,"TFB%02dCh%02d",numtfb,numch);
	g[nummod][numtfb][numch]=new TGraph(NFile,x,gain[nummod][numtfb][numch]);
	g[nummod][numtfb][numch]->SetName(buff1);
	g[nummod][numtfb][numch]->SetMarkerStyle(8);
	g[nummod][numtfb][numch]->SetMarkerSize(0.7);
	//Fit

	c1->Clear();
	g[nummod][numtfb][numch]->Draw("AP");
	c1->Update();
	cin>>temp;
      }//numch
    }//numtfb
  }//nummod





}//end

    
