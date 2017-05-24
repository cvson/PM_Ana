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
#include "BeamInfoSummary.h"
#include "IngridBasicReconSummary.h"
#include "IngridSimParticleSummary.h"
#include "IngridSimVertexSummary.h"
#include "IngridTrackSummary.h"
#include "NeutInfoSummary.h"


//##### INGRID Software ########
//#include "INGRID_Dimension.cxx"


//INGRID_Dimension* fINGRID_Dimension;

FileStat_t fs;

int main(int argc,char *argv[]){
  int NDUMMY= 10; //### Poisson mean value of # of noise hit at each module
  TROOT        root ("GUI","GUI");
  TApplication app  ("App",0,0);
  //fINGRID_Dimension = new INGRID_Dimension();
  int  run_number;
  int  sub_run_number, sub_run_number2;
  int  c=-1;
  char FileName[300], Output[300], AddFile[300], NewFile1[300], NewFile2[300], NewFile3[300];
  bool MC = false;
  while ((c = getopt(argc, argv, "f:o:")) != -1) {
    switch(c){
    case 'f':
      sprintf(FileName,"%s",optarg);
      //renamef = true;
      break;
    case 'o':
      sprintf(Output,"%s",optarg);
      //rename  = true;
      break;
    }
  }


  //#### Read file before adding noise ######
  //#########################################
 
  

  if(gSystem->GetPathInfo(FileName,fs)){
    cout<<"Cannot open file "<<FileName<<endl;
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

  TFile*            afile = new TFile(FileName,"read");
  TTree*            atree = (TTree*)afile -> Get("tree");
  IngridEventSummary* asummary = new IngridEventSummary();
  TBranch*          aEvtBr = atree->GetBranch("fDefaultReco.");
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
  TFile*              wfile    = new TFile(Output,"recreate");
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

    atree   -> GetEntry((ievt+1)%Nevt);
    tree    -> GetEntry(ievt);


    BeamInfoSummary* binfo  = (BeamInfoSummary*)( summary  -> GetBeamSummary(0) );
    BeamInfoSummary* binfo2 = (BeamInfoSummary*)( asummary -> GetBeamSummary(0) );


    for(int index=0; index<5; index++){
      for(int jndex=0; jndex<9; jndex++){
	binfo-> ct_np[index][jndex] += binfo2-> ct_np[index][jndex];
      }
    }




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
          inghitsum   = (IngridHitSummary*) (asummary -> GetIngridModHit(idummyhit, mod, cyc) );

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
		inghitsum2  -> pe += inghitsum  -> pe;
	      }
	      if( inghitsum -> time < inghitsum2 -> time ){
		inghitsum2 -> cyc = -2;
		inghitsum  -> pe += inghitsum2  -> pe;

	      }

	    }
	  }
	}



      }//cyc
    }//mod


    /*
    IngridSimVertexSummary* simver =  (IngridSimVertexSummary*)( summary -> GetSimVertex(0) );
    wsummary -> AddSimVertex(simver);
    */

    wsummary = summary;

    wtree -> Fill();
  }


  wtree -> Write();
  wfile -> Write();
//wtree1 -> Write();
//wfile1 -> Write();
//wtree2 -> Write();
//wfile2 -> Write();
  wfile -> Close();
//wfile1 -> Close();
//wfile2 -> Close();
  //app.Run();
}
