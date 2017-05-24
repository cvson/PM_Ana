//C++ include
#include<iostream>
#include<sstream>
#include<fstream>
#include<math.h>
#include <iomanip.h>
#include <sys/stat.h>
using namespace std;
//ROOT include
#include <TROOT.h>
#include <TStyle.h>
#include <TApplication.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TBranch.h>
#include <TH1.h>
#include <TObject.h>
#include <TSystem.h>
FileStat_t fs;


#define RUNSTART  280060
#define RUNEND    300200

int main(int argc,char *argv[]){
  TROOT        root("GUI","GUI");
  TApplication theApp("App", 0, 0);

  Int_t                 nurun;
  Int_t            spill_flag;
  Int_t       good_spill_flag;
  Int_t               trg_sec;
  Int_t              spillnum;
  Int_t         nd280spillnum;
  Double_t          beam_time [5] [9];
  Double_t              ct_np [5] [9];
  Double_t              mumon [12];
  Double_t                hct [3] [5];
  Double_t                otr [13];


  TFile*       wfile = new TFile("/home/daq/data/BeamSummary/bsd_sum_ingird.root",
				 "recreate");
  TTree*       wtree = new TTree("bsd_ingrid","bsd_ingrid");
 

  wtree   ->  Branch ("nurun"           , &nurun           ,"nurun/I" );
  wtree   ->  Branch ("spill_flag"      , &spill_flag      ,"spill_flag/I" );
  wtree   ->  Branch ("good_spill_flag" , &good_spill_flag ,"good_spill_flag/I" );
  wtree   ->  Branch ("spillnum"        , &spillnum        ,"spillnum/I" );
  wtree   ->  Branch ("trg_sec"         , &trg_sec         ,"trg_sec/I" );
  wtree   ->  Branch ("nd280spillnum"   , &nd280spillnum   ,"nd280spillnum/I" );

  wtree   ->  Branch ("beam_time"       ,  beam_time       ,"beam_time[5][9]/D" );
  wtree   ->  Branch ("ct_np"           ,  ct_np           ,"ct_np[5][9]/D" );
  wtree   ->  Branch ("mumon"           ,  mumon           ,"mumon[12]/D" );
  wtree   ->  Branch ("hct"             ,  hct             ,"hct[3][5]/D" );
  wtree   ->  Branch ("otr"             ,  otr             ,"otr[13]/D" );
  wfile   ->  cd();

  TFile*       sfile;           //###### Run Summary File #######
  TTree*       tree;            //###### Tree at Run Summary ####
  char         FileName[300];   //###### Run Summary Name #######
  for( Int_t run = RUNSTART; run <= RUNEND; run++ ){
    for(Int_t subrun = 0; subrun < 20; subrun++){
      if(subrun == 0){
	sprintf(FileName, 
		"/home/daq/data/BeamSummary/bsd_run%07dp01.root",
		run);
      }
      else if(subrun >=1){
	sprintf(FileName, 
		"/home/daq/data/BeamSummary/bsd_run%07d_%dp01.root",
		run,subrun);
      }

      if( !gSystem->GetPathInfo(FileName,fs) ){ //##### Read Summary File
	cout  <<"reading "  <<FileName	  <<endl;
	sfile = new TFile( FileName, "read" );
	tree  = (TTree*)sfile ->Get("bsd");
	tree->SetBranchAddress ("nurun"      , &nurun  );
	tree->SetBranchAddress ("spill_flag" , &spill_flag  );
	tree->SetBranchAddress ("good_spill_flag" , &good_spill_flag  );
	tree->SetBranchAddress ("spillnum"   , &spillnum    );
	tree->SetBranchAddress ("trg_sec"    , &trg_sec     );
	tree->SetBranchAddress ("beam_time"  ,  beam_time   );
	tree->SetBranchAddress ("ct_np"      ,  ct_np       );
	tree->SetBranchAddress ("mumon"      ,  mumon       );
	tree->SetBranchAddress ("hct"        ,  hct       );
	tree->SetBranchAddress ("otr"        ,  otr       );

	//###### Loop over Summary number <run> #######
	//###### And Fill summary file for ingrid #####
	Int_t Nevent = tree -> GetEntries();
	for(Int_t i=0; i<Nevent; i++){
	  tree  -> GetEntry(i);
	  nd280spillnum = ( spillnum & 0xffff ) + 1;
	  wtree -> Fill      ();
	}

	sfile ->cd();
	sfile ->Close();
      }//##Read Summary File End
      else {
	cout  <<"not found: "  <<FileName	  <<endl;
      }
    }//SubRun
  }//Run
  wfile -> cd();
  wtree -> Write();
  wfile -> Write();
  wfile -> Close();

  //theApp.Run    ();
  return 0; 
}
 
