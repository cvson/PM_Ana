//##### Standard C++ lib. ######
#include<iostream>
#include<sstream>
#include<fstream>
using namespace std;
#include <iomanip.h>
#include <sys/stat.h>
//##### Root Library ###########
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
#include <TSystem.h>
#include <TF1.h>
#include <TRandom3.h>
#include <TMath.h>
//##### INGRID Library #########
#include "INGRIDEVENTSUMMARY.h"
#include "IngridHitSummary.h"
#include "IngridSimHitSummary.h"
#include "IngridSimVertexSummary.h"


int main(int argc,char *argv[]){
  TROOT        root ("GUI","GUI");
  TApplication app  ("App",0,0);
  FileStat_t fs;
  int  c=-1;
  char FileName[300], Output[300];
  float prob=1,rand;
  while ((c = getopt(argc, argv, "f:o:p:")) != -1) {
    switch(c){
    case 'f':
      sprintf(FileName, "%s", optarg);
      break;
    case 'o':
      sprintf(Output, "%s", optarg);
      break;
    case 'p':
      prob=atof(optarg);
      break;
    }
  }

  cout << "Probability = " << prob <<endl;

  //#### Read file before masking hits ######
  //######################################### 
  IngridEventSummary* summary = new IngridEventSummary();

  if(gSystem->GetPathInfo(FileName,fs)){
    cout<<"Cannot open file "<<FileName<<endl;
    exit(1);
  }
  TFile*            rfile = new TFile(FileName,"read");
  TTree*             tree = (TTree*)rfile -> Get("tree");
  TBranch*          EvtBr = tree->GetBranch("fDefaultReco.");
  EvtBr                   ->SetAddress(&summary);
  tree                    ->SetBranchAddress("fDefaultReco.", &summary);
  int                nevt = (int)tree -> GetEntries();
  cout << "Total # of events = " << nevt <<endl;

  //#### Create file after masking hits ######
  //##########################################
  TFile*              wfile    = new TFile(Output,"recreate");
  TTree*              wtree    = new TTree("tree","tree");
  IngridEventSummary* wsummary = new IngridEventSummary(); 
  wtree              -> Branch   ("fDefaultReco.","IngridEventSummary", 
				 &wsummary,  64000,   99);

  TRandom3 r; //### random number sheed
  int      startcyc = 4; //MC data is fille in cycle 4 
  int      endcyc   = 5;
 
  IngridHitSummary* inghitsum;
  for(int ievt = 0; ievt < nevt; ++ievt){
    if(ievt%1000==0)cout<<"Event:"<<ievt<<endl;
    summary -> Clear();
    wsummary-> Clear();
    tree    -> GetEntry(ievt);

    IngridSimVertexSummary* simver =  (IngridSimVertexSummary*)( summary -> GetSimVertex(0) );
    wsummary -> AddSimVertex(simver);

    for(int mod=0; mod<17; mod++){
      for(int cyc=startcyc; cyc<endcyc; cyc++){
	int ninghit = summary -> NIngridModHits(mod, cyc);

        for(int i=0; i<ninghit; i++){
          inghitsum   = (IngridHitSummary*) (summary -> GetIngridModHit(i, mod, cyc) );
	  rand = r.Uniform(0,1);
	  if(rand <= prob){
	    wsummary -> AddIngridModHit(inghitsum ,mod, cyc);
	  }
	}

      }//cyc
    }//mod

    wtree -> Fill();
  }

  wtree -> Write();
  wfile -> Write();
  wfile -> Close();
  //app.Run();
}
