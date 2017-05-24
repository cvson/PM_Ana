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
	    if(numtfb<11&&numch<48){//tracking plane
	      hist_adc[nummod][numtfb][numch]->Fill(Adc[nummod][numtfb][numch][numcyc]);
	    }
	    else if(numtfb>=11&&numch<22){//veto plane
	      hist_adc[nummod][numtfb][numch]->Fill(Adc[nummod][numtfb][numch][numcyc]);
	    }

	  }//numcyc
	}//numch
    }}//numtfb
    }}//nummod
  }//nevent



  //draw hist
  cout<<"draw hist"<<endl;
  TCanvas *c1 = new TCanvas("c1","c1",0,0,950,950);
  Int_t count=1;
  c1->Clear();
  c1->Divide(4,4);
  for(Int_t nummod=0;nummod<NumMod;nummod++){
  if(WriMod==-1||WriMod==nummod){
    for(Int_t numtfb=0;numtfb<NumTFB;numtfb++){
    if(WriPlane==-1||WriPlane==numtfb){
      for(Int_t numch=0;numch<NumCh;numch++){
	c1->cd(count);
	count++;
	hist_adc[nummod][numtfb][numch]->Draw();
	cout<<"tfb:"<<numtfb+1<<"\tch:"<<numch+1<<endl;

	if(count==17){
	  sprintf(buff1,"run_%08d_Mod%02d_TPL%02d_Ch%02d.pdf",run_number,nummod,numtfb,numch);
	  c1->Update();
	  c1->Print(buff1);
	  cin>>temp;

	  c1->Clear();
	  c1->Divide(4,4);
	  count=1;

	}//count==17

      }//numch
    }}//numtfb
  }}//nummod
  f->Close();
}
