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

//#include "ReconTrack.cxx"
#include "ReconTrackBasic_rev.cxx"
#include "IngEvtDisp.cxx"

ReconTrackBasic* frecontrack;
IngEvtDisp* fingevtdisp;

//#define DEBUG
//#define DEBUG2
int main(int argc,char *argv[]){
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  frecontrack = new ReconTrackBasic();
  fingevtdisp = new IngEvtDisp();

  char buff1[300];
  int run_number;
  int sub_run_number=0;
  int n;
  Int_t c=-1;  char FileName[300];
  char Output[300]; bool rename = false;
  while ((c = getopt(argc, argv, "f:o:")) != -1) {
    switch(c){

    case 'o':
      sprintf(Output,"%s",optarg);
      rename = true;
      break;
    case 'f':
      sprintf(FileName,"%s",optarg);
      run_number=77;
      break;

    }
  }
  FileStat_t fs;
  ifstream timing;

  ofstream debugtxt("reconmubasic_debug.txt");
#ifdef DEBUG2
  ofstream txtfile("slx.txt");
#endif
  int      counter[2]={0};

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

  //#### make rootfile after analysis #####
  TFile*            wfile = new TFile(Output,"recreate");
  TTree*            wtree = new TTree("tree","tree");
  IngridEventSummary* wsummary = new IngridEventSummary(); 
  wtree              -> Branch   ("fDefaultReco.","IngridEventSummary", 
				 &wsummary,  64000,   99);
  wtree -> SetMaxTreeSize(5000000000);

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

  int ntrk=0;
#ifndef DEBUG111206
  for(int ievt=0; ievt<nevt; ievt++){
#endif
#ifdef DEBUG111206
  for(int ievt=26; ievt<27; ievt++){
#endif
    
    if(ievt%100==0)cout<<ievt<<endl;

    evt      -> Clear();
    wsummary -> Clear();
    tree     -> GetEntry(ievt);
    int nbrecon  = evt -> NIngridBasicRecons();
    //cout<<"------------- event:"<<ievt<<"\t "<<nbrecon<<"  ----------------"<<endl;
    beaminfo = (BeamInfoSummary*)evt->GetBeamSummary(0);


    for(int i=0; i<nbrecon; i++){
      basicrecon      = (IngridBasicReconSummary*)evt->GetBasicRecon( i );
      int   nactpln   = basicrecon -> nactpln;
      float layerpe   = basicrecon -> layerpe;

#ifdef DEBUG111206
      cout << "  module " << basicrecon->hitmod << "\t cyc " << basicrecon->hitcyc << endl;
#endif 
      if( nactpln    >   2 &&           
	  layerpe    > 6.5  ){
	testcount++;

	//cout << " ----- "<< i << " th basic recon." << endl;
	/*
	cout << "# of active planes =" << nactpln
	     << "\tpe/active layer ="  << layerpe
	     <<endl;
	*/

	//#### Add IngridHit to TRef #####
	//################################
	int nhit    = basicrecon -> nhits;

	for( int ihit=0; ihit<INGRIDHIT_MAXHITS; ihit++ ){
	//for( int ihit=0; ihit < nhit; ihit++ ){
	  fIngridHit[ihit] = 0;
	  if(ihit < nhit)
	    fIngridHit[ihit] = TRef( (IngridHitSummary*)basicrecon->GetIngridHit(ihit) );
	}

#ifdef DEBUG
	evtdisp     -> Clear();
	fingevtdisp -> Draw_Module    ( *evtdisp );
	fingevtdisp -> Draw_Hit_A     ( *basicrecon,0.08 );
	//fingevtdisp -> Draw_BeamInfo  ( *beaminfo );
#endif

	//## recon
	frecontrack -> Clear();
	
	frecontrack -> SetHit(fIngridHit, nhit);

	//if( frecontrack -> ReconTrack() ){
	if( frecontrack -> ReconTrack(1) ){
	  ntrk++;
	  //cout << "tracking successful" << endl;
	  debugtxt << ievt << "\t" << i << "\t" <<endl; 
	  testcount2++;
	  double ax, ay;
	  basicrecon -> hastrk   = true;
	  basicrecon -> matchtrk = frecontrack -> TrkMatching(); 

	  //### Fill X and Y Track Summary 
	  for(int v=0; v<2; v++){
	    IngridTrackSummary* trk = frecontrack -> GetTrack(v, 0);
	    trk -> view = v;
	    evt -> AddTrack(trk);
	    //cout<<"I have "<<basicrecon->Ntracks()<<" tracks."<<endl;
	    IngridTrackSummary* ttt = (IngridTrackSummary*)evt->GetTrack(evt->NIngridTracks()-1);
	    basicrecon -> AddTrack(ttt);
	    //cout<<basicrecon->Ntracks()<<endl;
	    //basicrecon -> AddTrack(trk);
	    //### VETO ??
	    if( frecontrack -> is1stTPL(v) || frecontrack->isUVETO(v) 
		 || frecontrack -> UVETOisTrack(v)){
	      basicrecon -> vetowtracking = true;

	      basicrecon -> vpe = frecontrack->Vpe(v);
	    }
	    if( frecontrack->UEdgeisTrack(v) ){
	      basicrecon -> edgewtracking = true;

	    }

	    if(v==FromX){
	      ay = ttt -> tx;
	      basicrecon -> thetay = TMath::ATan( ay )/TMath::Pi() * 180;
	      basicrecon -> vertexy.clear();	    
	      basicrecon -> vertexy.push_back( frecontrack->GetInitialXY(v) );
	      basicrecon -> vertexyz  = frecontrack->GetInitialZ(v);
	      basicrecon -> startypln = frecontrack->StartPln(v); 
	      basicrecon -> startych  = frecontrack->StartCh(v); 
	      basicrecon -> endypln   = frecontrack->EndPln(v); 
	      basicrecon -> endych    = frecontrack->EndCh(v); 
	      basicrecon -> nhity     = frecontrack->NHit(v);
	      basicrecon -> ntrackhity= frecontrack->NTrackhit(v);
	    } 
	    if(v==FromY){
	      ax = ttt -> tx;
	      basicrecon -> thetax = TMath::ATan( ax )/TMath::Pi() * 180;
	 
	      basicrecon -> vertexx.clear();	    
	      basicrecon -> vertexx.push_back( frecontrack->GetInitialXY(v) );
	      basicrecon -> vertexxz = frecontrack->GetInitialZ(v);
	      basicrecon -> startxpln = frecontrack->StartPln(v); 
	      basicrecon -> startxch  = frecontrack->StartCh(v); 
	      basicrecon -> endxpln   = frecontrack->EndPln(v); 
	      basicrecon -> endxch    = frecontrack->EndCh(v); 
	      basicrecon -> nhitx     = frecontrack->NHit(v);
	      basicrecon -> ntrackhitx= frecontrack->NTrackhit(v);
	    } 

#ifdef DEBUG

	    TF1* f = new TF1("f","pol1",0,130);
	    f   -> SetParameter(0, trk->etx);
	    f   -> SetParameter(1, trk->tx);
	    fingevtdisp -> Draw_Line(*f, v,trk->vtxi[2],trk->vtxf[2]);
#endif

	  } 


	  basicrecon -> angle = TMath::ATan( sqrt(ax*ax + ay*ay) ) /

	    TMath::Pi() * 180;
	  int endpln   = max( basicrecon -> endxpln, 
			      basicrecon -> endypln );
	  int startpln = min( basicrecon -> startxpln, 
			      basicrecon -> startypln );

	  if( !isVeto(endpln) && endpln <  10 &&
	      2 <= basicrecon -> endxch     && basicrecon -> endxch <= 21 &&
	      2 <= basicrecon -> endych     && basicrecon -> endych <= 21 ){
	    basicrecon -> modfc     = true;
	    basicrecon -> penIron   = endpln - startpln + 1;
	    int           penScinti = endpln - startpln + 1;
	    basicrecon -> muE     = 
	      1.0 * ( 1.0 * MIPdepE * basicrecon -> penIron * IronDensity * IronThick +  MIPdepE     * penScinti * 2 ) / TMath::Cos( basicrecon -> angle * TMath::Pi()/180 );
	    float         muE     = basicrecon -> muE;
	    basicrecon -> nuErec = 
	      //( Mp * Mp - ( Mn - NuclearPotE ) * ( Mn - NuclearPotE ) - muE * muE + 2 * ( Mn - NuclearPotE ) * muE ) /
	      //( 2 * ( Mn - NuclearPotE - muE + sqrt( muE * muE - Mmu * Mmu ) * TMath::Cos( basicrecon -> angle * TMath::Pi()/180 ) ) );
	      ( Mn * muE - 0.5 * Mmu * Mmu ) / ( Mn - muE + sqrt( muE*muE - Mmu*Mmu )*TMath::Cos(basicrecon->angle * TMath::Pi()/180) );

	      }
	  
	  else{
	    basicrecon -> modfc     = false;
	  }


	  counter[0]++;
	  counter[1]++;

	


#ifdef DEBUG
	//### debug test

	  //if(endpln!=9)
	  //continue;

	  if( !(basicrecon->endxch>1&&
		basicrecon->endxch<22&&
		basicrecon->endych>1&&
		basicrecon->endych<22) )
	    continue;
	  //if(!basicrecon -> modfc)
	  //continue;

	  if( !( basicrecon -> hastrk && basicrecon -> matchtrk &&
		 !basicrecon -> vetowtracking&&!basicrecon -> edgewtracking) )
	    continue;
	  IngridSimVertexSummary* simver = (IngridSimVertexSummary*)(evt -> GetSimVertex(0));
	  if(simver->inttype!=1)
	    continue;

	  //if(simver->nuE < 3)
	  //continue;
	  cout << "end x pln  :"  << basicrecon -> endxpln << endl;
	  cout << "end y pln  :"  << basicrecon -> endypln << endl;
	//cout << "difference of vertex z=" << diffverz << endl;
	  cout << "nhitx      :"  << basicrecon -> nhitx << endl;
	  cout << "nhity      :"  << basicrecon -> nhity << endl;
	  cout << "ntrackhitx :"  << basicrecon -> ntrackhitx << endl;
	  cout << "ntrackhity :"  << basicrecon -> ntrackhity << endl;

	  cout << "endpln   : " << endpln << endl;
	  cout << "startpln : " << startpln << endl;
	  cout << "penIron  : " << basicrecon -> penIron << endl;
	  cout << "muE      : "     << basicrecon -> muE     * 1e-3<< endl;
	  cout << "rec  Enu : "     << basicrecon -> nuErec  * 1e-3<< endl;
	  cout << "true Enu : "     << simver     -> nuE     << endl;
	  cout << "Mode     : "     << simver     -> inttype << endl;	  

	  /*
	cout << "event number = "  << ievt << endl;
	cout << "cycle number = "  << basicrecon->hitcyc << endl;
	cout << "module number = " << basicrecon->hitmod << endl;

	cout << "-- At Side View --" << endl;
	cout << "Start ( " << basicrecon -> startypln  
	     << ", "       << basicrecon -> startych  
	     << " )"       << endl; 
	cout << "End   ( " << basicrecon -> endypln  
	     << ", "       << basicrecon -> endych    
	     << " )"       << endl; 
	cout << "-- At Top View --" << endl;
	cout << "Start ( " << basicrecon -> startxpln  
	     << ", "       << basicrecon -> startxch  
	     << " )"       << endl; 
	cout << "End   ( " << basicrecon -> endxpln  
	     << ", "       << basicrecon -> endxch    
	     << " ) "      << endl; 
	  */
	evtdisp     -> Update();
	cin.get();
#endif
	}
      }//like
    }

    wsummary = evt;
    wtree -> Fill();
  }//Event Loop

#ifdef DEBUG2
  txtfile.close();
#endif

  //######## Write and Close ####################
  wtree -> Write();
  wfile -> Write();
  wfile -> Close();
#ifdef DEBUG2
  cout << "All:"     << counter[0]
       << "\tRecon:" << counter[1]
       << "\teff:"   << 1.0*counter[1]/counter[0]
       << endl;
#endif
  cout<<testcount2<<"\t"<<testcount<<endl;
  cout<<"# of tracks :" << ntrk << endl;
}
 
