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

int main(int argc,char *argv[]){

  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  char FileName[300];
  int c=-1; 
  int NStartRun=0;
  int NEndRun=1;
  while ((c = getopt(argc, argv, "s:e:")) != -1) {
    switch(c){
    case 's':
      NStartRun = atoi( optarg );
      break;
   
    case 'e':
      NEndRun = atoi( optarg );
      break;
    }
  }
  FileStat_t fs;

  IngridEventSummary* evt = new IngridEventSummary();
  BeamInfoSummary* beam;
  time_t ltime;
  localtime(&ltime);
  //#####################################################
  //ofstream wfile("/home/daq/shell/ingdb.txt");
  ofstream wfile("ingdb.txt");
  int      run_stime,run_etime;
  for(int irun=NStartRun; irun<=NEndRun; irun++){
    cout << "making db of run# " << irun << endl;
    for(int isrun=0; isrun<=200; isrun++){
      //sprintf(FileName, "/home/ingrid/data/dst/ingrid_%08d_%04d_done.root",irun, isrun);
      sprintf(FileName, "/home/ingrid/data/beam_ana/ingrid_%08d_%04d_trk.root",irun, isrun);
      if(gSystem->GetPathInfo(FileName,fs)){
	continue;
      }
      cout << FileName << endl;
      TFile* rfile = new TFile(FileName, "read");
      TTree* tree = (TTree*)rfile -> Get("tree");
      TBranch*          EvtBr= tree->GetBranch("fDefaultReco.");
      EvtBr ->SetAddress(&evt);
      tree  ->SetBranchAddress("fDefaultReco.", &evt);
      int  nevt = (int)tree -> GetEntries();
      if(nevt==0)continue;
      long stime, etime, sspill, espill;
      //### Start time ####
      evt      -> Clear();
      tree     -> GetEntry(0);
      beam = (BeamInfoSummary*)(evt->GetBeamSummary(0));
      stime  = evt  -> time;
      sspill = beam -> spillnum;
      //### End time ####
      evt      -> Clear();
      tree     -> GetEntry(nevt-1);
      beam = (BeamInfoSummary*)(evt->GetBeamSummary(0));
      etime  = evt -> time;
      espill = beam -> spillnum;
      if(stime==-1||etime==-1){
	rfile -> Close();
	continue;
      }



      
      wfile << irun << "\t"
	    << isrun << "\t"
	    << stime << "\t"
	    << etime << "\t"
	    << sspill << "\t"
	    << espill << endl;
      

      rfile -> Close();
    }//isrun
  }//irun

}
