#include<iostream>
#include<sstream>
#include<fstream>
using namespace std;
#include <iomanip.h>
#include <sys/stat.h>
#include <vector>

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
#include "analysis_beam.hxx"
#include "setup.hxx"
#include "INGRID_Dimension.hxx"
//#include "root_setup.hxx"
#include "INGRIDEVENTSUMMARY.h"
#include "IngridHitSummary.h"
#include "IngridSimHitSummary.h"
#include "IngridSimVertexSummary.h"
#include "IngridSimParticleSummary.h"
#include "BeamInfoSummary.h"
#include "IngridBasicReconSummary.h"
#include "Ingrid1stReducSummary.h"


//##### The Variable of INGRID Data  ############
//###############################################
Int_t           nSpill;
Long_t           UTime;
Int_t           NumEvt;
Int_t            Cycle;
Int_t             fMod;
vector<int>*       adc;
vector<double>*     pe;
vector<int>*      nsec;
vector<int>*      view;
vector<int>*       pln;
vector<int>*        ch;
vector<double>*  posxy;
vector<double>*   posz;
double   tracking_posz;
double       veto_posz; 

//##### The Variable of Beam Summary file #######
//###############################################

Int_t               trg_sec;
Int_t              spillnum;
Int_t         nd280spillnum;
Double_t          beam_time [5] [9];
Double_t              ct_np [5] [9];
Double_t              mumon [12];
Double_t                otr [13];

//#### INGRID Local Beam summary variable #######
//###############################################

Bool_t               wohorn;
Bool_t                whorn;
Bool_t                horn1;
Bool_t              cutwotr;
Bool_t             scutwotr;
Bool_t             horn250;


//#### The variable after analyzed ##############
//###############################################
Long_t           fTime;
Long_t          fcTime;     //#### without correction by proton beam timing measured by CT
Long_t      fcTimecorr;     //#### with    correction by proton beam timing measured by CT

const static int THRESHOLD_HIT = 2;
const static int maxhit        = 4;
vector<Hit> hitclster;
const static int TDCOffset  = 300;    //[nsec] there is a offset at start of TDC

int main(int argc,char *argv[]){

  pe   = 0;
  nsec = 0;
  view = 0;
  pln  = 0;
  ch   = 0;
  posxy= 0;
  posz = 0;


  char buff1[200],buff2[200];
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  Int_t run_number;
  Int_t c=-1;  char FileName[300];
  while ((c = getopt(argc, argv, "r:f:")) != -1) {
    switch(c){
    case 'r':
      run_number=atoi(optarg);
      sprintf(FileName,"/home/daq/data/dst/ingrid_%08d_0000_processed.root",run_number);
      break;

    case 'f':
      sprintf(FileName,"%s",optarg);
      run_number=77;
      break;

    }
  }
  FileStat_t fs;

  //######## read root file #############################
  //#####################################################
  cout<<"reading "<<FileName<<"....."<<endl;
  if(gSystem->GetPathInfo(FileName,fs)){
    cout<<"Cannot open file "<<buff1<<endl;
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

  //#### make rootfile after 1st reduction #####
  //############################################
  sprintf(buff1,"/home/daq/data/dst/ingrid_%08d_0000_1streduc.root",run_number);
  TFile*            wfile = new TFile(buff1,"recreate");
  TTree*            wtree = new TTree("tree","tree");
  IngridEventSummary* wsummary = new IngridEventSummary(); 
  wtree              -> Branch   ("fDefaultReco.","IngridEventSummary", 
				 &wsummary,  64000,   99);

  Bool_t       isActive[15];
  Long_t            fModif;


  Int_t totalSpill=0;

  BeamInfoSummary*               bsd;
  IngridHitSummary*        inghitsum;
  IngridHitSummary*      inghitsum_t; 
  Ingrid1stReducSummary*       reduc = new Ingrid1stReducSummary();
  Hit hit;

  for(int ievt=0; ievt<nevt; ievt++){

    if(ievt%100==0)cout<<ievt<<endl;

    wsummary -> Clear();
    evt      -> Clear();
    tree     -> GetEntry(ievt);
 
    for( int mod=0; mod<14; mod++ ){ //### Module Loop
      for( int cyc=0; cyc<23; cyc++ ){  //### Cycle Loop

	nSpill = evt -> nd280nspill;
	fMod  = mod;
	Cycle = cyc;
	//##### put the IngridModHit class to simple Hit class for analysis ######
	//########################################################################
	allhit.clear();
	int ninghit = evt -> NIngridModHits(mod, cyc);
	for(int i=0; i<ninghit; i++){
	  IngridHitSummary*        inghitsum;
	  inghitsum   = (IngridHitSummary*) (evt -> GetIngridModHit(i, mod, cyc) );
	  hit.clear();
	  hit.id    = i;
	  hit.pe    = inghitsum -> pe;
	  hit.time  = inghitsum -> time;
	  hit.view  = inghitsum -> view;
	  hit.pln   = inghitsum -> pln;
	  hit.ch    = inghitsum -> ch;
	  hit.posxy = inghitsum -> xy;
	  hit.posz  = inghitsum -> z;
	  inghitsum -> Clear();
	
	  allhit.push_back(hit);
	}
	fSortTime(allhit);


	hitclster.clear();
	int dimch;
	while(fFindTimeClster(allhit, hitclster, fTime, fcTime, dimch, maxhit)){	   
	  fSortTime(hitclster);
	  Int_t nCls = hitclster.size();
	  //###### make hit bits ##############
	  //###################################
	  Int_t       xhitbit=0;
	  Int_t       yhitbit=0;
	  float       xtotpe =0;
	  float       ytotpe =0;
	  vector<int>  hitx;
	  vector<int>  hity;
	  vector<int>  hitxz;
	  vector<int>  hityz;
	  for( Int_t i = 0 ; i < nCls ; i++){
	    int p = hitclster[i].pln;
	    int v = hitclster[i].view;
	    int c = hitclster[i].ch;
	    if( v==0 && p<11 ){ //## Y pln(from X(0) view)
	      yhitbit  =  yhitbit | ( (0x400)>>(10-p) );
	      ytotpe  +=  hitclster[i].pe;
	      hity. push_back( c  );
	      hityz.push_back( p );
	    }
	    if( v==1 && p<11 ){ //## Y pln(from X(0) view)
	      xhitbit  =  xhitbit | ( (0x400)>>(10-p) );
	      xtotpe  +=  hitclster[i].pe;
	      hitx. push_back( c );
	      hitxz.push_back( p );
	    }
	  }//i

	  //###### count number of hits #######
	  //###################################
	  Int_t   nhitxlyr = 0;
	  Int_t   nhitylyr = 0;
	  for( Int_t i = 1; i<2049; i=i<<1 ){
	    if( xhitbit&i )
	      nhitxlyr++;
	    if( yhitbit&i )
	      nhitylyr++;
	  }

	  //### check track like or not #######
	  //###################################
	  bool    xtracklike = false;
	  bool    ytracklike = false;
	  int     xcounter_in_a_row=0;
	  int     ycounter_in_a_row=0;
	  for( Int_t i = 1; i<2049; i=i<<1 ){
	    if( xhitbit&i ){
	      xcounter_in_a_row++;
	      if( xcounter_in_a_row >=3 )
		xtracklike = true;
	    }
	    else
	      xcounter_in_a_row = 0;

	    if( yhitbit&i ){
	      ycounter_in_a_row++;
	      if( ycounter_in_a_row >=3 )
		ytracklike = true;
	    }
	    else
	      ycounter_in_a_row = 0;
	  }

	  //cout<<xhitbit<<"\t"<<yhitbit<<endl;
	  //cout<<nhitxlyr<<"\t"<<nhitylyr<<endl;
	  //### Fill the 1streduc class                    ###
	  //### if there is hits larger than THRESHOLD_HIT ###
	  //##################################################
	  if( nhitxlyr >= THRESHOLD_HIT && nhitylyr >= THRESHOLD_HIT ){
	    reduc  -> Clear();
	    reduc  -> hitmod   = mod;
	    reduc  -> hitcyc   = cyc;
	    reduc  -> xhitbit  = xhitbit;
	    reduc  -> yhitbit  = yhitbit;
	    reduc  -> nhitxlyr = nhitxlyr;
	    reduc  -> nhitylyr = nhitylyr;
	    reduc  -> xtotpe   = xtotpe;
	    reduc  -> ytotpe   = ytotpe;
	    reduc  -> ytracklike   = ytracklike;
	    reduc  -> xtracklike   = xtracklike;


	    reduc  -> hitx     = hitx;
	    reduc  -> hity     = hity;
	    reduc  -> hitxz    = hitxz;
	    reduc  -> hityz    = hityz;
	  
	    for(Int_t i = 0; i < nCls; i++){
	      inghitsum_t 
		= (IngridHitSummary*)evt->GetIngridModHit( hitclster[i].id, mod, cyc );


	      reduc -> AddIngridHit( inghitsum_t );
	    }


	    evt    -> Add1stReduc( reduc );

	  }

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
 
