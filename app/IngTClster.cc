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
//#include "analysis_beam.hxx"
#include "IngTClster.hxx"


#include "INGRIDEVENTSUMMARY.h"
#include "IngridHitSummary.h"
#include "IngridSimHitSummary.h"
#include "IngridSimVertexSummary.h"
#include "IngridSimParticleSummary.h"
#include "BeamInfoSummary.h"
#include "IngridBasicReconSummary.h"

//#define  STUDY_BADCHANNEL
vector<int>  bad_mod;
vector<int>  bad_pln;
vector<int>  bad_view;
vector<int>  bad_ch;
bool Add_Bad_Channel();
bool Is_Bad_Channel(IngridHitSummary* thit);

#include "IngridConstants.h"
long fcTime;
vector<Hit> hitclster;
int main(int argc,char *argv[]){

#ifdef STUDY_BADCHANNEL
  cout << "It is BAD channel study mode. Is it O.K.?" << endl;
  //cin.get();
  Add_Bad_Channel();
#endif

  char buff1[200],buff2[200];
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  Int_t run_number;
  Int_t sub_run_number=0;
  Int_t c=-1;  char FileName[300];
  char Output[300]; bool rename = false;
  Int_t Scyc =  0;
  Int_t Ncyc = 23;
  bool  cosmic = false;
  while ((c = getopt(argc, argv, "r:s:f:b:co:")) != -1) {
    switch(c){
    case 'r':
      run_number=atoi(optarg);
      break;
    case 'o':
      sprintf(Output,"%s",optarg);
      rename  = true;
      break;
    case 's':
      sub_run_number=atoi(optarg);

      break;
    case 'b':
      run_number=atoi(optarg);
      sprintf(buff1,"%s/ingrid_%08d_%04d_calib.root",
	      dst_data,run_number, sub_run_number);
      break;
    case 'f':
      sprintf(buff1,"%s",optarg);
      run_number=77;
      break;
    case 'c':
      Scyc = 14;
      Ncyc = 16;
      cosmic = true;
      break;

    }
  }
  FileStat_t fs;
  ifstream timing;

  //sprintf(buff1,"%s/ingrid_%08d_%04d_calib.root",
  //  dst_data, run_number,sub_run_number);
  //if(cosmic){
  //sprintf(buff1,"%s/ingrid_%08d_%04d_calib.root",
  //    cosmic_data, run_number,sub_run_number);
  //
  //}

  //######## read root file #############################
  //#####################################################
  cout<<"reading "<<buff1<<"....."<<endl;
  if(gSystem->GetPathInfo(buff1,fs)){
    cout<<"Cannot open file "<<buff1<<endl;
    exit(1);
  }
  cout << buff1 << endl;
  IngridEventSummary* evt = new IngridEventSummary();
  //TFile*            rfile = new TFile(Form("/home/daq/data/dst/ingrid_%08d_%04d_processed.root", 
  TFile*            rfile = new TFile(buff1,
				      "read");
  TTree*             tree = (TTree*)rfile -> Get("tree");
  TBranch*          EvtBr = tree->GetBranch("fDefaultReco.");
  EvtBr                   ->SetAddress(&evt);
  tree                    ->SetBranchAddress("fDefaultReco.", &evt);
  int                nevt = (int)tree -> GetEntries();
  cout << "Total # of events = " << nevt <<endl;

  //#### make rootfile after analysis #####
  //#######################################
  //sprintf(buff1,"/home/daq/data/dst/ingrid_%08d_0000_tclster.root",run_number);
  //if( !rename ){
  //sprintf(Output, "%s/ingrid_%08d_%04d_tclster.root", 
  //    dst_data, run_number, sub_run_number); 
  //}
  //if(cosmic){
  //sprintf(Output, "%s/ingrid_%08d_%04d_tclster.root", 
  //    cosmic_data, run_number, sub_run_number); 
  //}

  TFile*            wfile = new TFile(//Form("%s/ingrid_%08d_%04d_tclster.root", 
				      //dst_data,
				      //run_number,
				      //sub_run_number),
				      Output,
				      "recreate");
  TTree*            wtree = new TTree("tree","tree");
  wtree -> SetMaxTreeSize(5000000000);
  IngridEventSummary* wsummary = new IngridEventSummary(); 
  wtree              -> Branch   ("fDefaultReco.","IngridEventSummary", 
				 &wsummary,  64000,   99);
  IngridHitSummary*    inghitsum;
  IngridHitSummary*  inghitsum_t; 
  IngridBasicReconSummary* recon = new IngridBasicReconSummary();
  Hit                        hit;
  for(int ievt=0; ievt<nevt; ievt++){
    //for(int ievt=0; ievt<1000; ievt++){

    if(ievt%100==0)cout << "analyze event# " << ievt<<endl;
    wsummary -> Clear();
    evt      -> Clear();
    tree     -> GetEntry(ievt);
     for( int mod=0; mod<nMod; mod++ ){   //### Module Loop
      for( int cyc=Scyc; cyc<Ncyc; cyc++ ){  //### Cycle Loop

	//##### put the IngridModHit class to simple Hit class ######
	//###########################################################
	allhit.clear();
	int ninghit = evt -> NIngridModHits(mod, cyc);
	for(int i=0; i<ninghit; i++){
	  inghitsum   = (IngridHitSummary*) (evt -> GetIngridModHit(i, mod, cyc) );
	  if(inghitsum->cyc==-2)continue; //This is killed hit 

	  //#ifdef STUDY_BADCHANNEL
	  //if( Is_Bad_Channel( inghitsum ) )
	  //continue;
	  //#endif

	  inghitsum -> addbasicrecon = false;


	  hit.clear();
	  hit.id    = i;
	  hit.pe    = inghitsum -> pe;
	  hit.time  = inghitsum -> time;
	  hit.view  = inghitsum -> view;
	  hit.pln   = inghitsum -> pln;
	  hit.ch    = inghitsum -> ch;
	  hit.posxy = inghitsum -> xy;
	  hit.posz  = inghitsum -> z;
	  allhit.push_back(hit);
	}
	fSortTime(allhit);

	fGetTNearHit(allhit);
	for(int i=0; i<ninghit; i++){
	  inghitsum = (IngridHitSummary*) (evt -> GetIngridModHit(i, mod, cyc) );
	  inghitsum->tnearhit = allhit[i].tnearhit;
	}

 
	hitclster.clear();
	while(fFindTimeClster(allhit, hitclster, fcTime, 3)){	   
	  fSortTime(hitclster);
	  //#### Fill the data to IngridBasicRecon class ######
	  //###################################################
	  recon  -> Clear();

	  recon  -> vinternal = false;//added 2010/09/21 for tracking efficiency study
	  for(Int_t i = 0; i < hitclster.size(); i++){
	    inghitsum_t  = (IngridHitSummary*)evt->GetIngridModHit( hitclster[i].id, mod, cyc );
	    inghitsum_t -> addbasicrecon = true; 
	    inghitsum_t -> tnearhit = hitclster[i].tnearhit;
	    recon -> AddIngridHit( inghitsum_t );
	    
	    if( inghitsum_t -> pe > 6.5 ){
	      if( inghitsum_t -> pln == 0  || inghitsum_t -> pln >= 11)
		recon -> vinternal = true;
	      else if( inghitsum_t -> pln >= 1  && inghitsum_t -> pln <= 10){
		if( inghitsum_t -> ch < 2 || inghitsum_t->ch > 21 ){
		  recon -> vinternal = true;
		}
	      }
	    }

	  }

	  recon   -> clstime = fcTime;
	  recon   -> hitmod  = mod;
	  recon   -> hitcyc  = cyc;

	  evt     -> AddBasicRecon( recon );
	  hitclster.clear(); //Reset hit clster

	 
	}//Find Time Clster
      }//cyc
    }//mod
    wsummary = evt;
    wtree -> Fill();
  }//Event Loop
  //######## Write and Close ####################
  wtree  -> Write();
  wfile  -> Write();
  wfile  -> Close();

}
 
const int bad_num = 19;
int       bad_id[bad_num][4] =
  {// mod, view,  pln,  hh, 
    {   0,    0,    0,  12 },
    {   1,    0,    5,  12 },
    {   2,    0,    5,   0 },
    {   3,    1,    5,  12 },
    {   4,    0,   10,  12 },

    {   5,    1,    2,   0 },
    {   5,    1,    4,   2 },
    {   5,    1,    4,  18 },
    {   5,    1,    7,   9 },

    {   6,    0,   13,  13 },
    {   6,    1,    8,  14 },

    {   7,    0,   13,  13 },
    {   7,    1,    8,  14 },
    {   7,    1,    9,  15 },

    {   8,    0,   13,  12 },
    {   9,    1,    1,  14 },
    {  10,    0,    5,   0 },
    {  11,    1,    0,  12 },
    {  12,    0,    5,  13 },


  };
bool Add_Bad_Channel(){
  bad_mod. clear();
  bad_view.clear();
  bad_pln. clear();
  bad_ch.  clear();
  for(int ibad=0; ibad < bad_num; ibad++){
    bad_mod.  push_back ( bad_id[ibad][0] );
    bad_view. push_back ( bad_id[ibad][1] );
    bad_pln.  push_back ( bad_id[ibad][2] );
    bad_ch.   push_back ( bad_id[ibad][3] );
  }
}

bool Is_Bad_Channel(IngridHitSummary* thit){
  for(int ibad=0; ibad < bad_num; ibad++){
    if( thit -> mod  == bad_id[ibad][0]  &&
	thit -> view == bad_id[ibad][1]  &&
	thit -> pln  == bad_id[ibad][2]  &&
	thit -> ch   == bad_id[ibad][3]
	)
      return true;
  }
  return false;
}
