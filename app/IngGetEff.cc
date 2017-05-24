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
#include "IngridBasicReconSummary.h"

#define  CCQE    1
#define  CCother 2
#define  NC      3
int main(int argc,char *argv[]){
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);

  Int_t c=-1;  char FileName[300], Output[300];
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
  FileStat_t fs;
  ifstream timing;
  //######## read root file #############################
  //#####################################################
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

  TFile*            wfile = new TFile(Output,"recreate");
  TTree*            wtree = new TTree("tree","tree");
  IngridEventSummary* wsummary = new IngridEventSummary(); 
  wtree              -> Branch   ("fDefaultReco.","IngridEventSummary", 
				  &wsummary,  64000,   99);

  IngridSimVertexSummary* simver;
  int   inttype;
  int   flavor;
  float Enu;
  for( int ievt = 0; ievt < nevt; ievt++  ){
    if(ievt%100==0)cout << "analyze event# " << ievt<<endl;
    evt      -> Clear();
    wsummary -> Clear();
    tree     -> GetEntry(ievt);


    simver = (IngridSimVertexSummary*)( evt -> GetSimVertex(0) );
    //### neutrino flavor #####
    flavor = (int)( (simver -> promode)/10 );
    //### neutrino interaction type #####
    if( simver -> inttype==1 )     
      inttype = CCQE;
    else if( simver -> inttype!=1 && simver -> inttype < 30 )
      inttype = CCother;
    else if( simver -> inttype > 30 )
      inttype = NC;
    //### neutrino energy ####
    Enu = simver->Enu;

    
    // wsummary -> Write();
    wtree    -> Fill();
  }//Event Loop


  //######## Write and Close ####################
  wtree  -> Write();
  wfile  -> Write();
  wfile  -> Close();
  rfile  -> Close();
}
 

