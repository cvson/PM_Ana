//#define TEST 1
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

const int nYEA = 40;
const int nMON = 12;
const int nDAY = 31;
const int nMOD = 14;


int    nSPILL[nYEA][nMON][nDAY];
int    iNEVT[nYEA][nMON][nDAY][nMOD];
int    iDMU [nYEA][nMON][nDAY][nMOD];
double iPOT[nYEA][nMON][nDAY];
double iMUMON[nYEA][nMON][nDAY];
double MUXxPOT [nYEA][nMON][nDAY];
double MUYxPOT [nYEA][nMON][nDAY];
double MUX [nYEA][nMON][nDAY];
double MUY [nYEA][nMON][nDAY];

void  clear(){

  for(int iyea=0; iyea<nYEA; iyea++){
    for(int imon=0; imon<nMON; imon++){
      for(int iday=0; iday<nDAY; iday++){
	for(int imod=0; imod<nMOD; imod++){
	  iNEVT[iyea][imon][iday][imod]=0;
	}
	nSPILL[iyea][imon][iday]=0;
	iPOT[iyea][imon][iday]=0;
	iMUMON[iyea][imon][iday]=0;
	MUX[iyea][imon][iday]=0;
	MUY[iyea][imon][iday]=0;
	MUXxPOT[iyea][imon][iday]=0;
	MUXxPOT[iyea][imon][iday]=0;
      }
    }
  }
}

void get_time(int t, int& yea, int& mon, int& mday){
  time_t aclock = t;
  struct tm *newtime;
  newtime = localtime(&aclock);
  mday = newtime -> tm_mday;
  mon  = newtime -> tm_mon;
  yea  = newtime -> tm_year+1900-2000;
#ifdef TEST
  yea  = 1;
  mday = 1;
  mon  = 1;
#endif
}

void  fill_beam(int utime){
  int y,m,d;
  get_time(utime, y, m, d);
  nSPILL[y][m][d]++;
  iPOT[y][m][d]     += beaminfo->ct_np[4][0];
  iMUMON[y][m][d]   += beaminfo->mumon[0];
  MUX [y][m][d]     += beaminfo->mumon[2];
  MUY [y][m][d]     += beaminfo->mumon[4];
  MUXxPOT [y][m][d] += beaminfo->mumon[2] * beaminfo->ct_np[4][0];
  MUYxPOT [y][m][d] += beaminfo->mumon[4] * beaminfo->ct_np[4][0];
}
void  fill_ing(int utime){
  int y,m,d;
  get_time(utime,y, m, d);
  if(ingbasic->hastrk && ingbasic->matchtrk && 
     //ingbasic->ontime &&
     !(ingbasic->vetowtracking) &&
     !(ingbasic->edgewtracking) ){
    
    int mod=ingbasic->hitmod;
    if(mod<14)
      iNEVT[y][m][d][mod]++;
  }

  if(ingbasic->hastrk && ingbasic->matchtrk && 
     //ingbasic->ontime && 
     ( (ingbasic->vetowtracking) || (ingbasic->edgewtracking) ) ){
    
    int mod=ingbasic->hitmod;
    if(mod<14)
      iDMU[y][m][d][mod]++;
  }


}


char Output[300];
void out(){
  ofstream wfile(Output);
  for(int iyea=0; iyea<nYEA; iyea++){
    for(int imon=0; imon<nMON; imon++){
      for(int iday=0; iday<nDAY; iday++){
	if(nSPILL[iyea][imon][iday]==0)continue;
	
	wfile << imon << "\t" 
	      << iday << "\t" 
	      << nSPILL[iyea][imon][iday] << "\t" 
	      << iPOT[iyea][imon][iday] << "\t"
	      << iMUMON[iyea][imon][iday] << "\t"
	      << MUXxPOT[iyea][imon][iday] << "\t"
	      << MUYxPOT[iyea][imon][iday] << "\t" ;
	for(int imod=0; imod<nMOD; imod++){
	  wfile << iNEVT[iyea][imon][iday][imod] << "\t";
	}
	for(int imod=0; imod<nMOD; imod++){
	  wfile << iDMU[iyea][imon][iday][imod] << "\t";
	}
	wfile << endl;
      }
    }
  }
}

int main(int argc,char *argv[]){
  TROOT        root ("GUI","GUI");
  TApplication app  ("App",0,0);

  Int_t c=-1;  char FileName[300]; 
  sprintf(Output,  "nevent.txt");
  sprintf(FileName,"temp.root");
  while ((c = getopt(argc, argv, "f:o:")) != -1) {
    switch(c){
    case 'f':
      sprintf(FileName,"%s",optarg);
      break;
    case 'o':
      sprintf(Output,"%s",optarg);
      break;
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

  clear();
  for(int ievt=0; ievt<nevt; ievt++){
    //for(int ievt=0; ievt<30000; ievt++){
    if(ievt%10000==0)cout << ievt << endl;
    evt       -> Clear();
    tree      -> GetEntry(ievt);
    beaminfo  = (BeamInfoSummary*)(evt->GetBeamSummary(0));
    int utime = beaminfo->trg_sec;
    //utime -= 3600 * 9;
    fill_beam(utime);
    for(int ibasic = 0; ibasic < evt->NIngridBasicRecons(); ibasic++){
      ingbasic     = (IngridBasicReconSummary*)(evt->GetBasicRecon(ibasic));
      fill_ing(utime);
    }
  }//ievt
  out();

}
 
