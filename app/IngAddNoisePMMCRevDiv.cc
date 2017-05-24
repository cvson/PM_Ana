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
//##### INGRID Software ########
#include "INGRID_Dimension.cxx"
#include "INGRID_BadCh_mapping.cxx"
INGRID_BadCh_mapping* IngBadChMap;

bool Is_Bad_Channel(IngridHitSummary* thit);


INGRID_Dimension* fINGRID_Dimension;

FileStat_t fs;

int main(int argc,char *argv[]){
  Double_t NDUMMY= 5.05; //### Poisson mean value of # of noise hit at each module
  Double_t NDUMMY2= 12.48; //### Poisson mean value of # of noise hit at each module
  TROOT        root ("GUI","GUI");
  TApplication app  ("App",0,0);
  fINGRID_Dimension = new INGRID_Dimension();
  int  run_number;
  int  sub_run_number;
  int  c=-1;
  int seed=0;
  char FileName[300], Output[300];
  bool MC = false;
  int Nini=0;
  int Nend=100;
  while ((c = getopt(argc, argv, "f:o:n:s:i:")) != -1) {
    switch(c){
    case 'f':
      sprintf(FileName, "%s", optarg);
      break;
    case 'o':
      sprintf(Output, "%s", optarg);
      break;
    case 'n':
      NDUMMY2=atof(optarg);
      break;
    case 's':
      seed=atoi(optarg);
      break;
    case 'i':
      Nini=atoi(optarg);
      Nend=Nini+1;
      break;
    }
  }


  //#### Read file before adding noise ######
  //#########################################
 
  IngridEventSummary* summary = new IngridEventSummary();
  /*
  if( !MC ){
    sprintf(FileName,"/home/daq/data/dst/ingrid_%08d_%04d.root",
	    run_number, sub_run_number);
    sprintf(Output, "/home/daq/data/add_noise/ingrid_%08d_%04d_noiseadd%d.root",
	    run_number, sub_run_number, NDUMMY);
  }
  else if( MC ){
    sprintf(FileName,"/home/daq/data/mc_data/v3r1/ingmc_numu_fe_nd%d_horn250ka_%d.root",
	    sub_run_number, run_number);
    sprintf(Output,"/home/daq/data/mc_data/add_noise/ingmc_numu_fe_nd%d_horn250ka_%d_noiseadd%d.root",
	    sub_run_number, run_number, NDUMMY);
  }
  */

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

  //#### Create file after adding noise ######
  //##########################################
  TFile*              wfile    = new TFile(Output,"recreate");
  TTree*              wtree    = new TTree("tree","tree");
  IngridEventSummary* wsummary = new IngridEventSummary(); 
  wtree              -> Branch   ("fDefaultReco.","IngridEventSummary", 
				 &wsummary,  64000,   99);



  TF1*     fHtime = new TF1("fHtime","0.-0.096*x*x+133*x-32800", 320, 750);
  TF1*     fHpe   = new TF1("fHpe"  ,"gaus", -5, 5);
  fHpe           -> SetParameters(1, 0, 0.3);
  TRandom3 r; //### random number sheed
  r.SetSeed(seed);
  int      startcyc = 4; //MC data is fille in cycle 4 
  int      endcyc   = 5;

  Nini=(Nini*nevt)/100;
  Nend=(Nend*nevt)/100;
 
  IngridHitSummary* inghitsum;  IngridHitSummary* inghitsum2;
  //for(int ievt = 0; ievt < nevt; ++ievt){
  for(int ievt = Nini; ievt < Nend; ++ievt){
    if(ievt%1000==0)cout<<"\tadd noise event:"<<ievt<<endl;
    //cout<<"Run#"<<run_number<<"\tadd noise event:"<<ievt<<endl;
    summary -> Clear();
    wsummary-> Clear();
    tree    -> GetEntry(ievt);
    for(int mod=0; mod<17; mod++){
      if(mod==14||mod==15)continue;
      for(int cyc=startcyc; cyc<endcyc; cyc++){
	int ninghit = summary -> NIngridModHits(mod, cyc);
	//#### Set true hit's dummy flag "dummy" = false ###
	//##################################################
        for(int i=0; i<ninghit; i++){
          inghitsum   = (IngridHitSummary*) (summary -> GetIngridModHit(i, mod, cyc) );
	  inghitsum  -> dummy = false;
	}

	//#### generate dummy noise hit with dummy = true####
	//###################################################
	if(mod!=16)
	  NDUMMY=5.05;
	else
	  NDUMMY=NDUMMY2;

	int ndummyhit = r.Poisson( NDUMMY );

	for(int idummyhit = 0; idummyhit < ndummyhit; idummyhit++){
	  inghitsum   = new IngridHitSummary();
	  double tpe  = (int)(r.Exp(-1./TMath::Log(0.26))) + 3 + fHpe->GetRandom() ; //### generate p.e.


	  int ttime = fHtime->GetRandom();              //### generate time

	  inghitsum -> time  = ttime - 200 - 320;// - 580 * 5+50;       //### add offset 
	  //cout << inghitsum -> time << endl;
	  int view, pln, ch,tch;                            //### channel ID
	  
	  if(mod!=16){
	    tch   = r.Uniform(0, 616+(1e-11) );

	    if(tch < 528){//Tracking plane
	      pln  = (int)( tch / (48) );
	      view = (int)((tch - pln*48)/24);
	      ch   = (int)( tch-pln*48-24*view );
	    }
	    else if( tch >= 528 ){//VETO
	      tch = tch - 528;
	      pln = 11 + (int)( tch/(22) );
	      if(pln == 11 || pln == 12)
		view=1;
	      else
		view=0;
	      ch = tch - 22 * (pln-11);
	    }
	  }
	  else{
            tch   = r.Uniform(0, 1204+(1e-11) );

            if(tch < 48){//Front plane
              pln  = 0;
              view = (int)( tch/24 );
              ch   = (int)( tch-24*view );
            }
            else if(tch < 1136){//Tracking plane
              tch = tch - 48;
              pln  = (int)( tch / (64) );
              view = (int)((tch - pln*64)/32);
              ch   = (int)( tch-pln*64-32*view );
	      pln++;
            }
            else if( tch >= 1136 ){//VETO
              tch = tch - 1136;
              pln = 18 + (int)( tch/(17) );
              if(pln == 19 || pln == 21)
                view=1;
              else
                view=0;
              ch = tch - 17 * (pln-18);
            }

	  }

	
	  //Fill p.e., time, channel map
	  inghitsum -> pe    = tpe;
	  inghitsum -> lope  = tpe;
	  inghitsum -> pln   = pln;
	  inghitsum -> view  = view;
	  inghitsum -> ch    = ch;
	  inghitsum -> mod   = mod;
	  inghitsum -> cyc   = -1;

	  double xy, z;
	  fINGRID_Dimension -> get_posXY( mod, view, pln, ch,
					  &xy, &z);
	  inghitsum -> xy    = xy;
	  inghitsum ->  z    =  z;
	  inghitsum -> dummy = true;                   //### dummy flag


	  //wsummary -> AddIngridModHit(inghitsum,mod, cyc);
	  summary   -> AddIngridModHit(inghitsum,mod, cyc);
	}//idummyhit

	//#### if there is noise hit faster than true hit, ####
	//#### mask the true hit(temporary cyc = -2) ##########
	//#####################################################

	for(int ihit1=0; ihit1 < summary->NIngridModHits(mod,cyc); ihit1++){
	  inghitsum = (IngridHitSummary*)(summary->GetIngridModHit(ihit1, mod, cyc));
	  for(int ihit2=ihit1+1; ihit2 < summary->NIngridModHits(mod,cyc); ihit2++){
	    inghitsum2 = (IngridHitSummary*)(summary->GetIngridModHit(ihit2, mod, cyc));

	    if( inghitsum -> pln  == inghitsum2  -> pln  &&
		inghitsum -> ch   == inghitsum2  -> ch   &&
		inghitsum -> view == inghitsum2  -> view ){
	      if( (inghitsum -> time) > (inghitsum2 -> time) )
		inghitsum  -> cyc = -2;
	      else if( (inghitsum -> time) < (inghitsum2 -> time) )
		inghitsum2 -> cyc = -2;
	    }//same channel
	  }
	}
	//### Add Bad channel
	for( int ihit = 0; ihit < summary->NIngridModHits(mod, cyc); ihit++ ){
	  inghitsum = (IngridHitSummary*)( summary -> GetIngridModHit(ihit, mod, cyc) );
	  if( !IngBadChMap->badchannel( inghitsum->mod, inghitsum->view,
					inghitsum->pln, inghitsum->ch ))
	    continue;

	  //#### if this is bad channel #########################
	  //#### mask the true hit(temporary cyc = -3) ##########
	  //#####################################################
	  inghitsum -> cyc = -3;
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
 
