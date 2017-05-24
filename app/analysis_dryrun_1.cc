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
#include "INGRID.cxx"

#define DEBUG 0
//const Int_t TdcStep =    1;
const Int_t TdcStep =    5;
const Int_t TdcRes  =   15;
const Int_t Reset   =  100;
const Int_t Gate    = 1500;
const Int_t TdcOffset  =  300;
 
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

  //sprintf(buff1,"/home/daq/data/test/ingrid.root");
  sprintf(buff1,"/home/daq/data/test/ingrid_%08d_0000.daq.mid.new.root",run_number);
  FileStat_t fs;
  if(gSystem->GetPathInfo(buff1,fs)){
    cout<<"Cannot open file "<<buff1<<endl;
    exit(1);
  }
  TH1F *test1 = new TH1F("test1","test1",50,0,50);
  TH1F *test2 = new TH1F("test2","test2",50,0,50);


  TFile*    f          = new TFile(buff1,"read");
  TTree*    IngTree    = (TTree*)f->Get("IngTree");
  Int_t     Nevent     = IngTree->GetEntries();
  IngEvt*   fIngEvt    = new IngEvt();
  TCanvas*  c1         = new TCanvas("c1","c1",10,10,700,700);
  IngTree->SetBranchAddress("IngEvt",&fIngEvt);

  TFile *wf = new TFile("anatest.root","recreate");
  TH1F*     fH_Cyc    = new TH1F("fH_Cyc","fH_Cyc",((Gate+Reset)*23-Reset)/100,(Gate+Reset)*0+TdcOffset-100,(Gate+Reset)*23+TdcOffset-Reset+100);
  TH1F*     fH_Cyc_wc    = new TH1F("fH_Cyc_wc","fH_Cyc_wc",((Gate+Reset)*23-Reset)/100,(Gate+Reset)*0+TdcOffset-100,(Gate+Reset)*23+TdcOffset-Reset+100);
  TH1F*     fH_Cyc0    = new TH1F("fH_Cyc0","fH_Cyc0",Gate/10,(Gate+Reset)*0+TdcOffset,(Gate+Reset)*1+TdcOffset-Reset);
  TH1F*     fH_Cyc0_wc = new TH1F("fH_Cyc0_wc","fH_Cyc0_wc",Gate/10,(Gate+Reset)*0+TdcOffset,(Gate+Reset)*1+TdcOffset-Reset);



  for(int n=0;n<Nevent;n++){
    if(n%1000==0)cout<<"event "<<n<<endl;
    if(n==23000)break;
    IngTree           -> GetEntry(n);
    Int_t  Cycle      =  fIngEvt->Cycle;
    if(DEBUG){
      cout<<"-------------------event:"<<n<<"--------------------"<<endl;
      cout<<" *********cycle:"<<Cycle<<" **********"<<endl;
    }
    Int_t  TdcStart   = (Gate+Reset)* Cycle+TdcOffset;
    Int_t  TdcStop    = (Gate+Reset)*(Cycle+1)+TdcOffset-Reset;

    //for(Int_t nummod = 0; nummod<14; nummod++){
    for(Int_t nummod = 0; nummod<1; nummod++){
      if(DEBUG)cout<<" **module:"<<nummod<<" **"<<endl;
      ModEvt        fModEvt    =  fIngEvt->fModEvt[nummod];
      //X layer
      TClonesArray* arr        = new TClonesArray("IngHit");
      arr                      = fModEvt.fXHits;
      Int_t         nhit       = fModEvt.NHitX;
      Bool_t        Hit[nhit];
      for(Int_t i=0;i<nhit;i++){Hit[i]=false;}

      for(Int_t BTime  = TdcStart;
	        BTime <= TdcStop ;
	        BTime  = BTime + TdcStep){
	//cout the Number of TDC hit
	Int_t NHit=0;
	for(Int_t i=0;i<nhit;i++){//All Hit
	  IngHit *fHit = (IngHit*)(arr->At(i));
	  Long_t time  = fHit->Time;
	  if(fabs(BTime-time)<TdcRes)NHit++;
	}
	//up hit flag
	if(NHit>=3){
	  for(Int_t i=0;i<nhit;i++){//All Hit
	    IngHit *fHit = (IngHit*)(arr->At(i));
	    Long_t time  = fHit->Time;
	    if(fabs(BTime-time)<TdcRes)Hit[i]=true;
	  }
	}//NHit>=3
      }//Tdc Step


      for(Int_t i=0;i<nhit;i++){//All Hit
	IngHit *fHit = (IngHit*)(arr->At(i));
	Long_t time  = fHit->Time;
	Int_t pln = fHit->Pln;
	Int_t ch=fHit->Ch;
	Int_t adc=fHit->HighAdc;
	fH_Cyc->Fill(time);
	if(Cycle==0)fH_Cyc0->Fill(time);
	//cout<<"  time:"<<time<<" X:"<<pln<<" ch:"<<ch<<" adc"<<adc<<endl;
	if(Hit[i]){
	  Int_t pln  = fHit->Pln;
	  Int_t ch   = fHit->Ch;
	  Int_t adc  = fHit->HighAdc;
	  fH_Cyc_wc->Fill(time);
	  if(Cycle==0)fH_Cyc0_wc->Fill(time);
	  //cout<<"  time:"<<time<<" X:"<<pln<<" ch:"<<ch<<" adc"<<adc<<endl;
	}
      }


      arr->Delete();
      //Y layer
    }//nummod
    if(DEBUG&&Cycle==0){
      fH_Cyc0->SetLineWidth(2);
      fH_Cyc0->Draw();
      fH_Cyc0_wc->SetLineColor(2);
      fH_Cyc0_wc->Draw("same");

      if(fH_Cyc0->GetEntries()>0){
	test1->Fill(fH_Cyc0->GetEntries());
	if(fH_Cyc0_wc->GetEntries()>0)test2->Fill(fH_Cyc0_wc->GetEntries());
      }

      c1->Update();
      //cin.get();
      c1->Clear();
      fH_Cyc0->Delete();
      fH_Cyc0_wc->Delete();

      fH_Cyc0    = new TH1F("fH_Cyc0","fH_Cyc0",Gate,(Gate+Reset)*0+TdcOffset,(Gate+Reset)*1+TdcOffset-Reset);
      fH_Cyc0_wc = new TH1F("fH_Cyc0_wc","fH_Cyc0_wc",Gate,(Gate+Reset)*0+TdcOffset,(Gate+Reset)*1+TdcOffset-Reset);
 
    }
  }//nevent
  test1->Draw();

  /*
  c1->Update();
  c1->Print("091019_NCh_within_cycle.pdf");
  cin.get();
  c1->Clear();
  test2->Draw();
  c1->Update();
  c1->Print("091019_NCh_within_cycle_wc.pdf");
  cin.get();
  */

  fH_Cyc->Write();
  fH_Cyc_wc->Write();
  fH_Cyc0->Write();
  fH_Cyc0_wc->Write();
  wf->Close();


  f->Close();

}
 
