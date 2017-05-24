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
#include <TSpectrum.h>
#include <TF1.h>
#include <TMath.h>
#include <TSpectrum.h>

#include "TApplication.h"
#include "setup.hxx"
#include "plot.cxx"

int main(){
  char buff1[200];
  sprintf(buff1,"%s/beam_1.root",beam_folder);
  TFile *f = new TFile(buff1,"read");
  TTree *IngEvt = (TTree*)f->Get("IngEvt");

  Double_t pe[NumMod][NumTFB][NumCh][NumCyc];
  Long_t Tdc[NumMod][NumTFB][NumCh][NumCyc];

  sprintf(buff1,"pe[%d][%d][%d][%d]",NumMod,NumTFB,NumCh,NumCyc);
  IngEvt->SetBranchAddress(buff1,pe);
  sprintf(buff1,"Tdc[%d][%d][%d][%d]",NumMod,NumTFB,NumCh,NumCyc);
  IngEvt->SetBranchAddress(buff1,Tdc);
  Int_t Nevent=IngEvt->GetEntries();

  plot h1(0,0);
  for(Int_t nevent=0;nevent<Nevent;nevent++){
    
    if((nevent+1)%1000==0)cout<<nevent+1<<"\t end"<<endl;
    IngEvt->GetEntry(nevent);
    cout<<"event"<<nevent<<endl;
    if(nevent==26)h1.evdisp(&pe[0][0][0][0]);
  }
}
