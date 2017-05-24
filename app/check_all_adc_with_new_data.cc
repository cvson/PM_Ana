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
#include <TClonesArray.h>
#include <TObject.h>
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
#include "ingrid.hxx"

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
  sprintf(buff1,"/home/daq/data/test/ingrid_%08d_0000.daq.mid.new.root",run_number);

  //sprintf(buff1,"/home/daq/data/test/ingrid.root");
  sprintf(buff1,"/home/daq/data/test/ingrid_%08d_0000.daq.mid.new.root",run_number);
  FileStat_t fs;
  if(gSystem->GetPathInfo(buff1,fs)){
    cout<<"Cannot open file "<<buff1<<endl;
    exit(1);
  }

  TFile *f = new TFile(buff1,"read");
  TTree *tree = (TTree*)f->Get("IngTree");
  Int_t Nevent=tree->GetEntries();
  IngEvt *fIngEvt = new IngEvt();
  tree->SetBranchAddress("IngEvt",&fIngEvt);

  cout<<Nevent<<endl;

  for(int n=0;n<Nevent;n++){
    if(n%100==0)cout<<"event "<<n<<endl;
    tree->GetEntry(n);
    for(Int_t nummod=0;nummod<16;nummod++){
      ModEvt fModEvt = fIngEvt->fModEvt[nummod];
      TClonesArray *arr = new TClonesArray("IngHit");
      int nhit=fModEvt.NHitX;
      arr = fModEvt.fXHits;

      for(Int_t i=0;i<nhit;i++){
	IngHit *fHit = (IngHit*)(arr->At(i));
	Int_t pln = fHit->Pln;
	Int_t ch = fHit->Ch;
	Int_t view = fHit->View;
	if(view==1){ch=ch+24;}
	hist_adc[nummod][pln][ch]->Fill(fHit->HighAdc);
      }//nhit
      arr->Delete();
      arr = fModEvt.fYHits;
      nhit=fModEvt.NHitY;
      for(Int_t i=0;i<nhit;i++){
	IngHit *fHit = (IngHit*)(arr->At(i));
	Int_t pln = fHit->Pln;
	Int_t ch = fHit->Ch;
	Int_t view = fHit->View;
	if(view==1){ch=ch+24;}
	hist_adc[nummod][pln][ch]->Fill(fHit->HighAdc);
      }//nhit

    }//nummod
  }//nevent


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
 
