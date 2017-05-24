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
#include <TH2.h>

#include <TGraph.h>
#include <TGaxis.h>
#include <TMarker.h>
#include <TText.h>
#include <TMath.h>
#include <TSpectrum.h>
#include <TString.h>
#include <TSystem.h>
#include <TPad.h>

#include "TApplication.h"
#include "setup.hxx"
#include "root_setup.hxx"

int main(int argc,char *argv[]){
  char buff1[200],buff2[200];
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  root_setup froot_setup;

  //read run number
  Int_t run_number;
  Int_t c=-1;
  char FileName[300];
  int WriMod=-1;
  int WriPlane=-1;
  while ((c = getopt(argc, argv, "r:h:p:m:")) != -1) {
    switch(c){
    case 'r':
      run_number=atoi(optarg);
      break;
    case 'p':
      WriPlane=atoi(optarg);
      break;
    case 'm':
      WriMod=atoi(optarg);
      break;
    case 'h':
      cout<<"help menu"<<endl;
      cout<<"-r [run_number]"<<endl;
      break;
    }
  }
  char temp[200];

  sprintf(buff1,"%s/ingrid_%08d_0000.daq.mid.new.root",root_file_folder,run_number);
  FileStat_t fs;
  if(gSystem->GetPathInfo(buff1,fs)){
    cout<<"Cannot open file "<<buff1<<endl;
    exit(1);
  }

  TFile *f = new TFile(buff1,"read");
  TTree *IngEvt = (TTree*)f->Get("IngEvt");
  Int_t Adc[NumMod][NumTFB][NumCh][NumCyc];
  sprintf(buff1,"Adc[%d][%d][%d][%d]",NumMod,NumTFB,NumCh,NumCyc);
  IngEvt->SetBranchAddress(buff1,Adc);
  Int_t Nevent = IngEvt->GetEntries();


  //create hist
  cout<<"creat hist..."<<endl;
  TH1F *hist_adc[NumMod][NumTFB][NumCh];
  for(Int_t nummod=0;nummod<NumMod;nummod++){
    for(Int_t numtfb=0;numtfb<NumTFB;numtfb++){
      for(Int_t numch=0;numch<NumCh;numch++){
	if(numtfb<11){
	  sprintf(buff1,"Mod%02d_TPL%02d_Ch%02d",nummod,numtfb,numch);
	}
	if(numtfb>=11){
	  sprintf(buff1,"Mod%02d_VETO%02d_Ch%02d",nummod,numtfb-11,numch);
	}
	hist_adc[nummod][numtfb][numch] = new TH1F(buff1,buff1,100,100,200);
	hist_adc[nummod][numtfb][numch]->SetName(buff1);
	hist_adc[nummod][numtfb][numch]->SetXTitle("Adc counts");
	hist_adc[nummod][numtfb][numch]->SetYTitle("# of events");
      }//numch
    }//numtfb
  }//nummod

  //Fill hist from data
  cout<<"fill data..."<<endl;
  for(Int_t nevent=0;nevent<Nevent;nevent++){
    IngEvt->GetEntry(nevent);
  for(Int_t nummod=0;nummod<NumMod;nummod++){
  if(WriMod==-1||WriMod==nummod){
    for(Int_t numtfb=0;numtfb<NumTFB;numtfb++){
    if(WriPlane==-1||WriPlane==numtfb){
      for(Int_t numch=0;numch<NumCh;numch++){
	for(Int_t numcyc=0;numcyc<NumCyc;numcyc++){
	  if(numtfb<11 &&numch<48){//tracking plane
	    hist_adc[nummod][numtfb][numch]->Fill(Adc[nummod][numtfb][numch][numcyc]);
	  }
	  else if(numtfb>=11&&numch<24){//veto plane
	    hist_adc[nummod][numtfb][numch]->Fill(Adc[nummod][numtfb][numch][numcyc]);
	  }
	}//numcyc
      }//numch
    }}//numtfb
    }}//nummod
  }//nevent
  
  //draw hist
  cout<<"draw hist"<<endl;
  char modnumtpl[100],modnumveto[100];
  char filename[100];
  Int_t NCh,num;
  Float_t NUMtpl[NumTFB*NumCh],RMStpl[NumTFB*NumCh];
  Float_t NUMveto[NumTFB*NumCh],RMSveto[NumTFB*NumCh];
  Color_t C=2;

  //TCanvas *c2 = new TCanvas("c2","c2",0,0,700,700);
  TCanvas *c1 = new TCanvas("c1","c1",0,0,950,950);
    TCanvas *c2 = new TCanvas("c2","c2",0,0,700,700);
  TSpectrum *s = new TSpectrum();
  TGraph *g;
  TF1 *f1 = new TF1("f1","gaus");
  //gStyle->SetOptFit();

  Int_t count=1;
  c1->Clear();
  c1->Divide(4,4);
  for(Int_t nummod=0;nummod<NumMod;nummod++){
  if(WriMod==-1||WriMod==nummod){
    Int_t i=0,j=0;
    sprintf(modnumtpl,"H%d TPLs",nummod+1);
    sprintf(modnumveto,"H%d VETOs",nummod+1);
    for(Int_t numtfb=0;numtfb<NumTFB;numtfb++){
    if(WriPlane==-1||WriPlane==numtfb){
      if(numtfb<11) NCh=48;
      else NCh=24;
      for(Int_t numch=0;numch<NCh;numch++){
	c1->cd(count);
	count++;
	hist_adc[nummod][numtfb][numch]->Draw();
	hist_adc[nummod][numtfb][numch]->Fit(f1,"q");
	Float_t peak=f1->GetParameter(1);
	/*	cout<<"tfb:"<<numtfb<<"\tch:"<<numch<<endl;
	if(count==17){
	  c1->Update();
	  cin.get();
	  c1->Clear();
	  c1->Divide(4,4);
	  count=1;
	  //sprintf(filename,"0module_%d_fit.pdf",nummod);
	  //c1->Print(filename);
	  }//count==17*/

	if(numtfb<11){
	  RMStpl[i]=f1->GetParameter(2);
	  NUMtpl[i]=i;
	  for(Int_t numcyc=0;numcyc<NumCyc;numcyc++){
	  if(Adc[nummod][numtfb][numch][numcyc]>(peak+RMStpl[i]*5) || Adc[nummod][numtfb][numch][numcyc]<(peak-RMStpl[i]*5)){
	    cout<<"module:"<<nummod<<"  TFB:"<<numtfb<<"  Ch"<<numch;
	    cout<<"  "<<Adc[nummod][numtfb][numch][numcyc] <<endl;
	  }}
	  i++;
	}
	else{
	  RMSveto[j]=f1->GetParameter(2);
	  NUMveto[j]=j;
	  for(Int_t numcyc=0;numcyc<NumCyc;numcyc++){
	  if(Adc[nummod][numtfb][numch][numcyc]>0 && (Adc[nummod][numtfb][numch][numcyc]>(peak+RMSveto[j]*5) || Adc[nummod][numtfb][numch][numcyc]<(peak-RMSveto[j]*5))){
	    cout<<"module:"<<nummod<<"  TFB:"<<numtfb<<"  Ch"<<numch;
	    cout<<"  "<<Adc[nummod][numtfb][numch][numcyc] <<endl;
	  }}
	  j++;
	}
      }//numch
    }}//numtfb
      
    //draw RMS
    if(nummod>0)cin.get();
    cout << modnumtpl << endl;
    c2->Clear();
    g = new TGraph(i,NUMtpl,RMStpl);
    g->Draw("APL");
    Double_t *nx=g->GetX();
    Double_t *ny=g->GetY();
    for(Int_t n=0;n<i;n++){
      if(C>4) C=2;
      TMarker *m1 = new TMarker(nx[n],ny[n],20);
      m1->SetMarkerColor(C);
      m1->Draw();
      if((n+1)%16==0) C++;
    }
    g->SetTitle(modnumtpl);
    g->GetXaxis()->SetTitle("Ch(0-47)*TPL(0-10)");
    g->GetYaxis()->SetTitle("RMS");
    c2->Update();
    //sprintf(filename,"module_%d.pdf",nummod);
    //c2->Print(filename);
     cin.get();
    
    cout << modnumveto << endl;
    c2->Clear();
    g = new TGraph(j,NUMveto,RMSveto);
    g->Draw("APL");
    Double_t *NX=g->GetX();
    Double_t *NY=g->GetY();
    for(Int_t n=0;n<j;n++){
      if(C>4) C=2;
      TMarker *m2 = new TMarker(NX[n],NY[n],20);
      m2->SetMarkerColor(C);
      m2->Draw();
      if((n+1)%16==0) C++;
    }
    g->SetTitle(modnumveto);
    g->GetXaxis()->SetTitle("Ch(0-22)*TPL(0-3)");
    g->GetYaxis()->SetTitle("RMS");
    c2->Update();
    //sprintf(filename,"module_%d_veto.pdf",nummod);
    //c2->Print(filename);	
  }}//nummod
  f->Close();
}
