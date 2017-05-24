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
#include "INGRID_Dimension.cxx"

#define DEBUG 0
#define DEBUG2 0
//const Int_t TdcStep =    1;
const Int_t TdcStep =    5;
const Int_t TdcRes  =   15;
const Int_t Reset   =  100;
const Int_t Gate    = 1500;
const Int_t TdcOffset  =  300;
//ofstream testfile("cosmicchi2.txt");
int main(int argc,char *argv[]){
  //ofstream testfile("cosmicchi2.txt");
  char buff1[200],buff2[200];
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  root_setup froot_setup;
  INGRID_Dimension *fINGRID_Dimension = new INGRID_Dimension();

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



  TFile*    f          = new TFile(buff1,"read");
  TTree*    IngTree    = (TTree*)f->Get("IngTree");
  Int_t     Nevent     = IngTree->GetEntries();
  IngEvt*   fIngEvt    = new IngEvt();
  TCanvas*  c1         = new TCanvas("c1","c1",10,10,700,700);
  IngTree->SetBranchAddress("IngEvt",&fIngEvt);

  Int_t Cycle;
  Int_t TdcStart,TdcStop;


  //IngHit *fHit = new IngHit();;
  for(int n=0;n<Nevent;n++){
    if(n%1000==0)cout<<"event "<<n<<endl;
    IngTree           -> GetEntry(n);
    Cycle      =  fIngEvt->Cycle;

    Int_t  TdcStart   = (Gate+Reset)* Cycle+TdcOffset;
    Int_t  TdcStop    = (Gate+Reset)*(Cycle+1)+TdcOffset-Reset;
    //TdcStart   = -3000;
    //TdcStop    = -500;

    //for(Int_t nummod = 0; nummod<14; nummod++){
    for(Int_t nummod = 0; nummod<1; nummod++){
    //for(Int_t nummod = 6; nummod<7; nummod++){if(Cycle==19){
      ModEvt        fModEvt    =  fIngEvt->fModEvt[nummod];


      TClonesArray* arrx        = new TClonesArray("IngHit");
      arrx                      = fModEvt.fXHits;  
      Int_t nhitx       = fModEvt.NHitX;     
      Bool_t      HitX[nhitx];
      for(Int_t BTime  = TdcStart;
	        BTime <= TdcStop ;
	        BTime  = BTime + TdcStep){
      ///////////////////////////////X layer//////////////////////////////////
 	for(Int_t i=0;i<nhitx;i++){HitX[i]=false;}
	Int_t NHit=0;
	for(Int_t i=0;i<nhitx;i++){//All Hit
	  IngHit *fHit = (IngHit*)(arrx->At(i));
	  Long_t time  = fHit->Time;
	  if(fabs(BTime-time)<TdcRes)NHit++;
	}
	double x[NHit],z[NHit];
	int temp_nhitx=0;
	if(NHit>=3){
	  cout<<"--------event:"<<n<<" X View-----------"<<endl;
	  for(Int_t i=0;i<nhitx;i++){//All Hit
	    IngHit *fHit = (IngHit*)(arrx->At(i));
	    Long_t time  = fHit->Time;
	    if(fabs(BTime-time)<TdcRes){
	      HitX[i]=true;
	      Int_t pln    = fHit->Pln;
	      Int_t ch     = fHit->Ch;
	      Int_t adc    = fHit->HighAdc;
	      fINGRID_Dimension->get_posXY(nummod,XLayer,pln,ch,&x[temp_nhitx],&z[temp_nhitx]);
	      temp_nhitx++;
	      cout<<"pln:"<<pln<<" ch:"<<ch<<" adc:"<<adc<<endl;
	    }
	  }
	  //Fit
	  TGraph *track = new TGraph(NHit,z,x);
	  TF1 *pol1 = new TF1("pol1","pol1",0,200);
	  track->Fit("pol1","qn","",0,200);
	  Double_t chi2 = pol1->GetChisquare();
	  Double_t ndf = pol1->GetNDF();
	  if(chi2/ndf<20){
	    track->SetMarkerStyle(3);
	    track->SetMarkerSize(1.5);
	    track->SetMarkerColor(2);
	    TH1F *frame = gPad->DrawFrame(0,-10,100,130);
	    track->Draw("P");
	    cout<<"chi2/ndf="<<chi2<<"/"<<ndf<<"="<<chi2/ndf<<endl;
	    pol1->Draw("sames");
	    c1->Update();
	    cin.get();
	  }
	  delete track;
	  delete pol1;
	}//NHit>=3


      }//Tdc Step
      arrx->Delete("IngEvt");
      arrx->Clear("IngEvt");
      arrx=0;
      //Y layer
    }//nummod
      //} }//nummod and Cycle(for test )
  }//nevent


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




  f->Close();

}
 
