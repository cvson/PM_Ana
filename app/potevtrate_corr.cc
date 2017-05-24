#define TEST 1
//##### Standard C++ lib. ######
#include<iostream>
#include<sstream>
#include<fstream>
using namespace std;
#include <iomanip.h>
#include <sys/stat.h>
#include "math.h"
//##### Root Library ###########
#include <TROOT.h>
#include <TStyle.h>
#include <TH1.h>
#include <TH2.h>
#include <TApplication.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TClonesArray.h>
#include <TObject.h>
#include <TEventList.h>
#include <TBranch.h>
#include <TSystem.h>
#include <TBrowser.h>
//##### INGRID Library #########
#include "INGRIDEVENTSUMMARY.h"
#include "IngridHitSummary.h"
#include "IngridSimHitSummary.h"
#include "IngridBasicReconSummary.h"
#include "BeamInfoSummary.h"
IngridBasicReconSummary* ingbasic;
BeamInfoSummary*         beaminfo;



ofstream outfile("evtrate.txt");

double corr[14];//correction factor

#define POTBIN 70
#define POTMAX 1.4e13

double npot[POTBIN];
double nnu[14][POTBIN];
int nmu[14][POTBIN],inttmp;

TH1F *hnu = new TH1F("hnu","hnu",POTBIN,0,POTMAX);
TH1F *hmu = new TH1F("hmu","hmu",POTBIN,0,POTMAX);
TH1F *hpot = new TH1F("hpot","hpot",POTBIN,0,POTMAX);

/*
const int nMON = 12;
const int nDAY = 31;
const int nMOD = 14;


int    nSPILL[nMON][nDAY];
int    iNEVT[nMON][nDAY][nMOD];
int    iDMU [nMON][nDAY][nMOD];
double iPOT[nMON][nDAY];
double iMUMON[nMON][nDAY];
double MUXxPOT [nMON][nDAY];
double MUYxPOT [nMON][nDAY];
double MUX [nMON][nDAY];
double MUY [nMON][nDAY];

void  clear(){

  for(int imon=0; imon<nMON; imon++){
    for(int iday=0; iday<nDAY; iday++){
      for(int imod=0; imod<nMOD; imod++){
	iNEVT[imon][iday][imod]=0;
      }
      nSPILL[imon][iday]=0;
      iPOT[imon][iday]=0;
      iMUMON[imon][iday]=0;
      MUX[imon][iday]=0;
      MUY[imon][iday]=0;
      MUXxPOT[imon][iday]=0;
      MUXxPOT[imon][iday]=0;
    }
  }
}
*/


void  fill_beam(){
  double pot;
  for(int i=0;i<8;i++){
    pot=beaminfo->ct_np[4][i+1];
    hpot->Fill(pot,pot);

    inttmp=pot/POTMAX*POTBIN;
    npot[inttmp]+=pot;

  }
 

  /*
  int m,d;
  get_time(utime, m, d);
  nSPILL[m][d]++;
  iPOT[m][d]     += beaminfo->ct_np[4][0];
  iMUMON[m][d]   += beaminfo->mumon[0];
  MUX [m][d]     += beaminfo->mumon[2];
  MUY [m][d]     += beaminfo->mumon[4];
  MUXxPOT [m][d] += beaminfo->mumon[2] * beaminfo->ct_np[4][0];
  MUYxPOT [m][d] += beaminfo->mumon[4] * beaminfo->ct_np[4][0];
  */
}
void  fill_ing(){

  double pot[8];
  for(int i=0;i<8;i++)
    pot[i]=beaminfo->ct_np[4][i+1];


  //int m,d;
  //get_time(utime, m, d);



  if(ingbasic->hastrk && ingbasic->matchtrk && 
     //ingbasic->ontime && !(ingbasic->vetowtracking) &&
     !(ingbasic->vetowtracking) &&
     !(ingbasic->edgewtracking) ){
    
    int mod=ingbasic->hitmod;
    int cyc=ingbasic->hitcyc;
    if(mod<14&&cyc>=4&&cyc<12){
      hnu->Fill(pot[cyc-4],(double)1/(1-(double)pot[cyc-4]*corr[mod]));
      inttmp=pot[cyc-4]/POTMAX*POTBIN;
      nnu[mod][inttmp]+=(double)1/(1-(double)pot[cyc-4]*corr[mod]);
    }

      //iNEVT[m][d][mod]++;
  }

  if(ingbasic->hastrk && ingbasic->matchtrk && 
     //ingbasic->ontime && 
     ( (ingbasic->vetowtracking) || (ingbasic->edgewtracking) ) ){
    
    int mod=ingbasic->hitmod;
    int cyc=ingbasic->hitcyc;
    if(mod<14&&cyc>=4&&cyc<12){
      hmu->Fill(pot[cyc-4]);
      inttmp=pot[cyc-4]/POTMAX*POTBIN;
      nmu[mod][inttmp]++;

    }
      //iDMU[m][d][mod]++;
  }


}



void out(){
  /*
  ofstream wfile(Output);
  for(int imon=0; imon<nMON; imon++){
    for(int iday=0; iday<nDAY; iday++){
      if(nSPILL[imon][iday]==0)continue;

      wfile << imon << "\t" 
	    << iday << "\t" 
	    << nSPILL[imon][iday] << "\t" 
	    << iPOT[imon][iday] << "\t"
	    << iMUMON[imon][iday] << "\t"
	    << MUXxPOT[imon][iday] << "\t"
	    << MUYxPOT[imon][iday] << "\t" ;
      for(int imod=0; imod<nMOD; imod++){
	wfile << iNEVT[imon][iday][imod] << "\t";
      }
      for(int imod=0; imod<nMOD; imod++){
	wfile << iDMU[imon][iday][imod] << "\t";
      }
      wfile << endl;
    }
  }
  */
  //TH1F *hrat= new TH1F(((*hmu)+(*hnu))/(*hpot));

  for(int ibin=0; ibin<POTBIN; ibin++){
    outfile<<npot[ibin]<<endl;
  }
  for(int imon=0; imon<14; imon++){
    outfile<<endl;
    for(int ibin=0; ibin<POTBIN; ibin++){
      outfile<<nnu[imon][ibin]<<endl;
    }
  }


  TH1F *hrat= new TH1F("hrat","hrat",POTBIN,0,POTMAX);
  hrat->Divide(hnu,hpot);
  for(int i=0;i<POTBIN;i++){
    if(hpot -> GetBinContent(i+1)==0)
      hrat->SetBinError(i+1,0);
    else
      hrat->SetBinError(i+1,sqrt( hnu->GetBinContent(i+1) )/hpot -> GetBinContent(i+1));
  }
  TCanvas *c1= new TCanvas("c1","c1",0,0,600,400);
  hrat->Draw("E");
  //hmu->Draw();
  c1->Print("evtrate.C");
}

int main(int argc,char *argv[]){
  TROOT        root ("GUI","GUI");
  TApplication app  ("App",0,0);


  /***Correction factor***/
  corr[0]  = 3.18e-15;
  corr[1]  = 4.25e-15;
  corr[2]  = 5.15e-15;
  corr[3]  = 5.48e-15;
  corr[4]  = 5.51e-15;
  corr[5]  = 4.67e-15;
  corr[6]  = 3.31e-15;
  corr[7]  = 3.21e-15;
  corr[8]  = 4.39e-15;
  corr[9]  = 5.42e-15;
  corr[10] = 6.06e-15;
  corr[11] = 5.48e-15;
  corr[12] = 4.31e-15;
  corr[13] = 3.17e-15;
  /**********************/

  for(int index=0;index<14;index++)
    corr[index]=0.975e-15;
  //corr[index]=4.06e-15;


 memset(nnu,0,sizeof(nnu));
 memset(nmu,0,sizeof(nmu));
 memset(npot,0,sizeof(npot));

  Int_t c=-1;  char FileName[300]; 
  //sprintf(Output,  "nevent.txt");
  sprintf(FileName,"temp.root");
  while ((c = getopt(argc, argv, "f:")) != -1) {
    switch(c){
    case 'f':
      sprintf(FileName,"%s",optarg);
      break;
      //case 'o':
      //sprintf(Output,"%s",optarg);
      //break;
    }
  }

  FileStat_t fs;
  cout<<"reading and processsing"<<FileName<<"....."<<endl;
  if(gSystem->GetPathInfo(FileName,fs)){
    cout<<"Cannot open file "<<FileName<<endl;
    exit(1);
  }

  //#### Read ROOT File after basic reconstruction ######
  //#####################################################
  TFile*            rfile = new TFile(FileName,"read");
  TTree*             tree = (TTree*)rfile -> Get("tree");
  TBranch*          EvtBr = tree->GetBranch("fDefaultReco.");
  IngridEventSummary* evt = new IngridEventSummary();
  EvtBr                   ->SetAddress(&evt);
  tree                    ->SetBranchAddress("fDefaultReco.", &evt);
  int                nevt = (int)tree -> GetEntries();

  //clear();
  for(int ievt=0; ievt<nevt; ievt++){
    //for(int ievt=0; ievt<30000; ievt++){
    if(ievt%10000==0)cout << ievt << endl;
    evt       -> Clear();
    tree      -> GetEntry(ievt);
    beaminfo  = (BeamInfoSummary*)(evt->GetBeamSummary(0));
    //int utime = beaminfo->trg_sec;
    //utime -= 3600 * 9;
    fill_beam();
    for(int ibasic = 0; ibasic < evt->NIngridBasicRecons(); ibasic++){
      ingbasic     = (IngridBasicReconSummary*)(evt->GetBasicRecon(ibasic));
      fill_ing();
    }
  }//ievt
  out();

}
