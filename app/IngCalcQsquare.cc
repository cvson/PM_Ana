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
#include <TArrayF.h>
#include <TObject.h>
#include <TEventList.h>
#include <TBranch.h>
#include <TSystem.h>
#include <TBrowser.h>
//##### INGRID Library #########
/*
#include "INGRIDEVENTSUMMARY.h"
#include "IngridHitSummary.h"
#include "IngridSimHitSummary.h"
#include "IngridBasicReconSummary.h"
#include "BeamInfoSummary.h"
*/

int main(int argc,char *argv[]){
  TROOT        root ("GUI","GUI");
  TApplication app  ("App",0,0);

  Int_t c=-1;
  char FileName[300], Output[300];

  sprintf(Output,"output.root");
  sprintf(FileName,"filename.root");

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
  TFile*            file = new TFile(FileName,"read");
  TTree*            tree = (TTree*)file -> Get("h1");
  int               mode;
  float             Pvc[100][3];

  tree  -> SetBranchAddress("mode", &mode);
  tree  -> SetBranchAddress("Pvc",  &Pvc);

  int               nevt = (int)tree -> GetEntries();


  float Qsq;
  TFile*            wfile = new TFile(Output, "recreate");
  TTree*            wtree = new TTree("tree","tree");
  wtree->Branch("Qsq",&Qsq,"Qsq/F");


  float Pmu[3],Pnu[3],Mmu,Enu,Emu;
  for(int ievt=0; ievt<nevt; ievt++){
    if(ievt%10000==0)cout << ievt << endl;
    tree        -> GetEntry(ievt);

    if(mode==0){
      Qsq = 0;
    }
    else{
      for(int i=0;i<3;i++){
	Pnu[i]=Pvc[0][i];
	Pmu[i]=Pvc[2][i];
      }
      if(mode<30)Mmu=105.658;
      else       Mmu=0;
      Enu = sqrt(Pnu[0]*Pnu[0]+Pnu[1]*Pnu[1]+Pnu[2]*Pnu[2]);
      Emu = sqrt(Pmu[0]*Pmu[0]+Pmu[1]*Pmu[1]+Pmu[2]*Pmu[2]+Mmu*Mmu);
      Qsq = (Pmu[0]-Pnu[0])*(Pmu[0]-Pnu[0])
	+   (Pmu[1]-Pnu[1])*(Pmu[1]-Pnu[1])
	+   (Pmu[2]-Pnu[2])*(Pmu[2]-Pnu[2])
	-   (Emu-Enu)*(Emu-Enu);
      Qsq = Qsq*1e-6;
    }

    wtree->Fill();

  }//ievt

  wtree  -> Write();
  wfile  -> Write();
  wfile  -> Close();


}
