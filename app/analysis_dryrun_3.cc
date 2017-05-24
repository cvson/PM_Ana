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
ofstream testfile("dryadc.txt");
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
  int anaMod=-1;
  int anaCycle=-1;
  bool cosmicflag=false;
  while ((c = getopt(argc, argv, "r:h:c:m:t")) != -1) {
    switch(c){
    case 'r':
      run_number=atoi(optarg);
      break;
    case 'c':
      anaCycle=atoi(optarg);
      break;
    case 'm':
      anaMod=atoi(optarg);
      break;
    case 't':
      cosmicflag=true;
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
  TCanvas*  cx         = new TCanvas("cx","cx",10,10,700,700);
  TCanvas*  cy         = new TCanvas("cy","cy",10,10,700,700);
  IngTree->SetBranchAddress("IngEvt",&fIngEvt);

  Int_t Cycle;
  Int_t TdcStart,TdcStop;
  TGraph *trackx,*tracky;
  TF1 *pol1x,*pol1y;


  //IngHit *fHit = new IngHit();;
  //for(int n=0;n<Nevent;n++){
  for(int n=0;n<Nevent;n++){
    if(n==56948||
       n==41036||
       n==695||
       n==15760||
       n==20361||
       n==59714||
       n==59095||
       n==99992||
       n==39391||
       n==40312||
       n==47097||
       n==70948||
       n==105287||
       n==9447||
       n==58070||
       n==10323||
       n==96965||
       n==64950||
       n==42457
       ){
       
    //if(n%1000==0)cout<<"event "<<n<<endl;
    IngTree           -> GetEntry(n);
    Cycle      =  fIngEvt->Cycle;
    Int_t TdcStart,TdcStop;

    TdcStart   = (Gate+Reset)* Cycle+TdcOffset;
    TdcStop    = (Gate+Reset)*(Cycle+1)+TdcOffset-Reset;
    
    if(cosmicflag){
      TdcStart   = -3000;
      TdcStop    = -500;
    }

    for(Int_t nummod = 0; nummod<14; nummod++){
    //for(Int_t nummod = 2; nummod<3; nummod++){
    //for(Int_t nummod =2; nummod<3; nummod++){if(Cycle==0){
      if(nummod==anaMod){
    //if(Cycle==anaCycle){
      ModEvt        fModEvt    =  fIngEvt->fModEvt[nummod];



      TClonesArray* arrx        = new TClonesArray("IngHit");
      arrx                      = fModEvt.fXHits;  
      Int_t nhitx       = fModEvt.NHitX; 
      Bool_t      HitX[nhitx];    
      TClonesArray* arry        = new TClonesArray("IngHit");
      arry                      = fModEvt.fYHits;  
      Int_t nhity       = fModEvt.NHitY;     
      Bool_t      HitY[nhity];

      TClonesArray* arrxveto        = new TClonesArray("IngHit");
      arrxveto                      = fModEvt.fXVetoHits;  
      Int_t nhitxveto       = fModEvt.NHitXVeto; 
      TClonesArray* arryveto        = new TClonesArray("IngHit");
      arryveto                      = fModEvt.fYVetoHits;  
      Int_t nhityveto       = fModEvt.NHitYVeto;     
 



      for(Int_t BTime  = TdcStart;
	        BTime <= TdcStop ;
	        BTime  = BTime + TdcStep){
	double chi2x=1000;
	double chi2y=1000;
	double ndfx=1;
	double ndfy=1;
      ///////////////////////////////X layer//////////////////////////////////
 	for(Int_t i=0;i<nhitx;i++){HitX[i]=false;}
	Int_t NHitX=0;
	Bool_t flagx=true;
	int temp;
	for(Int_t i=0;i<nhitx;i++){//All Hit
	  IngHit *fHit = (IngHit*)(arrx->At(i));
	  Long_t time  = fHit->Time;
	  if(fabs(BTime-time)<TdcRes){
	    NHitX++;
	    if(NHitX==1){
	      temp=fHit->Pln;
	    }
	    else{
	      if(temp!=fHit->Pln)flagx=false;
	    }
	  }
	}

	double x[NHitX],z[NHitX];
	int temp_nhitx=0;
	//if(NHitX>=3&&!flagx){
	if(NHitX>=3&&!flagx){
	  //cout<<"--------event:"<<n<<" X View-----------"<<endl;
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
	      //cout<<"pln:"<<pln<<" ch:"<<ch<<" adc:"<<adc<<endl;
	    }
	  }
	  //Fit
	  trackx = new TGraph(NHitX,z,x);
	  pol1x = new TF1("pol1x","pol1",-10,150);
	  trackx->Fit("pol1x","qn","",0,200);
	  chi2x = pol1x->GetChisquare();
	  ndfx = pol1x->GetNDF();
	}//NHit>=3

      ///////////////////////////////Y layer//////////////////////////////////

 	for(Int_t i=0;i<nhity;i++){HitY[i]=false;}
	Int_t NHitY=0;

	Bool_t flagy=true;
	for(Int_t i=0;i<nhity;i++){//All Hit
	  IngHit *fHit = (IngHit*)(arry->At(i));
	  Long_t time  = fHit->Time;
	  if(fabs(BTime-time)<TdcRes){
	    NHitY++;
	    if(NHitY==1){
	      temp=fHit->Pln;
	    }
	    else{
	      if(temp!=fHit->Pln)flagy=false;
	    }
	  }

	}
	double y[NHitY],yz[NHitY];
	int temp_nhity=0;
	//if(NHitY>=3&&!flagy){
	if(NHitY>=3&&!flagy){
	  //cout<<"--------event:"<<n<<" Y View-----------"<<endl;
	  for(Int_t i=0;i<nhity;i++){//All Hit
	    IngHit *fHit = (IngHit*)(arry->At(i));
	    Long_t time  = fHit->Time;
	    if(fabs(BTime-time)<TdcRes){
	      HitY[i]=true;
	      Int_t pln    = fHit->Pln;
	      Int_t ch     = fHit->Ch;
	      Int_t adc    = fHit->HighAdc;
	      fINGRID_Dimension->get_posXY(nummod,YLayer,pln,ch,&y[temp_nhity],&yz[temp_nhity]);
	      temp_nhity++;
	      //cout<<"pln:"<<pln<<" ch:"<<ch<<" adc:"<<adc<<endl;
	    }
	  }
	  //Fit
	  tracky=new TGraph(NHitY,yz,y);
	  pol1y=new TF1("pol1y","pol1",-10,150);
	  tracky->Fit("pol1y","qn","",-10,150);
	  chi2y = pol1y->GetChisquare();
	  ndfy = pol1y->GetNDF();



	}//NHit>=3


	//if(NHitX>=3&&NHitY>=3&&chi2x/ndfx>100&&chi2y/ndfy<20){
	if(NHitX>=3&&NHitY>=3&&chi2x/ndfx<20&&chi2y/ndfy<20){
	  cout<<"--------event:"<<n<<"-----------"<<endl;
	  testfile<<"--------event:"<<n<<endl;
	  for(Int_t i=0;i<nhitx;i++){//All Hit
	    IngHit *fHit = (IngHit*)(arrx->At(i));
	    Long_t time  = fHit->Time;
	    if(fabs(BTime-time)<TdcRes){
	      HitX[i]=true;
	      Int_t pln    = fHit->Pln;
	      Int_t ch     = fHit->Ch;
	      Int_t adc    = fHit->HighAdc;
	      testfile<<adc<<endl;
	      cout<<"Xpln:"<<pln<<" ch:"<<ch<<" adc:"<<adc<<endl;
	    }
	  }


	  for(Int_t i=0;i<nhity;i++){//All Hit
	    IngHit *fHit = (IngHit*)(arry->At(i));
	    Long_t time  = fHit->Time;
	    if(fabs(BTime-time)<TdcRes){
	      HitY[i]=true;
	      Int_t pln    = fHit->Pln;
	      Int_t ch     = fHit->Ch;
	      Int_t adc    = fHit->HighAdc;
	      testfile<<adc<<endl;
	      cout<<"Ypln:"<<pln<<" ch:"<<ch<<" adc:"<<adc<<endl;
	    }
	  }


	  for(Int_t i=0;i<nhitxveto;i++){
	    IngHit *fHit = (IngHit*)(arrxveto->At(i));
	    Long_t time  = fHit->Time;
	    if(fabs(BTime-time)<TdcRes){
	      Int_t pln    = fHit->Pln;
	      Int_t ch     = fHit->Ch;
	      Int_t adc    = fHit->HighAdc;
	      testfile<<adc<<endl;
	      cout<<"XVeto:"<<pln<<" ch:"<<ch<<" adc"<<adc<<endl;
	    }
	  }
	  for(Int_t i=0;i<nhityveto;i++){
	    IngHit *fHit = (IngHit*)(arryveto->At(i));
	    Long_t time  = fHit->Time;
	    if(fabs(BTime-time)<TdcRes){
	      Int_t pln    = fHit->Pln;
	      Int_t ch     = fHit->Ch;
	      Int_t adc    = fHit->HighAdc;
	      testfile<<adc<<endl;
	      cout<<"YVeto:"<<pln<<" ch:"<<ch<<" adc"<<adc<<endl;
	    }
	  }

	  cx->cd();
	  trackx->SetMarkerStyle(3);
	  trackx->SetMarkerSize(1.5);
	  trackx->SetMarkerColor(2);
	  TH1F *framex = gPad->DrawFrame(-10,-10,130,130);
	  trackx->Draw("P");
	  cout<<"chi2/ndf="<<chi2x<<"/"<<ndfx<<"="<<chi2x/ndfx<<endl;
	  pol1x->Draw("sames");
	  cx->Update();
	  cy->cd();
	  tracky->SetMarkerStyle(3);
	  tracky->SetMarkerSize(1.5);
	  tracky->SetMarkerColor(2);
	  TH1F *framey = gPad->DrawFrame(-10,-10,130,130);
	  tracky->Draw("P");
	  cout<<"chi2/ndf="<<chi2y<<"/"<<ndfy<<"="<<chi2y/ndfy<<endl;
	  pol1y->Draw("sames");
	  cy->Update();
	  cin.get();
	}

	//delete trackx;
	//delete pol1x;
	//delete tracky;
	//delete pol1y;
      }//Tdc Step
      arrx->Delete("IngEvt");
      arrx->Clear("IngEvt");
      arrx=0;
      delete arrx;
      arry->Delete("IngEvt");
      arry->Clear("IngEvt");
      arry=0;      
      delete arry;

      //}//nummod
    }} }//nummod and Cycle(for test )
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
 
