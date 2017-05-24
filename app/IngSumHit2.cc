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
  char FileName[300], AddFile[300], NewFile1[300], NewFile2[300], NewFile3[300];
  bool MC = false;
  while ((c = getopt(argc, argv, "mr:s:t:n:")) != -1) {
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
    case 't':
      sub_run_number2=atoi(optarg);
      break;
    case 'm':
      MC = true;
      break;
    }
  }


  //#### Read file before adding noise ######
  //#########################################
 
  
  if( !MC ){
    sprintf(FileName,"/home/akira.m/scraid0/INGRID_MC/10d/numu/ingmc/ingmc_10d_numu_fe_nd%d_%d.root",
	    run_number, sub_run_number);

    //sprintf(AddFile,"/home/akira.m/scraid0/INGRID_MC/10d/numu/ingmc/ingmc_10d_numu_fe_nd%d_%d.root",
    sprintf(AddFile,"/home/ingrid/scraid0/kikawa/bgmc/nd7_nd%d_10c_rock_%d.root",
	    run_number, sub_run_number2);

    sprintf(NewFile1,"/export/scraid0/data/kikawa/new/ingmc_10d_numu_fe_nd%d_%d_%d.root",
	    run_number, sub_run_number, sub_run_number2);
    sprintf(NewFile2,"/export/scraid0/data/kikawa/old1/ingmc_10d_numu_fe_nd%d_%d_%d.root",
	    run_number, sub_run_number, sub_run_number2);
    sprintf(NewFile3,"/export/scraid0/data/kikawa/old2/ingmc_10d_numu_fe_nd%d_%d_%d.root",
	    run_number, sub_run_number, sub_run_number2);
  }



  else if( MC ){
    sprintf(FileName,"/home/akira.m/scraid0/INGRID_MC/10d/numu/ingmc/ingmc_10d_numu_fe_nd%d_%d.root",
	    run_number, sub_run_number);

    //sprintf(AddFile,"/home/akira.m/scraid0/INGRID_MC/10d/numu/ingmc/ingmc_10d_numu_fe_nd%d_%d.root",
    sprintf(AddFile,"/home/ingrid/scraid0/kikawa/bgmc/nd7_nd%d_10c_rock_%d.root",
	    run_number, sub_run_number2);

    sprintf(NewFile1,"/export/scraid0/data/kikawa/new/ingmc_10d_numu_fe_nd%d_%d_%d.root",
	    run_number, sub_run_number, sub_run_number2);
    sprintf(NewFile2,"/export/scraid0/data/kikawa/old1/ingmc_10d_numu_fe_nd%d_%d_%d.root",
	    run_number, sub_run_number, sub_run_number2);
    sprintf(NewFile3,"/export/scraid0/data/kikawa/old2/ingmc_10d_numu_fe_nd%d_%d_%d.root",
	    run_number, sub_run_number, sub_run_number2);
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
  TBranch*          aEvtBr = atree->GetBranch("fDefaultReco.");
  aEvtBr                   ->SetAddress(&asummary);
  atree                    ->SetBranchAddress("fDefaultReco.", &asummary);
  int                nevt2 = (int)atree -> GetEntries();
  //cout << "Total # of events = " << nevt2 <<endl;

  int Nevt,Nevt2;

  if(nevt>nevt2) {Nevt=nevt;Nevt2=nevt2;}
  else           {Nevt=nevt2;Nevt2=nevt;}
  cout << "Total # of events = " << Nevt <<endl;

  //#### Create file after adding noise ######
  //##########################################
  TFile*              wfile    = new TFile(NewFile1,"recreate");
  TTree*              wtree    = new TTree("tree","tree");
  IngridEventSummary* wsummary = new IngridEventSummary(); 
  wtree              -> Branch   ("fDefaultReco.","IngridEventSummary", 
				 &wsummary,  64000,   99);

  TFile*              wfile1    = new TFile(NewFile2,"recreate");
  TTree*              wtree1    = new TTree("tree","tree");
  IngridEventSummary* wsummary1 = new IngridEventSummary(); 
  wtree1              -> Branch   ("fDefaultReco.","IngridEventSummary", 
				 &wsummary1,  64000,   99);

  TFile*              wfile2    = new TFile(NewFile3,"recreate");
  TTree*              wtree2    = new TTree("tree","tree");
  IngridEventSummary* wsummary2 = new IngridEventSummary(); 
  wtree2              -> Branch   ("fDefaultReco.","IngridEventSummary", 
				 &wsummary2,  64000,   99);

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
    wsummary1-> Clear();
    wsummary2-> Clear();

    if(nevt>nevt2) {
    tree    -> GetEntry(ievt);    
    atree   -> GetEntry(ievt%Nevt2);
    }
    else           {
    tree    -> GetEntry(ievt%Nevt2);
    atree   -> GetEntry(ievt);
    }

    //atree   -> GetEntry(ievt);
    //tree    -> GetEntry(ievt);

    wsummary1 = summary;
    wtree1 -> Fill();
    wsummary2 = asummary;
    wtree2 -> Fill();



    IngridSimVertexSummary* simver =  (IngridSimVertexSummary*)( summary -> GetSimVertex(0) );
    IngridSimVertexSummary* simver2 =  (IngridSimVertexSummary*)( asummary -> GetSimVertex(0) );

    simver->norm *= simver2->norm;
    simver->totcrsne *= simver2->totcrsne;

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
  wtree1 -> Write();
  wfile1 -> Write();
  wtree2 -> Write();
  wfile2 -> Write();
  wfile -> Close();
  wfile1 -> Close();
  wfile2 -> Close();
  //app.Run();
}
