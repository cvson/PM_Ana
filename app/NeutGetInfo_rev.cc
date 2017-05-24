#include<iostream>
#include<sstream>
#include<fstream>
using namespace std;
#include <iomanip.h>
#include <math.h>
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

int main(int argc,char *argv[]){

  char buff1[200];//,buff2[200];
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  Int_t c=-1;
  char Output[300];

  while ((c = getopt(argc, argv, "f:o:")) != -1) {
    switch(c){
    case 'o':
      sprintf(Output,"%s",optarg);
      break;
    case 'f':
      sprintf(buff1,"%s",optarg);
      break;

    }
  }
  FileStat_t fs;
  
  //######## read root file #############################
  //#####################################################
  cout<<"reading "<<buff1<<"....."<<endl;
  if(gSystem->GetPathInfo(buff1,fs)){
    cout<<"Cannot open file "<<buff1<<endl;
    exit(1);
  }
  cout << buff1 << endl;
  TFile*            rfile = new TFile(buff1,"read");
  TTree*            tree  = (TTree*)rfile -> Get("h10");
  int                nevt = (int)tree -> GetEntries();
  Int_t           Mode;
  Int_t           Npar;

  Int_t           Ipnu[100];   //[Numnu]
  Float_t         Pnu[100][3];   //[Numnu]

  Int_t           Ipv[100];   //[Npar]                                         
  Int_t           Iorgv[100];   //[Npar]                                       
  Int_t           Icrnv[100];   //[Npar]                                       
  Int_t           Iflgv[100];   //[Npar]                                       
  Float_t         Abspv[100];   //[Npar]                                       
  Float_t         Pmomv[100][3];   //[Npar]
  Float_t norm,Totcrsne;
  tree->SetBranchAddress("Mode",&Mode);
  tree->SetBranchAddress("Npar",&Npar);

  tree->SetBranchAddress("Ipnu",&Ipnu);
  tree->SetBranchAddress("Pnu",&Pnu);

  tree->SetBranchAddress("Ipv",&Ipv);
  tree->SetBranchAddress("Iorgv",&Iorgv);
  tree->SetBranchAddress("Icrnv",&Icrnv);
  tree->SetBranchAddress("Iflgv",&Iflgv);
  tree->SetBranchAddress("Abspv",&Abspv);
  tree->SetBranchAddress("Pmomv",&Pmomv);
  tree->SetBranchAddress("norm",&norm);
  tree->SetBranchAddress("Totcrsne",&Totcrsne);


  cout << "Total # of events = " << nevt <<endl;

  //#### make rootfile after analysis #####
  //#######################################

  TFile*            wfile = new TFile(Output, "recreate");
  TTree*            wtree = new TTree("h10","h10");

  int npr,npi;
  int tpr,tpi;
  int pr[9],pi[9],tot[9];
  float pmom,pang,ppang;
  int inttype;

  wtree->Branch("inttype",&inttype,"inttype/I");
  wtree->Branch("norm",&norm,"norm/F");
  wtree->Branch("Totcrsne",&Totcrsne,"Totcrsne/F");
  wtree->Branch("npr",&npr,"npr/I");
  wtree->Branch("npi",&npi,"npi/I");
  wtree->Branch("tpr",&tpr,"tpr/I");
  wtree->Branch("tpi",&tpi,"tpi/I");
  wtree->Branch("pr",&pr,"pr[9]/I");
  wtree->Branch("pi",&pi,"pi[9]/I");
  wtree->Branch("tot",&tot,"tot[9]/I");
  wtree->Branch("pmom",&pmom,"pmom/F");
  wtree->Branch("pang",&pang,"pang/F");
  wtree->Branch("ppang",&ppang,"ppang/F");

  for(int ievt=0; ievt<nevt; ievt++){
    if(ievt%10000==0)cout << "analyze event# " << ievt<<endl;
    tree     -> GetEntry(ievt);
    //if(Mode!=1)continue;
    inttype=Mode;

    npr=0;
    npi=0;
    tpr=0;
    tpi=0;
    pang=-1;
    ppang=-1;
    pmom=-1;
    float pm1[3],pm2[3];
    memset(pr,0,sizeof(pr));
    memset(pi,0,sizeof(pi));
    memset(tot,0,sizeof(tot));
    for(int i=0;i<Npar;i++){
      if(Ipnu[i]==2212){
	pm1[0]=Pnu[i][0];
	pm1[1]=Pnu[i][1];
	pm1[2]=Pnu[i][2];
      }
    }
    
    for(int i=0;i<Npar;i++){
      if(Ipv[i]==211){
	if(Icrnv[i]!=0)
	  npi++;
	tpi++;
	pi[Iflgv[i]]++;
      }
      if(Ipv[i]==2212){
	if(Icrnv[i]!=0){
	  npr++;
	  if(pmom<Abspv[i]){
	    pmom=Abspv[i];
	    pang=180/3.141592*acos(Pmomv[i][2]/Abspv[i]);
	    pm2[0]=Pmomv[i][0];pm2[1]=Pmomv[i][1];pm2[2]=Pmomv[i][2];
	    ppang=180/3.141592*acos((pm1[0]*pm2[0]+pm1[1]*pm2[1]+pm1[2]*pm2[2])/sqrt(pm1[0]*pm1[0]+pm1[1]*pm1[1]+pm1[2]*pm1[2])/sqrt(pm2[0]*pm2[0]+pm2[1]*pm2[1]+pm2[2]*pm2[2]));
	  }
	}
	tpr++;
	pr[Iflgv[i]]++;
      }
      tot[Iflgv[i]]++;
    }

    norm=norm;
    Totcrsne=Totcrsne;
    wtree->Fill();

  }//Event Loop
  
  //######## Write and Close ####################
  wtree  -> Write();
  wfile  -> Write();
  wfile  -> Close();
  
}

