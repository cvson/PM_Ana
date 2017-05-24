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
#include <TMath.h>
//##### INGRID Library #########
#include "INGRIDEVENTSUMMARY.h"
#include "IngridHitSummary.h"
#include "IngridSimHitSummary.h"
//##### INGRID Software ########
#include "INGRID_Dimension.cxx"

INGRID_Dimension* fINGRID_Dimension;

FileStat_t fs;

int main(int argc,char *argv[]){
  int NDUMMY= 10; //### Poisson mean value of # of noise hit at each module
  TROOT        root ("GUI","GUI");
  TApplication app  ("App",0,0);
  fINGRID_Dimension = new INGRID_Dimension();
  int  run_number;
  int  sub_run_number;
  int  c=-1;
  char FileName[300], AddFile[300], NewFile[300];
  bool MC = false;
  while ((c = getopt(argc, argv, "mr:s:n:")) != -1) {
    switch(c){
    case 'n':
      NDUMMY=atoi(optarg);
      break;
    case 'r':
      run_number=atoi(optarg);
      break;
    case 's':
      sub_run_number=atoi(optarg);
      break;
    case 'm':
      MC = true;
      break;
    }
  }


  //#### Read file before adding noise ######
  //#########################################
 
  
  if( !MC ){
    sprintf(FileName,"/home/daq/data/dst/ingrid_%08d_%04d.root",
	    run_number, sub_run_number);
    sprintf(AddFile,"/home/daq/data/dst/ingrid_%08d_%04d.root",
	    run_number, sub_run_number);
    sprintf(NewFile, "/home/daq/data/add_noise/ingrid_%08d_%04d_noiseadd%d.root",
	    run_number, sub_run_number, NDUMMY);
  }




  else if( MC ){
    sprintf(FileName,"/home/akira.m/scraid0/INGRID_MC/10d/numu/track_wnoise/track_10d_numu_fe_nd%d_%d.root",
	    sub_run_number, run_number);

    sprintf(AddFile,"/home/akira.m/scraid0/INGRID_MC/10d/numu/track_wonoise_wbadch/track_10d_numu_fe_nd%d_%d.root",
	    sub_run_number, run_number);

    sprintf(NewFile,"/home/ingrid/test.root",
	    sub_run_number, run_number, NDUMMY);
  }

  if(gSystem->GetPathInfo(FileName,fs)){
    cout<<"Cannot open file "<<FileName<<endl;
    exit(1);
  }
  if(gSystem->GetPathInfo(AddFile,fs)){
    cout<<"Cannot open file "<<AddFile<<endl;
    exit(1);
  }


  TFile*            rfile = new TFile(FileName,"read");
  TTree*             tree = (TTree*)rfile -> Get("tree");
  IngridEventSummary* summary = new IngridEventSummary();
  TBranch*          EvtBr = tree->GetBranch("fDefaultReco.");
  EvtBr                   ->SetAddress(&summary);
  tree                    ->SetBranchAddress("fDefaultReco.", &summary);
  int                nevt = (int)tree -> GetEntries();
  //cout << "Total # of events = " << nevt <<endl;

  TFile*            afile = new TFile(AddFile,"read");
  TTree*            atree = (TTree*)afile -> Get("tree");
  IngridEventSummary* asummary = new IngridEventSummary();
  TBranch*          aEvtBr = tree->GetBranch("fDefaultReco.");
  aEvtBr                   ->SetAddress(&asummary);
  atree                    ->SetBranchAddress("fDefaultReco.", &asummary);
  int                nevt2 = (int)atree -> GetEntries();
  //cout << "Total # of events = " << nevt2 <<endl;

  int Nevt;
  if(nevt<nevt2) Nevt=nevt;
  else           Nevt=nevt2;
  cout << "Total # of events = " << Nevt <<endl;

  //#### Create file after adding noise ######
  //##########################################
  TFile*              wfile    = new TFile(NewFile,"recreate");
  TTree*              wtree    = new TTree("tree","tree");
  IngridEventSummary* wsummary = new IngridEventSummary(); 
  wtree              -> Branch   ("fDefaultReco.","IngridEventSummary", 
				 &wsummary,  64000,   99);

  int      startcyc, endcyc;
  if(!MC){
    startcyc = 0; endcyc = 23;
  }
  else if(MC){
    startcyc = 4; endcyc = 5;
  }
 
  IngridHitSummary* inghitsum;  IngridHitSummary* inghitsum2;
  for(int ievt = 0; ievt < Nevt; ++ievt){
  
    if(ievt%1000==0)cout<<"Run#"<<run_number<<"\tadd noise event:"<<ievt<<endl;
    summary -> Clear();
    asummary -> Clear();
    wsummary-> Clear();
    atree   -> GetEntry(ievt);
    tree    -> GetEntry(ievt);
    for(int mod=0; mod<16; mod++){
     
      for(int cyc=startcyc; cyc<endcyc; cyc++){
	
	int ninghit = summary -> NIngridModHits(mod, cyc);
	int ndummyhit = asummary -> NIngridModHits(mod, cyc);

	//#### Set true hit's dummy flag = false ###
	//##########################################
        for(int i=0; i<ninghit; i++){
          inghitsum   = (IngridHitSummary*) (summary -> GetIngridModHit(i, mod, cyc) );
	  inghitsum  -> dummy = false;
	}


	//#### generate dummy noise hit ####
	//##################################
	for(int idummyhit = 0; idummyhit < ndummyhit; idummyhit++){
          //inghitsum   = (IngridHitSummary*) (asummary -> GetIngridModHit(idummyhit, mod, cyc) );

	  inghitsum = new IngridHitSummary();
	  int tpe   = 0;//test
	  int ttime = 0;//test

	  inghitsum -> pe    = tpe;                     //### p.e.
	  if(!MC)
	    inghitsum -> time  = ttime + 580 * cyc;     //### time
	  else if(MC)
	    inghitsum -> time  = ttime - 200 - 320;           //### time


	  int view=0, pln=0, ch=0;                            //### channel ID

	  inghitsum -> pln   = pln;
	  inghitsum -> view  = view;
	  inghitsum -> ch    = ch;
	  inghitsum -> mod   = mod;
	  inghitsum -> cyc   = cyc;
	  if( MC )
	    inghitsum -> cyc   = -1;
	  double xy, z;
	  fINGRID_Dimension -> get_posXY( mod, view, pln, ch,
					  &xy, &z);
	  inghitsum -> xy    = xy;
	  inghitsum ->  z    =  z;
	  inghitsum -> dummy = true;                   //### dummy flag


	  //#### if there is noise hit faster than true hit, ####
	  //#### mask the true hit(temporary cyc = -2) ##########
	  //#####################################################


	  //wsummary -> AddIngridModHit(inghitsum,mod, cyc);
	  summary   -> AddIngridModHit(inghitsum,mod, cyc);
	}//idummyhit



	for(int ihit1=0; ihit1 < summary->NIngridModHits(mod,cyc); ihit1++){
	  inghitsum = (IngridHitSummary*)(summary->GetIngridModHit(ihit1, mod, cyc));


	  for(int ihit2=ihit1+1; ihit2 < summary->NIngridModHits(mod,cyc); ihit2++){
	    inghitsum2 = (IngridHitSummary*)(summary->GetIngridModHit(ihit2, mod, cyc));

	    if( inghitsum -> pln  == inghitsum2  -> pln  &&
		inghitsum -> ch   == inghitsum2  -> ch   &&
		inghitsum -> view == inghitsum2  -> view 

		){
	      if( inghitsum -> time > inghitsum2 -> time ){
		inghitsum  -> cyc = -2;

	      }
	      if( inghitsum -> time < inghitsum2 -> time ){
		inghitsum2 -> cyc = -2;

	      }

	    }
	  }
	}



      }//cyc
    }//mod
    wsummary = summary;
    wtree -> Fill();
  }


  wtree -> Write();
  wfile -> Write();
  wfile -> Close();
  //app.Run();
}
