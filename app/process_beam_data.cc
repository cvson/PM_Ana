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


#define RUNSTART  280000
#define RUNEND    290000


vector<int>*       adc;
vector<double>*     pe;
vector<int> *     nsec;
vector<long>*      tdc;
vector<int>*       pln;
vector<int>*        ch;
vector<int>*      view;
vector<double>*  posxy;
vector<double>*   posz;


int main(int argc,char *argv[]){
  TROOT        root("GUI","GUI");
  TApplication theApp("App", 0, 0);
  Int_t        run_number;
  Int_t c=-1;
  while ((c = getopt(argc, argv, "r:")) != -1) {
    switch(c){
    case 'r':
      run_number=atoi(optarg);
      break;
    }
  }
  char FileName[300];
  char newFileName[300];
  sprintf(FileName, 
	  "/home/daq/data/root_new/ingrid_%08d_0000.root", run_number);
  cout  <<"reading "  <<FileName	  <<endl;
  if( gSystem->GetPathInfo(FileName,fs) ){ //##### Read Summary File
    cout<<"not exist "<<FileName<<endl;
    exit(0);
  }
  sprintf(newFileName, 
	  "/home/daq/data/root_new/ingrid_processed_%08d_0000.root", run_number);
      
  
  //######## Variables for INGRID Data ###############
  //################################################## 
  Int_t          NumEvt;
  Int_t            fMod;
  Long_t          fTime;
  Int_t            nHit;
  Int_t          nSpill;
  Int_t           Cycle;
  Long_t          UTime;
  adc   == 0;
  pe    == 0;
  nsec  == 0;
  tdc   == 0;
  pln   == 0;
  ch    == 0;
  view  == 0;
  posxy == 0;
  posz  == 0;


  //######## Variables for Beam Summary Data #########
  //################################################## 
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
  //######## INGRID Original #########################
  Bool_t           bunch_flag;
  Bool_t               wohorn;
  Bool_t                whorn;
  Bool_t                horn1;
  Bool_t              cutwotr;
  Bool_t             scutwotr;
  Bool_t              horn250;

  //######## Make Processed INGRID DATA ##############
  //######## Which had Beam Summary Data #############
  TFile*       wfile = new TFile(newFileName,
				 "recreate");
  TTree*       wtree = new TTree("tree","tree");
  wtree    ->  Branch("nSpill" ,&nSpill ,"nSpill/I");
  wtree    ->  Branch("UTime"  ,&UTime  ,"UTime/L" );
  wtree    ->  Branch("NumEvt" ,&NumEvt ,"NumEvt/I");
  wtree    ->  Branch("Cycle"  ,&Cycle  ,"Cycle/I" );
  wtree    ->  Branch("fMod"   ,&fMod   ,"fMod/I"  );
  wtree    ->  Branch("nHit"   ,&nHit   ,"nHit/I"  );

  wtree    ->  Branch("adc"    ,&adc           );
  wtree    ->  Branch("pe"     ,&pe            );
  wtree    ->  Branch("nsec"   ,&nsec          );
  wtree    ->  Branch("tdc"    ,&tdc           );
  wtree    ->  Branch("view"   ,&view          );
  wtree    ->  Branch("pln"    ,&pln           );
  wtree    ->  Branch("ch"     ,&ch            );
  wtree    ->  Branch("posxy"  ,&posxy         );
  wtree    ->  Branch("posz"   ,&posz          );
  
  wtree    ->  Branch("nurun"           , &nurun           ,"nurun/I"      );
  wtree    ->  Branch("spill_flag"      , &spill_flag      ,"spill_flag/I"      );
  wtree    ->  Branch("good_spill_flag" , &good_spill_flag ,"good_spill_flag/I" );
  wtree    ->  Branch("spillnum"        , &spillnum        ,"spillnum/I"        );
  wtree    ->  Branch("nd280spillnum"   , &nd280spillnum   ,"nd280spillnum/I"   );
  wtree    ->  Branch("trg_sec"         , &trg_sec         ,"trg_sec/I"         );
  wtree    ->  Branch("beam_time"       ,  beam_time       ,"beam_time[5][9]/D" );
  wtree    ->  Branch("ct_np"           ,  ct_np           ,"ct_np[5][9]/D"     );
  wtree    ->  Branch("mumon"           ,  mumon           ,"mumon[12]/D"       );
  wtree    ->  Branch("hct"             ,  hct             ,"hct[3][5]/D"       );
  wtree    ->  Branch("otr"             ,  otr             ,"otr[13]/D"         );
  wtree    ->  Branch("bunch_flag"      , &bunch_flag      ,"bunch_flag/B"      );
  wtree    ->  Branch("wohorn"          , &wohorn          ,"wohorn/B"          );
  wtree    ->  Branch("whorn"           , &whorn           ,"whorn/B"           );
  wtree    ->  Branch("horn1"           , &horn1           ,"horn1/B"           );
  wtree    ->  Branch("cutwotr"         , &cutwotr         ,"cutwotr/B"          );
  wtree    ->  Branch("scutwotr"        , &scutwotr        ,"scutwotr/B"           );
  wtree    ->  Branch("horn250"         , &horn250         ,"horn250/B"           );
 
 
 
  //######## Read Beam Summary File for INGRID #######
  //##################################################
  TFile*       fingrid = new TFile(FileName,   "read");
  TTree*       tree    = (TTree*)fingrid->Get("tree");

  tree->SetBranchAddress("nHit"   ,&nHit          );
  tree->SetBranchAddress("adc"    ,&adc           );
  tree->SetBranchAddress("pe"     ,&pe            );
  tree->SetBranchAddress("nsec"   ,&nsec          );
  tree->SetBranchAddress("tdc"    ,&tdc           );
  tree->SetBranchAddress("view"   ,&view          );
  tree->SetBranchAddress("pln"    ,&pln           );
  tree->SetBranchAddress("ch"     ,&ch            );
  tree->SetBranchAddress("posxy"  ,&posxy         );
  tree->SetBranchAddress("posz"   ,&posz          );
  tree->SetBranchAddress("nSpill" ,&nSpill        );
  tree->SetBranchAddress("UTime"  ,&UTime         );
  tree->SetBranchAddress("NumEvt" ,&NumEvt        );
  tree->SetBranchAddress("Cycle"  ,&Cycle         );
  tree->SetBranchAddress("fMod"   ,&fMod          );
 
  //######## Read Beam Summary File for INGRID #######
  //##################################################
  TFile*       bsum = new TFile("/home/daq/data/BeamSummary/bsd_sum_ingird.root",
				 "read");
  TTree*       bsd  = (TTree*)bsum->Get("bsd_ingrid");
  bsd               ->SetBranchAddress ("nurun"      , &nurun  );
  bsd               ->SetBranchAddress ("spill_flag" , &spill_flag  );
  bsd               ->SetBranchAddress ("good_spill_flag" , &good_spill_flag  );
  bsd               ->SetBranchAddress ("spillnum"   , &spillnum    );
  bsd               ->SetBranchAddress ("nd280spillnum"   , &nd280spillnum    );
  bsd               ->SetBranchAddress ("trg_sec"    , &trg_sec     );
  bsd               ->SetBranchAddress ("beam_time"  ,  beam_time   );
  bsd               ->SetBranchAddress ("ct_np"      ,  ct_np       );
  bsd               ->SetBranchAddress ("mumon"      ,  mumon       );
  bsd               ->SetBranchAddress ("hct"        ,  hct         );
  bsd               ->SetBranchAddress ("otr"        ,  otr         );
 
  
  Int_t Nevent  = tree -> GetEntries();
  Int_t lastbsd = 0;
  for(Int_t i=0; i<Nevent; i++){
    if(i%100==0)cout<<"processed... "<<i<<endl;
    tree  -> GetEntry(i);
    Int_t Nevent2 = bsd -> GetEntries();
    for(Int_t j=lastbsd; j<Nevent2; j++){
      bsd -> GetEntry(j);
      if( trg_sec > UTime ) {
	lastbsd = j - 100;
	if(lastbsd<0)lastbsd = 0;
	break;
      }
      if( spill_flag && 
	  nd280spillnum ==  nSpill &&
	  abs( trg_sec - UTime ) < 100 ){ // Consistent Event

	bunch_flag = false;
	if( ( Cycle == 4 || Cycle == 5 || Cycle == 6 || 
	      Cycle == 7 || Cycle == 8 || Cycle == 9 ) &&
	    ( ct_np [0][ Cycle - 4 + 1] > 1e11 )) //Use total of CT
	  bunch_flag = true;

	//##### Horn Flag #######
	//#######################
	wohorn = false;//Before 1001, hct*0.206 = current 
	               //after  1001, hct       = current
	whorn  = false;
	horn1  = false;
	if( hct[0][0] * 0.206 > 300 &&
	    hct[1][0] * 0.206 > 300 &&
	    hct[2][0] * 0.206 > 300 ){
	  whorn  = true;
	}
	else if( hct[0][0] * 0.206 > 300 &&
		 hct[1][0] * 0.206 < 10  &&
		 hct[2][0] * 0.206 < 10 ){
	  horn1  = true;
	}
	else if( hct[0][0] * 0.206 < 10  &&
		 hct[1][0] * 0.206 < 10  &&
		 hct[2][0] * 0.206 < 10 ){
	  wohorn = true;
	}
	horn250 = false;
	if( fabs ( hct[0][0] - 250 ) < 10 &&
	    fabs ( hct[1][0] - 250 ) < 10 &&
	    fabs ( hct[2][0] - 250 ) < 10 )
	  horn250 = true;

	//##### Beam position cut ####
	//##### with OTR #############
	//############################
	cutwotr = false;
	scutwotr= false;
	double r = sqrt ( 1.0 * otr[0] * otr[0] + 1.0 * otr[1] * otr[1] );
	if( r <= 10 ){
	  cutwotr  = true;
	}
	if( r <= 3 ){
	  scutwotr = true;
	}

	wtree -> Fill();
	lastbsd = j;
	break;
      }
    }//BSD Loop
  }//INGRID Loop

  bsum    -> cd();
  bsum    -> Close();
  fingrid -> cd();
  fingrid -> Close();
  wfile   -> cd();
  wtree   -> Write();
  wfile   -> Write();
  wfile   -> Close();

  //theApp.Run    ();
  return 0; 
}
