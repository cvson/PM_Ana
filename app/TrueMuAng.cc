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
  IngridEventSummary* evt = new IngridEventSummary();
  TFile*            rfile = new TFile(FileName,"read");
  TTree*             tree = (TTree*)rfile -> Get("tree");
  TBranch*          EvtBr = tree->GetBranch("fDefaultReco.");
  EvtBr                   ->SetAddress(&evt);
  tree                    ->SetBranchAddress("fDefaultReco.", &evt);
  int                nevt = (int)tree -> GetEntries();
  cout << "Total # of events = " << nevt <<endl;
  if( nevt==0 )
    exit(1);

  float norm,totcrsne,muang,nuE;
  int inttype;
  TFile* wfile = new TFile(Output, "recreate");
  TTree* wtree = new TTree("tree","tree");
  wtree->Branch("muang",&muang,"muang/F");
  wtree->Branch("norm",&norm,"norm/F");
  wtree->Branch("totcrsne",&totcrsne,"totcrsne/F");
  wtree->Branch("inttype",&inttype,"inttype/I");
  wtree->Branch("nuE",&nuE,"nuE/F");


  IngridSimParticleSummary* simpar;
  IngridSimVertexSummary* simver;


  for( int ievt = 0; ievt < nevt; ievt++  ){
    if(ievt%100==0)cout << "analyze event# " << ievt<<endl;
    evt      -> Clear();
    tree -> GetEntry(ievt);


    if(evt->NIngridSimVertexes()>0){
      simver = (IngridSimVertexSummary*)(evt -> GetSimVertex(0) );
      norm     = simver -> norm;
      totcrsne = simver -> totcrsne;
      inttype  = simver -> inttype;
      nuE      = simver -> nuE;
    }


    float mommax=0;
    float trueang=0;
    int nsimpar = evt -> NIngridSimParticles();
    for( int ipar = 0; ipar < nsimpar; ipar++  ){
      
      simpar = (IngridSimParticleSummary*)(evt->GetSimParticle(ipar));

      if(simpar -> pdg != 13) continue;

      if(fabs(simpar -> momentum[2]) < mommax) continue;
      mommax = fabs(simpar -> momentum[2]);
      //trueang = acos(fabs(simpar -> momentum[2])/sqrt(simpar -> momentum[1]*simpar -> momentum[1]+simpar -> momentum[2]*simpar -> momentum[2]+simpar -> momentum[0]*simpar -> momentum[0]))*180/3.14159265;
      trueang = acos(simpar -> momentum[2]/sqrt(simpar -> momentum[1]*simpar -> momentum[1]+simpar -> momentum[2]*simpar -> momentum[2]+simpar -> momentum[0]*simpar -> momentum[0]))*180/3.14159265;


    }
    muang = trueang;
    wtree    -> Fill();
  }//Event Loop


  //######## Write and Close ####################
  wtree  -> Write();
  wfile  -> Write();
  wfile  -> Close();
  rfile  -> Close();
}
 

