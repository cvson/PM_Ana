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

#include "INGRIDEVENTSUMMARY.h"
#include "IngridHitSummary.h"
#include "IngridSimHitSummary.h"
#include "IngridSimVertexSummary.h"
#include "IngridSimParticleSummary.h"
#include "BeamInfoSummary.h"
#include "IngridBasicReconSummary.h"
#include "PMReconSummary.h"
#include "NeutInfoSummary.h"



int main(int argc,char *argv[]){

  float nu[3];
  nu[0]=0;nu[1]=-tan(3.65/180*3.141592);nu[2]=1;
  float tmp=sqrt(nu[0]*nu[0]+nu[1]*nu[1]+nu[2]*nu[2]);
  nu[0]=nu[0]/tmp;
  nu[1]=nu[1]/tmp;
  nu[2]=nu[2]/tmp;

  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);

  Int_t c=-1;  char FileName[300];
  char Output[300];
  while ((c = getopt(argc, argv, "f:o:")) != -1) {
    switch(c){
    case 'f':
      sprintf(FileName, "%s",optarg);
      break;
    case 'o':
      sprintf(Output, "%s",optarg);
      break;
    }
  }

  //######## read root file #############################
  //#####################################################
  FileStat_t fs;
  cout<<"reading "<<FileName<<"....."<<endl;
  if(gSystem->GetPathInfo(FileName,fs)){
    cout<<"Cannot open file "<<FileName<<endl;
    exit(1);
  }

  int mode;
  int numnu;
  int ipnu[50];
  float pnu[50];
  float dirnu[50][3];

  TFile*            rfile = new TFile(FileName,"read");
  TTree*             tree = (TTree*)rfile -> Get("h1");

  tree -> SetBranchAddress("mode", &mode);//Neutrino interaction mode
  tree -> SetBranchAddress("numnu", &numnu);//Number of particle
  tree -> SetBranchAddress("ipnu", ipnu);//Particle code
  tree -> SetBranchAddress("pnu", pnu);//Abs. momentum
  tree -> SetBranchAddress("dirnu", dirnu);//Particle direction
  int                nevt = (int)tree -> GetEntries();
  cout << "Total # of events = " << nevt <<endl;
  if( nevt==0 )
    exit(1);

  float norm,totcrsne,muang,nuE;
  float mumom,X,enu,emu,Qsq;
  int target;
  int inttype;
  TFile* wfile = new TFile(Output, "recreate");
  TTree* wtree = new TTree("tree","tree");
  wtree->Branch("muang",&muang,"muang/F");
  wtree->Branch("mumom",&mumom,"mumom/F");
  wtree->Branch("enu",&enu,"enu/F");
  wtree->Branch("emu",&emu,"emu/F");
  wtree->Branch("X",&X,"X/F");
  wtree->Branch("Qsq",&Qsq,"Qsq/F");
  wtree->Branch("target",&target,"target/I");


  for( int ievt = 0; ievt < nevt; ievt++  ){
    if(ievt%1000==0)cout << "analyze event# " << ievt<<endl;

    tree -> GetEntry(ievt);

    float mommax=0;
    float trueang=-1;
    float truemom=-1;
    float trueX=-1;

    trueang=acos(dirnu[0][0]*dirnu[2][0]+dirnu[0][1]*dirnu[2][1]+dirnu[0][2]*dirnu[2][2]);
    truemom=pnu[2];

    if(ipnu[2]==13){
      emu=sqrt(0.10565836668*0.10565836668+pnu[2]*pnu[2]);
    }
    else{
      emu=sqrt(pnu[2]*pnu[2]);
    }

    float qsq = emu*pnu[0]-pnu[2]*pnu[0]*(dirnu[0][0]*dirnu[2][0]+dirnu[0][1]*dirnu[2][1]+dirnu[0][2]*dirnu[2][2]);
    trueX = qsq/(2*(pnu[0]-emu)*(0.939565379+0.938272046)/2);

    muang = trueang;
    mumom = truemom;
    X     = trueX;
    Qsq   = qsq;
    enu   = pnu[0];
    target = ipnu[1];
    wtree    -> Fill();
  }//Event Loop


  //######## Write and Close ####################
  wtree  -> Write();
  wfile  -> Write();
  wfile  -> Close();
  rfile  -> Close();
}
 

