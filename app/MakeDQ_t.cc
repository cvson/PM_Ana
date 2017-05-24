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
//#include "IngridConstants.h"

int main(int argc,char *argv[]){

  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  Int_t c=-1;  
  char FileName[300];
  Int_t SRUN = 0, ERUN = 0;
  while ((c = getopt(argc, argv, "s:e:")) != -1) {
    switch(c){
    case 's':
      SRUN=atoi(optarg);
      break;
    case 'e':
      ERUN=atoi(optarg);
      break;
    }
  }
  FileStat_t fs;
  ofstream www("INGRID_DQ.txt");
  for(Int_t irun = SRUN; irun <= ERUN; irun++){

    cout << irun << endl;
    for(Int_t isrun = 0; isrun <= 200; isrun++){
      if(irun==13149&&isrun>=140)continue;
      sprintf(FileName, "/home/ingrid/data/dst/ingrid_%08d_%04d_track.root", irun, isrun);

      if(gSystem->GetPathInfo(FileName,fs))continue;

      TFile*            rfile = new TFile(FileName, "read");
      TTree*             tree = (TTree*)rfile -> Get("tree");      
      IngridEventSummary* evt = new IngridEventSummary();
      TBranch*          EvtBr = tree->GetBranch("fDefaultReco.");
      EvtBr                   ->SetAddress(&evt);
      tree                    ->SetBranchAddress("fDefaultReco.", &evt);
      int                nevt = (int)tree -> GetEntries();
      
      tree -> GetEntry(0);
      Int_t stime = evt->time;
      tree -> GetEntry(nevt-1);
      Int_t etime = evt->time;
      www  << "'INGRID' | " 
	   << stime              << " | "
	   << etime              << " | "
	   << "'0'"              << " | "
	   << "1301965611"       << " | "
	   << "'Otani-san code'" << " | "
	   << "'Masashi Otani'"  << " | "
	   << "'0 is OK'"        << " | "
	   << "' '"              << " | "
	   << "'All OK'"         << " | "
	   << endl;
      rfile->Close();
    }//isrun
  }//irun
  www.close();
}
