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
#include <TString.h>
#include <TSystem.h>

#include "setup.hxx"

int main(int argc,char *argv[]){
  TROOT root("GUI","GUI");
  TApplication theApp("App",&argc,argv);
  char buff1[300],buff2[300];

  cout<<"run number of new file?"<<endl;
  Int_t new_run;
  cin>>new_run;
  sprintf(buff1,"%s/ingrid_%08d_00ly.daq.mid.pe.new.root",cosmic_LY_folder,new_run);
  TFile *f_new = new TFile(buff1,"recreate");
  TTree *IngEvt_new = new TTree("IngEvt","IngEvt");
  Double_t pe[NumMod][NumTFB][NumCh][NumCyc];
  Long_t Tdc[NumMod][NumTFB][NumCh][NumCyc];
  sprintf(buff1,"pe[%d][%d][%d][%d]",NumMod,NumTFB,NumCh,NumCyc);
  sprintf(buff2,"pe[%d][%d][%d][%d]/D",NumMod,NumTFB,NumCh,NumCyc);
  IngEvt_new->Branch(buff1,pe,buff2);
  sprintf(buff1,"Tdc[%d][%d][%d][%d]",NumMod,NumTFB,NumCh,NumCyc);
  sprintf(buff2,"Tdc[%d][%d][%d][%d]/L",NumMod,NumTFB,NumCh,NumCyc);
  IngEvt_new->Branch(buff1,Tdc,buff2);

  while(1){
    cout<<"run number?"<<endl;
    Int_t run_number;
    cin>>run_number;
    if(run_number==0)break;
    char FileName[300];
    sprintf(FileName,"%s/ingrid_%08d_0000.daq.mid.pe.new.root",root_file_pe_folder,run_number);
    FileStat_t fs;
    cout<<FileName<<endl;
    if(gSystem->GetPathInfo(FileName,fs)){
      cout<<"Cannot open file "<<FileName<<endl;
      exit(1);
    }
    TFile *f = new TFile(FileName,"read");
    TTree *IngEvt = (TTree*)f->Get("IngEvt");
    sprintf(buff1,"pe[%d][%d][%d][%d]",NumMod,NumTFB,NumCh,NumCyc);
    IngEvt->SetBranchAddress(buff1,pe);
    sprintf(buff1,"Tdc[%d][%d][%d][%d]",NumMod,NumTFB,NumCh,NumCyc);
    IngEvt->SetBranchAddress(buff1,Tdc);
    Int_t Nevent = IngEvt->GetEntries();
    cout<<"all event "<<Nevent<<endl;
    cout<<"Fill... "<<endl;
    for(Int_t nevent=0;nevent<Nevent;nevent++){
      if((nevent+1)%1000==0)cout<<nevent+1<<"\t end"<<endl;
      IngEvt->GetEntry(nevent);
      IngEvt_new->Fill();
    }//event
    f->Close();

  }

  IngEvt_new->Write();
  f_new->Write();
  f_new->Close();


}
