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
IngridSimVertexSummary*  simver;

#define POTBIN 70
#define POTMAX 1.4e13

TH1F *hnu = new TH1F("hnu","hnu",POTBIN,0,POTMAX);
TH1F *hmu = new TH1F("hmu","hmu",POTBIN,0,POTMAX);
TH1F *hpot = new TH1F("hpot","hpot",POTBIN,0,POTMAX);

int neut[3][14];
int bg[3][14];
//float case001[14],case100[14],case010[14],case111[14],case110[14];
double ndiff[14][21];
float weight;

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

/*
void  fill_beam(){
  double pot;
  for(int i=0;i<8;i++){
    pot=beaminfo->ct_np[4][i+1];
    hpot->Fill(pot,pot);
  }
  int m,d;
  get_time(utime, m, d);
  nSPILL[m][d]++;
  iPOT[m][d]     += beaminfo->ct_np[4][0];
  iMUMON[m][d]   += beaminfo->mumon[0];
  MUX [m][d]     += beaminfo->mumon[2];
  MUY [m][d]     += beaminfo->mumon[4];
  MUXxPOT [m][d] += beaminfo->mumon[2] * beaminfo->ct_np[4][0];
  MUYxPOT [m][d] += beaminfo->mumon[4] * beaminfo->ct_np[4][0];

}
*/

void  fill_ing(int fileid){

  //double pot[8];
  //for(int i=0;i<8;i++)
  //pot[i]=beaminfo->ct_np[4][i+1];
  //int m,d;
  //get_time(utime, m, d);


  if(ingbasic->hastrk && ingbasic->matchtrk
     /*ingbasic->ontime &&*/  ){
    int mod=ingbasic->hitmod;
    if(mod<14){

      if(!(ingbasic->vetowtracking) &&
	 !(ingbasic->edgewtracking))
	neut[fileid][mod]++;
      else
	bg[fileid][mod]++;
    }

  }
  
  /*
  if(ingbasic->hastrk && ingbasic->matchtrk && 
     ingbasic->ontime && 
     ( (ingbasic->vetowtracking) || (ingbasic->edgewtracking) ) ){
    
    int mod=ingbasic->hitmod;
    int cyc=ingbasic->hitcyc;
    if(mod<14&&cyc>=4&&cyc<12)
      hmu->Fill(pot[cyc-4]);
      //iDMU[m][d][mod]++;
  }
  */

}


void check(){
  for(int mod=0;mod<14;mod++){
    //if(neut[1][mod]!=1||neut[2][mod]!=1)continue;
    if(neut[1][mod]!=1||neut[2][mod]!=0)continue;
    if(bg[1][mod]!=0  ||bg[2][mod]!=1)continue;
    int diff=neut[1][mod]+neut[2][mod]-neut[0][mod];
    if(diff<-10)diff=-10;
    else if(diff>10)diff=10;
    ndiff[mod][diff+10]+=weight;

    /*
    if     (!neut[1][mod]&&!neut[2][mod]&& neut[0][mod])case001[mod]+=weight;
    else if( neut[1][mod]&&!neut[2][mod]&&!neut[0][mod])case100[mod]+=weight;
    else if(!neut[1][mod]&& neut[2][mod]&&!neut[0][mod])case010[mod]+=weight;
    else if( neut[1][mod]&& neut[2][mod]&& neut[0][mod])case111[mod]+=weight;
    else if( neut[1][mod]&& neut[2][mod]&&!neut[0][mod])case110[mod]+=weight;
    */
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
  /*
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
  */
  ofstream outfile("pileup_tmp.txt");

  for(int mod=0;mod<14;mod++){
    for(int i=0;i<21;i++){
      outfile<<ndiff[mod][i]<<endl;
    }
    outfile<<endl;

  }
}

int main(int argc,char *argv[]){
  TROOT        root ("GUI","GUI");
  TApplication app  ("App",0,0);

  Int_t c=-1;  char FileName[300], FileName1[300], FileName2[300];
  //sprintf(Output,  "nevent.txt");
  sprintf(FileName,"temp.root");
  while ((c = getopt(argc, argv, "f:a:b:")) != -1) {
    switch(c){
    case 'f':
      sprintf(FileName,"%s",optarg);
      break;
    case 'a':
      sprintf(FileName1,"%s",optarg);
      break;
    case 'b':
      sprintf(FileName2,"%s",optarg);
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


  memset(ndiff,0,sizeof(ndiff));

  /*
  memset(case001,0,sizeof(case001));
  memset(case100,0,sizeof(case100));
  memset(case010,0,sizeof(case010));
  memset(case111,0,sizeof(case111));
  memset(case110,0,sizeof(case110));
  */

  //#### Read ROOT File after basic reconstruction ######
  //#####################################################
  TFile*            rfile1 = new TFile(FileName1,"read");
  TTree*             tree1 = (TTree*)rfile1 -> Get("tree");
  TBranch*          EvtBr1 = tree1->GetBranch("fDefaultReco.");
  IngridEventSummary* evt1 = new IngridEventSummary();
  EvtBr1                   ->SetAddress(&evt1);
  tree1                    ->SetBranchAddress("fDefaultReco.", &evt1);
  int                nevt1 = (int)tree1 -> GetEntries();

  TFile*            rfile2 = new TFile(FileName2,"read");
  TTree*             tree2 = (TTree*)rfile2 -> Get("tree");
  TBranch*          EvtBr2 = tree2->GetBranch("fDefaultReco.");
  IngridEventSummary* evt2 = new IngridEventSummary();
  EvtBr2                   ->SetAddress(&evt2);
  tree2                    ->SetBranchAddress("fDefaultReco.", &evt2);
  int                nevt2 = (int)tree2 -> GetEntries();

  TFile*            rfile = new TFile(FileName,"read");
  TTree*             tree = (TTree*)rfile -> Get("tree");
  TBranch*          EvtBr = tree->GetBranch("fDefaultReco.");
  IngridEventSummary* evt = new IngridEventSummary();
  EvtBr                   ->SetAddress(&evt);
  tree                    ->SetBranchAddress("fDefaultReco.", &evt);
  int                nevt = (int)tree -> GetEntries();


  if(nevt!=nevt1||nevt!=nevt2){
    cout<<"# of event is different"<<endl;
    exit(1);
  }

  //clear();
  for(int ievt=0; ievt<nevt; ievt++){
    memset(neut,0,sizeof(neut));
    memset(bg,0,sizeof(bg));
    //for(int ievt=0; ievt<30000; ievt++){
    if(ievt%10000==0)cout << ievt << endl;
    evt       -> Clear();
    evt1      -> Clear();
    evt2      -> Clear();
    tree      -> GetEntry(ievt);
    tree1     -> GetEntry(ievt);
    tree2     -> GetEntry(ievt);

    simver  = (IngridSimVertexSummary*)( evt -> GetSimVertex(0) );
    weight = (simver->norm)*(simver->totcrsne);
    //fill_beam();
    for(int ibasic = 0; ibasic < evt->NIngridBasicRecons(); ibasic++){
      ingbasic     = (IngridBasicReconSummary*)(evt->GetBasicRecon(ibasic));
      fill_ing(0);
    }
    for(int ibasic = 0; ibasic < evt1->NIngridBasicRecons(); ibasic++){
      ingbasic     = (IngridBasicReconSummary*)(evt1->GetBasicRecon(ibasic));
      fill_ing(1);
    }
    for(int ibasic = 0; ibasic < evt2->NIngridBasicRecons(); ibasic++){
      ingbasic     = (IngridBasicReconSummary*)(evt2->GetBasicRecon(ibasic));
      fill_ing(2);
    }

    check();
    
  }//ievt
  out();

}
