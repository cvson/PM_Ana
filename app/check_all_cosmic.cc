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
#include <TMath.h>
#include <TSpectrum.h>
#include <TString.h>
#include <TSystem.h>

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
  while ((c = getopt(argc, argv, "r:h")) != -1) {
    switch(c){
    case 'r':
      run_number=atoi(optarg);
      break;
    case 'h':
      cout<<"help menu"<<endl;
      cout<<"-r [run_number]"<<endl;
      break;
    }
  }

  char temp[100];

  sprintf(buff1,"%s/ingrid_%08d_cosmic.root",root_file_folder,run_number);
  FileStat_t fs;
  if(gSystem->GetPathInfo(buff1,fs)){
    cout<<"Cannot open file "<<buff1<<endl;
    exit(1);
  }

  TFile *f = new TFile(buff1,"read");



  //read hist
  cout<<"read hist..."<<endl;
  TH1F *cosmic[NumMod][NumTFB][NumCh];
  TH1F *all = new TH1F("all","all",200,-2,120);
  TF1 *gaus = new TF1("gaus","gaus",0,100);
  for(Int_t nummod=0;nummod<NumMod;nummod++){
    for(Int_t numtfb=0;numtfb<13;numtfb++){
      for(Int_t numch=0;numch<UseNumCh;numch++){
	sprintf(buff1,"cosmic[%02d][%02d][%02d]",nummod+1,numtfb+1,numch+1);

	cosmic[nummod][numtfb][numch] = new TH1F(buff1,buff1,200,-2,120);
	sprintf(buff1,"TFB%02d_Ch%02d",numtfb+1,numch+1);
	cosmic[nummod][numtfb][numch] = (TH1F*)f->Get(buff1);

	//cosmic[nummod][numtfb][numch]->Fit("gaus","q","",10,50);

	//double ly=gaus->GetParameter(1);

	double mean=cosmic[nummod][numtfb][numch]->GetMean();
	int ent=cosmic[nummod][numtfb][numch]->GetEntries();
	
	/*
	if(13>=numtfb&&numtfb>=11)cout<<mean<<"\t"<<ent<<endl;
	if((mean<25&&numch<48)||(ent<200&&numch<48)){
	  cout<<"caution\ttfb:"<<numtfb+1<<"\tch:"<<numch+1<<endl;
	  cout<<"please call otani"<<endl;
	  cin>>mean;
	}
	*/

	//if(numch<48&&numtfb<11&&ent<2100)cout<<numtfb+1<<"\t"<<numch+1<<endl;
	cout<<mean<<"\t"<<ent<<endl;
	//cout<<cosmic[nummod][numtfb][numch]->GetMean()<<endl;
	all->Add(cosmic[nummod][numtfb][numch]);
      }//numch
    }//numtfb
  }//nummod

  //draw hist
  cout<<"draw hist"<<endl;
  TCanvas *c1 = new TCanvas("c1","c1",10,10,800,800);
  Int_t count=1;

  all->Draw();
  c1->Update();
  cin>>temp;

  c1->Clear();
  c1->Divide(4,4);

  TH1F *frame[NumMod][NumTFB][NumCh];
  for(Int_t nummod=0;nummod<NumMod;nummod++){
    for(Int_t numtfb=0;numtfb<13;numtfb++){

      for(Int_t numch=0;numch<UseNumCh;numch++){
	c1->cd(count);
	count++;

	//frame[nummod][numtfb][numch]=gPad->DrawFrame(0,0,100,100);
	//frame[nummod][numtfb][numch]->Draw();

	cosmic[nummod][numtfb][numch]->Draw();


	if(count==17){
	  cout<<"tfb:"<<numtfb+1<<"\tch:"<<numch-14<<"~"<<numch+1<<endl;
	  c1->Update();
	  cin>>temp;
	  c1->Clear();
	  c1->Divide(4,4);
	  count=1;

	}//count==17

      }//nummod
    }//numtfb
  }//numch
  f->Close();
}
