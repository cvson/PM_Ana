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

//#include "root_setup.hxx"
#include "INGRIDEVENTSUMMARY.h"
#include "IngridHitSummary.h"
#include "IngridSimHitSummary.h"
#include "IngridSimVertexSummary.h"
#include "IngridSimParticleSummary.h"
#include "BeamInfoSummary.h"
#include "IngridBasicReconSummary.h"
#include "IngridConstants.h"


int main(int argc,char *argv[]){
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);

  Int_t c=-1;  char FileName[300];
  char Output[300]; bool rename = false;
  int run_number;
  int sub_run_number=0;
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

  IngridHitSummary*         inghitsum;
  IngridBasicReconSummary*  ingbasrec;
  BeamInfoSummary*               info;
  IngBasicRecon*  fbrec   = new IngBasicRecon();
  IngEvtDisp*     evtdisp = new IngEvtDisp();
  //TCanvas*             c1 = new TCanvas("c1","c1",10,10,800,800);
  //evtdisp -> Draw_Module(*c1);
  TRef  fIngridHit[INGRIDHIT_MAXHITS];
  for( int ievt = 0; ievt < nevt; ievt++  ){
  //for( int ievt = 0; ievt < 1000; ievt++  ){
#ifdef BG
    if(ievt>5000)break;
#endif
 
    if(ievt%100==0)cout << "analyze event# " << ievt<<endl;
    evt      -> Clear();
    wsummary -> Clear();
    tree -> GetEntry(ievt);

    int nbrec = evt -> NIngridBasicRecons();
    for( int ibrec=0; ibrec < nbrec; ibrec++ ){

      ingbasrec   = (IngridBasicReconSummary*)( evt -> GetBasicRecon(ibrec) );
      int nhit    = ingbasrec -> nhits;
      for( int ihit=0; ihit<INGRIDHIT_MAXHITS; ihit++ ){
	fIngridHit[ihit] = 0;
	fIngridHit[ihit] = TRef( (IngridHitSummary*)ingbasrec->GetIngridHit(ihit) );
      }

      //### calculate # of active planes and       #####
      //### photo electrons / active layers        #####
      //################################################
      fbrec  -> Clear();
      fbrec  -> SetHit(fIngridHit);
      ingbasrec -> trgbit  = fbrec  -> fTrgbit();
      ingbasrec -> nactpln = fbrec -> fNactpln();
      ingbasrec -> layerpe = fbrec -> fLayerpe();
      ingbasrec -> upstreamVETO = fbrec -> fUVETOHit();
      fbrec     -> fGetVertex();
      ingbasrec -> vertexz = fbrec -> fVertexz();
      ingbasrec -> vertexx = fbrec -> fVertexx();
      ingbasrec -> vertexy = fbrec -> fVertexy();
      ingbasrec -> upstreamedge = fbrec -> fEdgeHit();
      ingbasrec -> actinarow    = fbrec -> fActInARow();
      ingbasrec -> vpe          = fbrec -> vpe;

      /*
      if( ingbasrec -> nactpln > 1   && 
	  ingbasrec -> layerpe > 6.5 && 
	  ingbasrec -> upstreamVETO )
	cout<<fbrec->vpe             <<"\t"
	    <<ingbasrec -> hitmod    <<"\t"
	    <<fbrec->vview    <<"\t"
	    <<fbrec->vpln    <<"\t"
	    <<fbrec->vch    <<"\t"
	    <<ingbasrec -> vertexz    <<"\t"
	    <<endl;
      */

      //### calculate time and diff. time from expected beam timing #####
      //#################################################################
      //int cyc = (int)( (ingbasrec -> clstime)/(fIntTime+fRstTime));
      int cyc = ingbasrec->hitcyc; //### Bug fix 2010/5/18

      info = (BeamInfoSummary*) ( evt -> GetBeamSummary(0) );
      float CTtime = CTtimeBase;
      ingbasrec -> bunch_flag = evt -> bunch_flag[cyc];
      if( info != 0){
	ingbasrec -> spill_flag = info -> spill_flag;
	/*
	if(  ( info -> hct[0][0] > 240 &&
	       info -> hct[0][0] < 260 &&
	       info -> hct[1][0] > 240 &&
	       info -> hct[1][0] < 260 &&
	       info -> hct[2][0] > 240 &&
	       info -> hct[2][0] < 260 
	       ) ){
	*/
	ingbasrec -> horn250 =true;    
	
	if( evt -> bunch_flag[cyc] ){
	  CTtime = info -> beam_time[0][cyc - bunch1st_cyc + 1] - GapbwBunch * 1e-9 * ( cyc - bunch1st_cyc );
	  ingbasrec -> CTtimecorr = info -> beam_time[0][cyc - bunch1st_cyc + 1] - 1.0 * GapbwBunch * 1e-9 * ( cyc - bunch1st_cyc );
	      
	}
	//}
      }
      ingbasrec -> clstimecorr = 
	ingbasrec -> clstime - ( CTtime - CTtimeBase )*1e9; 
      ingbasrec -> exptime = 
	((int)ingbasrec -> clstimecorr - TDCOffset )% GapbwBunch - fExpBeamTime;
      //if( fabs( ingbasrec->exptime ) < beamontime )
      if( fabs( ingbasrec->exptime ) < beamontime && ingbasrec -> bunch_flag ){//### 100426
	ingbasrec -> ontime = true;

      }

#ifdef DEBUG
     
      if( ingbasrec -> nactpln > 1 && ingbasrec -> layerpe > 6.5 ){
	  wtfile << info -> spillnum                         << "\t"	
		 << ievt                                     << "\t"
		 << (int)((ingbasrec  -> clstime -300)/580)  << "\t"
		 << ingbasrec -> hitmod                      << "\t"
		 << ingbasrec -> clstime                     << "\t";

	  wtfile << "1\t" ;
	if( fabs ( ingbasrec -> ontime ) < 100 ){
	  wtfile << "1\t" ;
	  if( !( ingbasrec -> upstreamVETO ) ){
	    wtfile << "1\t" ;
	    if( !( ingbasrec -> upstreamedge ) ){
	      wtfile << "1\t" << endl ;
	    }
	    else
	      wtfile << "0\t"<< endl ;
	  }
	  else
	    wtfile << "0\t0\t"<<endl ;
	}
	else
	  wtfile << "0\t0\t0\t"<<endl ;
      }


    

     
#endif

      //### rock muon study #####
      //#########################
      /*
      if( ingbasrec -> nactpln > cut_nactpln   &&
	  ingbasrec -> layerpe > cut_layerpe ){
	fbrec  -> GetMU_MD();
	fbrec  -> GetLine();
	evtdisp->Draw_Hit_A(*ingbasrec,1.1);
	evtdisp->fTFlineX.clear();
	evtdisp->fTFlineY.clear();
	TF1* line;
	for(int k=0; k<fbrec->nlx; k++){
	  line = fbrec -> LineX(k);
	  evtdisp->fTFlineX.push_back(*line);
	}
	for(int k=0; k<fbrec->nly; k++){
	  line = fbrec -> LineY(k);
	  evtdisp->fTFlineY.push_back(*line);
	}
	evtdisp->Draw_Line_all();
	c1 -> Update();
	cin.get();
      }
      */
    }
    wsummary = evt;
    wtree    ->Fill();
  }//Event Loop


  //######## Write and Close ####################
  wtree  -> Write();
  wfile  -> Write();
  wfile  -> Close();

}
 

