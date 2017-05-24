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
  Double_t charge[NumMod][NumTFB][NumCh][ana_number];
  Double_t pedestal[NumMod][NumTFB][NumCh][ana_number];
  for(Int_t numrun=0;numrun<ana_number;numrun++){
    sprintf(file_name,"%s/ingrid_%08d_0000.daq.mid.noise.txt",pedestal_shift_folder,ana_run_number[numrun]);//ADC mean - pedestal and pedestal data
    ifstream file(file_name);
    for(Int_t nummod=0;nummod<NumMod;nummod++){
      for(Int_t numtfb=0;numtfb<NumTFB;numtfb++){
	for(Int_t numch=0;numch<NumCh;numch++){
	  file>>temp>>temp>>temp>>charge[nummod][numtfb][numch][numrun]>>pedestal[nummod][numtfb][numch][numrun];
	}//ch
      }//nummod
    }//numtpl
    file.close();
  }//numrun

  char buff1[400];
  TCanvas *c1 = new TCanvas("c1","c1",10,10,500,500);
  Double_t chi2;//fit chisquare
  Double_t ndf;//fit ndf(=5)
  TGraph *g[NumMod][NumTFB][NumCh];//(ADC mean - pedestal):x (pedestal):y
  
  TH1F *slop = new  TH1F("slop","slop",80,-1.1,-0.7);
  TH1F *chi2_ndf = new  TH1F("chi2_ndf","chi2_ndf",80,0,0.04);
  for(Int_t nummod=0;nummod<NumMod;nummod++){
    for(Int_t numtfb=0;numtfb<11;numtfb++){
      for(Int_t numch=0;numch<UseNumCh;numch++){
	TF1 *pol1 = new TF1("pol1","pol1",0,100);

	sprintf(buff1,"TFB%02dCh%02d",numtfb,numch);
	g[nummod][numtfb][numch]=new TGraph(ana_number,charge[nummod][numtfb][numch],pedestal[nummod][numtfb][numch]);
	g[nummod][numtfb][numch]->SetName(buff1);
	g[nummod][numtfb][numch]->SetMarkerStyle(8);
	g[nummod][numtfb][numch]->SetMarkerSize(0.7);
	//Fit
	g[nummod][numtfb][numch]->Fit("pol1","q","",0,10);
	chi2=pol1->GetChisquare();
	ndf=pol1->GetNDF();
	chi2_ndf->Fill(chi2/ndf);
	slop->Fill(pol1->GetParameter(1));

	if(chi2/ndf>0.04){
	  cout<<"error chi2 tfb:"<<numtfb<<" ch:"<<numch<<endl;
	  c1->Clear();
	  g[nummod][numtfb][numch]->Draw("AP");
	  c1->Update();
	  cout<<"draw:"<<numtfb<<"-"<<numch<<" "<<chi2<<"/"<<ndf<<endl;
	  cin>>temp;

	}

	if(pol1->GetParameter(1)<-1.1||pol1->GetParameter(1)>-0.7)cout<<"error slope tfb:"<<numtfb<<" ch:"<<numch<<endl;

	delete pol1;
      }//numch
    }//numtfb
  }//nummod

  c1->Clear();
  slop->Draw();
  c1->Update();
  //c1->Print("~/090603_pedestalshift_slope.pdf");
  cin>>temp;

  c1->Clear();
  chi2_ndf->Draw();
  c1->Update();
  //c1->Print("~/090603_pedestalshift_fitchi2ndf.pdf");
  cin>>temp;



}//end

    
