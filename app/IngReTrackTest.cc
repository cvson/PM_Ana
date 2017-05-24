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
//#include "setup.hxx"
//#include "INGRID_Dimension.hxx"
//#include "root_setup.hxx"
#include "INGRIDEVENTSUMMARY.h"
#include "IngridHitSummary.h"
#include "IngridSimHitSummary.h"
#include "IngridSimVertexSummary.h"
#include "IngridSimParticleSummary.h"
#include "BeamInfoSummary.h"
#include "IngridBasicReconSummary.h"
#include "IngridTrackSummary.h"

#include "ReconTrackBasic.cxx"
#include "IngEvtDisp.cxx"

ReconTrackBasic* frecontrack;
IngEvtDisp* fingevtdisp;

//#define DEBUG


const static float  TestAreaOffset    = 2;
const static float  MatchDiff         = 10;

int main(int argc,char *argv[]){
  int itry=0, iok=0;
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  frecontrack = new ReconTrackBasic();
  fingevtdisp = new IngEvtDisp();

  char buff1[300], Output[300];
  int run_number;
  int sub_run_number=0;
  int n;
  Int_t c=-1;  char FileName[300];
  int    TestAreaStartPln  = 2; //####
  int    TestAreaEndPln    = 6; //####

  while ((c = getopt(argc, argv, "f:o:")) != -1) {
    switch(c){

    case 'f':
      sprintf(FileName,"%s",optarg);
      break;
    case 'o':
      sprintf(Output,"%s",optarg);
      break;

    }
  }
  FileStat_t fs;
  ifstream timing;
  float  TestAreaStartPosz = PlnZ[ TestAreaStartPln ] ;
  float  TestAreaEndPosz   = PlnZ[ TestAreaEndPln ]   ;


  //######## read root file #############################
  //#####################################################
  cout<<"reading "<<FileName<<"....."<<endl;
  if(gSystem->GetPathInfo(FileName,fs)){
    cout<<"Cannot open file "<<FileName<<endl;
    exit(1);
  }
  IngridEventSummary* evt = new IngridEventSummary();
  TFile*            rfile = new TFile(FileName,"read");
  TTree*             tree = (TTree*)rfile -> Get("tree");
  TBranch*          EvtBr = tree->GetBranch("fDefaultReco.");
  EvtBr                   ->SetAddress(&evt);
  tree                    ->SetBranchAddress("fDefaultReco.", &evt);
  int                nevt = (int)tree -> GetEntries();
  cout << "Total # of events = " << nevt <<endl;


  //######## recreated root file ########################
  //#####################################################
  TFile*            wfile = new TFile(Output,
				      "recreate");
  TTree*            wtree = new TTree("tree","tree");
  wtree -> SetMaxTreeSize(10000000000);
  IngridEventSummary* wsummary = new IngridEventSummary(); 
  wtree              -> Branch   ("fDefaultReco.","IngridEventSummary", 
				 &wsummary,  64000,   99);




  IngridHitSummary*          inghitsum;
  IngridHitSummary*        inghitsum_t; 
  IngridBasicReconSummary*  basicrecon;
  BeamInfoSummary*            beaminfo;
  int testcount  = 0;
  int testcount2 = 0;
#ifdef DEBUG
  TCanvas* evtdisp = new TCanvas("c1","c1",10,10,700,500);
#endif
  TRef  fIngridHit[INGRIDHIT_MAXHITS];
  int ntrkok    = 0;
  int nretrkok0 = 0;
  int nretrkok1 = 0;

  int highnuE   = 0;

    
  for(int ievt=0; ievt<nevt; ievt++){


    if(ievt%1000==0)cout<<"-----EVENT # "<<ievt<<endl;
   
    //cout<<"-----EVENT # "<<ievt<<endl;
    evt                -> Clear();
    wsummary           -> Clear();
    tree               -> GetEntry(ievt);
    int nbrecon  = evt -> NIngridBasicRecons();





    /*
    if(simver->nuE>5)
      highnuE++;
    if(highnuE==2){
      highnuE=0;
      continue;
    }
    */
 
    for(int ibas=0; ibas<evt->NIngridBasicRecons(); ibas++){
      basicrecon = (IngridBasicReconSummary*)evt->GetBasicRecon( ibas );
      basicrecon -> retracktest = 0;
      for(int itest=0; itest<10; itest++){
      //for(int itest=0; itest<1; itest++){
	switch(itest){
	  //case '0':
	case 0:
	  TestAreaStartPln=2;
	  TestAreaEndPln  =4;
	  break;
	  //case '1':
	case 1:
	  TestAreaStartPln=2;
	  TestAreaEndPln  =5;
	  break;
	  //case '2':
	case 2:
	  TestAreaStartPln=2;
	  TestAreaEndPln  =6;
	  break;
	  //case '3':
	case 3:
	  TestAreaStartPln=2;
	  TestAreaEndPln  =7;
	  break;
	  //case '4':
	case 4:
	  TestAreaStartPln=2;
	  TestAreaEndPln  =8;
	  break;
	  //case '5':
	case 5:
	  TestAreaStartPln=2;
	  TestAreaEndPln  =9;
	  break;
	  //case '6':
	case 6:
	  TestAreaStartPln=3;
	  TestAreaEndPln  =8;
	  break;
	case 7:
	  TestAreaStartPln=4;
	  TestAreaEndPln  =6;
	  break;
	case 8:
	  TestAreaStartPln=3;
	  TestAreaEndPln  =5;
	  break;
	case 9:
	  TestAreaStartPln=7;
	  TestAreaEndPln  =9;
	  break;
	}
  

	float  TestAreaStartPosz = PlnZ[ TestAreaStartPln ] ;
	float  TestAreaEndPosz   = PlnZ[ TestAreaEndPln ]   ;

	bool hastrk = basicrecon -> hastrk;

      if( hastrk ){   //##### First Tracking Successfull
	IngridTrackSummary* trk0 = (IngridTrackSummary*) basicrecon->GetTrack(0);
	IngridTrackSummary* trk1 = (IngridTrackSummary*) basicrecon->GetTrack(1);

	//#### if retracking situation is satified, #########
	//#### re-tracking test is applied  #################
	bool retracking = false;
	float endxz     = trk0 -> vtxf[2];
	float endyz     = trk1 -> vtxf[2];
	float startxz   = trk0 -> vtxi[2];
	float startyz   = trk1 -> vtxi[2];
	float endx      = trk0 -> vtxf[0];
	float endy      = trk1 -> vtxf[0];
	float startx    = trk0 -> vtxi[0];
	float starty    = trk1 -> vtxi[0];
	if( startxz   < TestAreaStartPosz - TestAreaOffset &&  
	    endxz     > TestAreaEndPosz   + TestAreaOffset &&
	    startyz   < TestAreaStartPosz - TestAreaOffset &&  
	    endyz     > TestAreaEndPosz   + TestAreaOffset &&
	    startx    >=   0               &&
	    startx    <= 120               &&
	    starty    >=   0               &&
	    starty    <= 120               &&
	    endx      >=   0               &&
	    endx      <= 120               &&
	    endy      >=   0               &&
	    endy      <= 120               &&
	    //basicrecon   -> ontime         &&
	    basicrecon   -> matchtrk       &&
	    !(basicrecon -> vetowtracking) &&
	    !(basicrecon -> edgewtracking)  
	  )
	  retracking = true;

	//#### Re-tracking test start #####
	//#################################
	
	bool ok = false;
	if( retracking ){
	  itry++;
	  basicrecon->retracktest = basicrecon -> retracktest + (int)(pow(2, itest*2));


	  bool display = true;
#ifdef DEBUG
	  cout << "------------ First Tracking success -------------" << endl;

#endif

	  //#### Add IngridHit to TRef which is only within Test Area #####
	  //###############################################################
	  int refhits    = basicrecon -> nhits;
	  int nhits      = 0;
	  
	  
	  for( int ihit=0; ihit < refhits; ihit++ ){

	    IngridHitSummary* b = basicrecon -> GetIngridHit(ihit);

	    if( b -> pln >= TestAreaStartPln &&
		b -> pln <= TestAreaEndPln ){ 
	      fIngridHit[nhits] = 0;
	      fIngridHit[nhits] = TRef( (IngridHitSummary*)basicrecon -> GetIngridHit(ihit) );
	      nhits++;
	    }
	  }
	  for(int ini=nhits; ini<INGRIDHIT_MAXHITS; ini++){
	    fIngridHit[nhits]=0;
	  }
	  
#ifdef DEBUG
	  evtdisp     -> Clear();
	  fingevtdisp -> Draw_Module    ( *evtdisp );
	  fingevtdisp -> Draw_Hit_A     ( *basicrecon,0.06 );
	  //fingevtdisp -> Draw_BeamInfo  ( *beaminfo );
#endif
	  frecontrack -> Clear();

	  if(nhits<5)
	    continue;


	  if(nhits>0)
	    frecontrack -> SetHit(fIngridHit, nhits);
	  
#ifdef DEBUG
	  //### Draw First Track #####
	  //##########################
	  TF1* ff0    =  new TF1("ff0", "pol1",0, 130);
	  ff0         -> SetParameter(0, trk0->etx);
	  ff0         -> SetParameter(1, trk0->tx);
	  fingevtdisp -> Draw_Line(*ff0, 0, trk0->vtxi[2], trk0->vtxf[2],1,3);
	  TF1* ff1    =  new TF1("ff1", "pol1",0, 130);
	  ff1         -> SetParameter(0, trk1->etx);
	  ff1         -> SetParameter(1, trk1->tx);
	  fingevtdisp -> Draw_Line(*ff1, 1, trk1->vtxi[2], trk1->vtxf[2],1,3);
#endif

	  if( frecontrack -> ReconTrack() ){
	    IngridTrackSummary* retrk0 = frecontrack -> GetTrack(0, 0);
	    IngridTrackSummary* retrk1 = frecontrack -> GetTrack(1, 0);
	 
#ifdef DEBUG
	    //### Draw ReTrack #####
	    //######################

	    TF1* f0     =  new TF1("f0", "pol1",0, 130);
	    f0          -> SetParameter(0, retrk0->etx);
	    f0          -> SetParameter(1, retrk0->tx);
	    fingevtdisp -> Draw_Line(*f0, 0, retrk0->vtxi[2], retrk0->vtxf[2],3,2);
	    TF1* f1     =  new TF1("f1", "pol1",0, 130);
	    f1          -> SetParameter(0, retrk1->etx);
	    f1          -> SetParameter(1, retrk1->tx);
	    fingevtdisp -> Draw_Line(*f1, 1, retrk1->vtxi[2], retrk1->vtxf[2],3,2);
#endif
	    /*
	    //######## Compare First Track and Re Track ####
	    //##############################################
	    //##### X 
	    //## vertex x
	    float ExpStartX   = ( trk0   -> tx ) * TestAreaStartPosz + trk0   -> etx; 
	    float ReTrkStartX = ( retrk0 -> tx ) * TestAreaStartPosz + retrk0 -> etx; 
	    float ExpEndX     = ( trk0   -> tx ) * TestAreaEndPosz   + trk0   -> etx; 
	    float ReTrkEndX   = ( retrk0 -> tx ) * TestAreaEndPosz   + retrk0 -> etx; 

	    if( fabs( ExpStartX  - ReTrkStartX ) < MatchDiff &&
		fabs( ExpEndX    - ReTrkEndX   ) < MatchDiff 
		){
	      //cout << " ****** Retracking successful X ********" << endl;
	      display = false;
	      nretrkok0++;
	      basicrecon->retracktest = basicrecon -> retracktest + (int)(pow(2, itest*3+1));

	      //basicrecon->retrackx = true;
	     
	      //cout << " ****** OK *******" << endl;
	    }
	    else{
	      //basicrecon -> retrackx[itest] = false;
	    }
	    //##### Y 
	    //## vertex 
	    float ExpStartY    = ( trk1   -> tx ) * TestAreaStartPosz + trk1   -> etx; 
	    float ReTrkStartY  = ( retrk1 -> tx ) * TestAreaStartPosz + retrk1 -> etx; 

	    float ExpEndY      = ( trk1   -> tx ) * TestAreaEndPosz   + trk1   -> etx; 
	    float ReTrkEndY    = ( retrk1 -> tx ) * TestAreaEndPosz   + retrk1 -> etx; 

	    if( fabs( ExpStartY  - ReTrkStartY ) < MatchDiff &&
		fabs( ExpEndY    - ReTrkEndY   ) < MatchDiff 
		){
	      nretrkok1++;
	      //cout << " ****** Retracking successful Y ********" << endl;
	      //cout << " ****** OK *******" << endl;
	      //basicrecon->retracky += (int)(pow(2,(itest)));
	      basicrecon->retracktest = basicrecon -> retracktest + (int)(pow(2, itest*3+2));
	      display = false;
	    }
	    else{
	      //basicrecon -> retracky[itest] = false;
	    }
	    */
	    bool Vetowtracking = false;
	    bool Edgewtracking = false;
	    for(int v=0; v<2; v++){
	      if( frecontrack -> is1stTPL(v) || frecontrack->isUVETO(v) 
		  || frecontrack -> UVETOisTrack(v))
		Vetowtracking = true;
	      if( frecontrack->UEdgeisTrack(v) )
		Edgewtracking = true;
	    }
	    //if( frecontrack -> TrkMatching() && !Edgewtracking && !Vetowtracking ){
	    if( frecontrack -> TrkMatching() ){
	      basicrecon->retracktest = basicrecon -> retracktest + (int)(pow(2, itest*2+1));	      
	      ok=true;
	      iok++;
	    }
	    //cout <<ievt << "\t" << itest << "\t"<< basicrecon->retracktest << endl;
	    /*
	    else{
	      cout << " ****** NO *******" << endl;
	      cout << "Exp Start X : " << ExpStartX  << "\t" << retrk0->vtxi[0] << endl;
	      cout << "Exp Start XZ: " << ExpStartXZ << "\t" << retrk0->vtxi[2] << endl;
	      cout << "Exp End X   : " << ExpEndX    << "\t" << retrk0->vtxf[0] << endl;
	      cout << "Exp End XZ  : " << ExpEndXZ   << "\t" << retrk0->vtxf[2] << endl;
	    }
	    */
	  } 

	  if(!ok){
	    cout << ievt << "\t" << basicrecon->hitmod << endl;
	  }
#ifdef DEBUG
	  //if(ok)continue;
	  //if(ok) cout << "ok" << endl;
	  if(ok)continue;
	  int nmod = basicrecon->hitmod;
	  int ncyc = basicrecon->hitcyc;

	  cout << evt->NIngridModHits(nmod, ncyc) <<"\t"<<evt->NIngridSimHits()<< "\t" << basicrecon->Nhits() << endl;

	  evtdisp     -> Update();
	  IngridSimVertexSummary* simver = (IngridSimVertexSummary*)evt->GetSimVertex(0);
	  //cout << (int)((simver -> z +52)/10.3)<< endl;
	  
	  //cout << "inttype = " << simver->inttype << endl;
	  /*
	  cout<< "------- all hit --------------------------------" << endl;
	  for(int ihit=0; ihit < evt->NIngridModHits(nmod,ncyc); ihit++){
	    IngridHitSummary* thit = (IngridHitSummary*)(evt->GetIngridModHit(ihit,nmod,ncyc));
	    IngridSimHitSummary* tsimhit = (IngridSimHitSummary*)(evt->GetIngridSimHit(ihit));
	    cout << "View: " << thit->view << " "
		 << "Pln: "  << thit->pln  << "\t"
		 << "Ch: "   << thit->ch  << "\t"
		 << "pe: "   << thit->pe  << "\t"
		 << "time: " << thit->time  << "\t" 
	      
		 << "pdg: "  << tsimhit->pdg  << "\t" << endl;



	  }
	  cout<< "------- clster hit --------------------------------" << endl;
	  for(int ihit=0; ihit < basicrecon->Nhits(); ihit++){
	    IngridHitSummary* thit = (IngridHitSummary*)(basicrecon->GetIngridHit(ihit));
	    cout << "View: " << thit->view << " "
		 << "Pln: "  << thit->pln  << " "
		 << "Ch: "   << thit->ch  << " "
		 << "pe: "   << thit->pe  << " "
		 << "time: " << thit->time  << endl;
	  }
	  */
	//cout << "nu E    = " << simver->nuE << endl;
	//frecontrack -> Print();
	cin.get();
#endif

	}


      }//like
      }//itest
    }//ibas

    wsummary = evt;
    wtree    ->Fill();
  }//Event Loop
 

  wtree -> Write();
  wfile -> Write();


  cout << iok << "/" << itry << endl;
}
 
