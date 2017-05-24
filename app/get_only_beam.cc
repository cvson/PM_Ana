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

Int_t             nHit;
vector<int>*       adc;
vector<double>*     pe;
vector<int>*      nsec;
vector<long>*      tdc;
vector<int>*      view;
vector<int>*       pln;
vector<int>*        ch;
Int_t           nSpill;
Long64_t         UTime;
Int_t            Cycle;
Int_t             fMod;


int                  n;
Int_t           NumEvt;
Int_t       Nhitclster;
int counter;

int main(int argc,char *argv[]){

  char buff1[200],buff2[200];
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  Int_t c=-1;  
  char FileName[300];
  while ((c = getopt(argc, argv, "r:")) != -1) {
    switch(c){
    case 'r':
      sprintf(FileName, "%s", optarg);
      break;
    }
  }
  FileStat_t fs;

  //######### real beam spill number reading ############
  //#####################################################
  vector<int> nRealBeam;
  int           tempnum;
  sprintf(buff1,"/home/daq/0912DecBeam/SumSpillNum0912.txt"); //0912 Beam Commissioning File
  cout<<"reading "<<buff1<<"....."<<endl;
  if(gSystem->GetPathInfo(buff1,fs)){
    cout<<"Cannot open file "<<buff1<<endl;
    exit(1);
  }
  ifstream FileSpill(buff1);
  while(FileSpill>>tempnum){
    nRealBeam.push_back(tempnum);
  }

  //#####################################################

 
  cout<<"reading "<<FileName
      <<"....."   <<endl;
  if(gSystem->GetPathInfo(FileName,fs)){
    cout<<"Cannot open file "<<buff1<<endl;
    exit(1);
  }
  //####### Old ROOT File which contains  #############
  //####### not only beam but also dummy  #############
  TFile*               f  = new TFile(FileName,"read");
  TTree*            tree  = (TTree*)f->Get("tree");
  Int_t        Evt        = tree->GetEntries();
  cout<<"---- # of Evt    = "<<Evt<<"---------"<<endl;
  pe   = 0;
  nsec = 0;
  view = 0;
  pln  = 0;
  ch   = 0;
  Long_t UTime;
  tree ->SetBranchAddress("nHit"  ,&nHit );
  tree ->SetBranchAddress("adc"   ,&adc  );
  tree ->SetBranchAddress("pe"    ,&pe   );
  tree ->SetBranchAddress("nsec"  ,&nsec );
  tree ->SetBranchAddress("tdc"   ,&tdc  );
  tree ->SetBranchAddress("view"  ,&view );
  tree ->SetBranchAddress("pln"   ,&pln  );
  tree ->SetBranchAddress("ch"    ,&ch   );
  tree ->SetBranchAddress("fMod"  ,&fMod );
  tree ->SetBranchAddress("Cycle" ,&Cycle);
  tree ->SetBranchAddress("nSpill",&nSpill);
  tree ->SetBranchAddress("UTime" ,&UTime);

  char NewFileName[300];
  sprintf(NewFileName,"%s.onlybeam", FileName);

  //####### New ROOT File which contains  #############
  //####### only beam #################################
  TFile*              rf  = new TFile(NewFileName,"recreate");
  TTree*           rtree  = new TTree("tree", "tree");
  rtree->Branch("nHit"  ,&nHit );
  rtree->Branch("adc"   ,&adc  );
  rtree->Branch("pe"    ,&pe   );
  rtree->Branch("nsec"  ,&nsec );
  rtree->Branch("tdc"   ,&tdc  );
  rtree->Branch("view"  ,&view );
  rtree->Branch("pln"   ,&pln  );
  rtree->Branch("ch"    ,&ch   );
  rtree->Branch("fMod"  ,&fMod );
  rtree->Branch("Cycle" ,&Cycle);
  rtree->Branch("nSpill",&nSpill);
  rtree->Branch("UTime" ,&UTime);


  for(int n=0; n<Evt; n++){
    if(n%1000==0)cout<<"processing "<<n<<"th events..."<<endl;
    tree         -> GetEntry(n);
    bool writing =  false;
    for(int i=0; i<nRealBeam.size(); i++){
      if(nSpill == nRealBeam[i]){
	writing  = true;
      }
    }
    if(writing)rtree -> Fill();
  }
  rtree -> Write ();
  rf    -> Write ();
  rf    -> Close ();
  f     -> Close ();
}
