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
#include <TBox.h>
#include <TLatex.h>
#include <TString.h>
#include <TSystem.h>

#include "TApplication.h"
#include "analysis_beam.hxx"
#include "setup.hxx"
#include "INGRID_Dimension.hxx"

#include "IngTree.hh"
//#include "root_setup.hxx"

vector<int>*       adc;
vector<double>*     pe;
vector<int>*      nsec;
vector<int>*      view;
vector<int>*       pln;
vector<int>*        ch;
vector<double>*  posxy;
vector<double>*   posz;
Int_t             fMod;
Long_t           fTime;
Long_t          fcTime; //with weightened by # of pe
Int_t            Cycle;
Int_t          nActPln;
Int_t          nActTPL; //TPL  means usual TPL 1 ~ 9
Int_t          nActVeto;//Veto means usual TPL 0, 10 and VETO
Float_t        fMeanPe;
Int_t           nSpill;
int                  n;
Int_t           NumEvt;
Int_t       Nhitclster;
int            counter;
int             MUpAct;
bool          UVETOHit;


int main(int argc,char *argv[]){
  


  char buff1[200],buff2[200];
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  Int_t run_number;
  Int_t c=-1;  char FileName[300];
  bool testflag=false;
  bool sflag = false;
  while ((c = getopt(argc, argv, "sr:ht")) != -1) {
    switch(c){
    case 's':
      sflag = true;
      break;
    case 'r':
      run_number=atoi(optarg);
      break;
    case 't':
      testflag=true;
      break;
    case 'h':
      cout<<"help menu"<<endl;
      cout<<"-r [run_number]"<<endl;
      break;
    }
  }
  FileStat_t fs;



  sprintf(FileName, "/home/daq/data/MC/ingridmc_numu_07a_wh_h_1.root","read");
  if(gSystem->GetPathInfo(FileName,fs)){
    cout<<"Cannot open file "<<FileName<<endl;
    exit(1);
  }
  TFile*               f  = new TFile(FileName,"read");
  TTree*            tree  = (TTree*)f->Get("tree");
  Int_t              Evt  = tree     ->GetEntries();
  cout<<"---- # of Evt    = "<<Evt<<"---------"<<endl;

  IngHit fHit;  

 
  f -> Close();
}
 
