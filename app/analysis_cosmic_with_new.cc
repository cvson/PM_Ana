#include<iostream>
#include<sstream>
#include<fstream>
using namespace std;
#include <iomanip.h>
#include <sys/stat.h>
#include <map>

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

//#### INGRID Local Beam summary variable #######
//###############################################
Bool_t           bunch_flag;
Bool_t               wohorn;
Bool_t                whorn;
Bool_t                horn1;
Bool_t              cutwotr;
Bool_t             scutwotr;


//#### The variable after analyzed ##############
//###############################################
vector<double>*  anape;
Long_t           fTime;
Long_t          fcTime;      //#### with weightened by # of pe
Double_t         fccTime;    //#### 
Double_t         fcbTime;    //#### without correction by proton beam timing measured by CT
Int_t          nActPln;
Float_t        fMeanPe;
Float_t        fMeanPe_Old;  //#### Old definition of Mean ( before 091229 )
Int_t          nActTPL;      //#### TPL  means usual TPL 1 ~ 9
Int_t          nActVeto;     //#### Veto means usual TPL 0, 10 and VETO
int             MUpAct;
bool          UVETOHit;
bool          MUTPLHit;
double          CTtime;
int              dimch;
bool            ontime;

const static int   DefOnTime=100;

int                  n;
Int_t       Nhitclster;
int            counter;



class ActivePlane{  //### Also can be used for layer
public:
  bool   active;
  int      view;    //### when used for layer
  float      pe;
  int        ch;
  int       pln;
};


vector<Hit> hitclster;
void Print{
}

//##### class anabeam ########################
//##### for Pringin all beam event ###########
//##### with sorting in time  ################
//############################################
class anabeam{
public:
  int nevent; 
  long anamodif;
  int anaspill;
  int anatime;
};
bool withftime(const anabeam& left, const anabeam& right){
  return left.anatime < right.anatime;
};
bool fSortfTime(vector<anabeam> &a){
  std::stable_sort(a.begin(), a.end(), withftime);
};


map<double, vector<ActivePlane> >      actpln[11]; //first double means threshold
map<double, vector<ActivePlane> >      actlyr[2][11];


int main(int argc,char *argv[]){
  pe   = 0;
  nsec = 0;
  view = 0;
  pln  = 0;
  ch   = 0;
  posxy= 0;
  posz = 0;
  fINGRID_Dimension = new INGRID_Dimension();
  counter           = 0;
  vector<anabeam> fanabeam;
  fanabeam.clear();

  char buff1[200],buff2[200];
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  Int_t run_number;
  Int_t c=-1;  char FileName[300];
  while ((c = getopt(argc, argv, "r:f:")) != -1) {
    switch(c){
    case 'r':
      run_number=atoi(optarg);
      sprintf(buff1,"/home/daq/data/dst/ingrid_%08d_0000_processed.root",run_number);
      break;
    case 'f':
      sprintf(buff1,"%s",optarg);
      run_number=77;
      break;

    }
  }
  FileStat_t fs;
  ifstream timing;




  //######## read root file #############################
  //#####################################################
  cout<<"reading "<<buff1<<"....."<<endl;
  if(gSystem->GetPathInfo(buff1,fs)){
    cout<<"Cannot open file "<<buff1<<endl;
    exit(1);
  }
  IngridEventSummary* evt = new IngridEventSummary();
  TFile*            rfile = new TFile(buff1,"read");
  TTree*             tree = (TTree*)rfile -> Get("tree");
  TBranch*          EvtBr = tree->GetBranch("fDefaultReco.");
  EvtBr                   ->SetAddress(&evt);
  tree                    ->SetBranchAddress("fDefaultReco.", &evt);
  int                nevt = (int)tree -> GetEntries();
  cout << "Total # of events = " << nevt <<endl;

  //#### make rootfile after analysis #####
  sprintf(buff1,"/home/daq/data/dst/ingrid_%08d_0000_cosmic.root",run_number);
  TFile*            wfile = new TFile(buff1,"recreate");
  TTree*            wtree = new TTree("tree","tree");
  IngridEventSummary* wsummary = new IngridEventSummary(); 
  wtree              -> Branch   ("fDefaultReco.","IngridEventSummary", 
				 &wsummary,  64000,   99);



  Bool_t       bActPln[15];
  Long_t            fModif;



  Int_t totalSpill=0;
  Int_t tnclster=0;
  for(int ievt=0; ievt<nevt; ievt++){
    n = ievt;
    if(ievt%100==0)cout<<ievt<<endl;
    wsummary -> Clear();
    evt      -> Clear();
    tree -> GetEntry(ievt);
    nSpill = evt -> nd280nspill;
    for( int mod=0; mod<16; mod++ ){ //### Module Loop
      for( int cyc=0; cyc<23; cyc++ ){  //### Cycle Loop
	if( evt -> bunch_flag[ cyc ] ){ //### There is beam bunch
	  fMod  = mod;
	  Cycle = cyc;
	  IngridHitSummary* inghitsum;


	  //##### put the IngridModHit class to simple Hit class for analysis ######
	  //########################################################################
	  allhit.clear();
	  int ninghit = evt -> NIngridModHits(mod, cyc);
	  for(int i=0; i<ninghit; i++){
	    inghitsum   = (IngridHitSummary*) (evt -> GetIngridModHit(i, mod, cyc) );
	    Hit hit  ;
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
	  tnclster = 0;
	  hitclster.clear();
	  while(fFindTimeClster(allhit, hitclster, fTime, fcTime, dimch)){	   
	    fSortTime(hitclster);

	    Int_t nCls = hitclster.size();
	    //######### check active tracking plane ################
	    //######################################################
	    Int_t   trgbit   = 0;
	    nActPln          = 0;
	    nActTPL          = 0;
	    nActVeto         = 0;
	    MUpAct           =10;
	    for(int i=0;i<15;i++){
	      bActPln[i]=false;
	    }
	    for( Int_t i = 0 ; i < nCls ; i++){
	      for( Int_t j=i+1 ; j<nCls ; j++){
		if( hitclster[i].pln == hitclster[j].pln){
		  if(hitclster[i].view != hitclster[j].view){
		    trgbit  =  trgbit | ((0x400)>>(10-hitclster[i].pln));
		    bActPln[hitclster[i].pln] = true;
		    if( MUpAct > hitclster[i].pln ) MUpAct = hitclster[i].pln;
		  }//view difference
		}//pln coince
	      }//j
	    }//i
	    for(Int_t i=1;i;i=i<<1){
	      if(trgbit&i){
		nActPln++;
		if( i==1 || i== 1024) 
		  nActVeto++;
		else 
		  nActTPL++;
	      }
	    }

	    //######### check active tracking plane ################
	    //######################################################
	    Int_t   trgbit   = 0;
	    nActPln          = 0;
	    nActTPL          = 0;
	    nActVeto         = 0;
	    MUpAct           =10;
	    for(int i=0;i<15;i++){
	      bActPln[i]=false;
	    }
	    actpln.clear();
	    actlyr.clear();
	    for( Int_t i = 0 ; i < nCls ; i++){
	      for( Int_t j=i+1 ; j<nCls ; j++){
		if( hitclster[i].pln == hitclster[j].pln){
		  if(hitclster[i].view != hitclster[j].view){
		    for(int thr = 3; thr<10; thr++){
		      double threshold  = 0.5 + thr;
		      if( hitclster[i].pe > threshold &&
			  hitclster[j].pe > threshold ){
		       

		      if( MUpAct > hitclster[i].pln ) MUpAct = hitclster[i].pln;
		    }//view difference
		  }//pln coince
		}//j
	      }//i
	    for(Int_t i=1;i;i=i<<1){
	      if(trgbit&i){
		nActPln++;
		if( i==1 || i== 1024) 
		  nActVeto++;
		else 
		  nActTPL++;
	      }
	    }

	    //######### check active Veto plane ####################
	    //######################################################
	    Int_t   trgvetobit  = 0;
	    for( Int_t i = 0 ; i < nCls ; i++){
	      if( hitclster[i].pln > 10 ) {
		trgvetobit = trgvetobit | ((0x8)>>(hitclster[i].pln-11));
		bActPln[hitclster[i].pln]=true;
	      }
	    }//i
	    for(Int_t i=1;i;i=i<<1){
	      if(trgvetobit&i){
		nActVeto++;
	      }
	    }



	    //######### calculate mean p.e. ###############
	    //#############################################
	    fMeanPe          = 0;
	    Int_t      nEnt  = 0;
	    for(Int_t i = 0 ; i < nCls ; i++){
	      if( trgbit & (  (0x400)>>(10 - hitclster[i].pln) ) ){
		fMeanPe     += hitclster[i].pe;
		fMeanPe_Old += hitclster[i].pe;
		nEnt++;
	      }
	    }

	    if(nEnt>0)fMeanPe     = 1.0 * fMeanPe / ( nActPln * 2 );   //#### Updated at 091229
	    else if(nEnt==0)fMeanPe=0;

	    //####### Upstream VETO  #############################
	    //####################################################
	    UVETOHit = false;
	    MUTPLHit = false;
	    fSortPosz(hitclster);
	    tracking_posz = -777;
	    veto_posz     =  120;
	    for(Int_t i = 0 ; i < nCls ; i++){
	      int check_pln  = hitclster[i].pln;
	      int check_view = hitclster[i].view;
	      if( check_pln >= 1 && check_pln <= 10 ){      // Tracking Plane ( #1 ~ #10 )
		if(bActPln[ check_pln ]){ //Active Plane
		  if( tracking_posz == -777 ){
		    tracking_posz = hitclster[i].posz;
		  }
		}
	      }
	      else if( check_pln >= 11 ){ // Upstream VETO ( TPL#0 and VETO )
		if( veto_posz == 120 ){
		  if( hitclster[i].pe > 3.5 ){ // Apply relatively high threshold
		    veto_posz = hitclster[i].posz;
		  }
		}
	      }
	    }

	    if( ( veto_posz != -777 && ( veto_posz < tracking_posz ) ) ||
		bActPln [0] ){
	      UVETOHit = true;
	    }
	    if(bActPln[0]){ 
	      veto_posz = 0;
	      MUTPLHit  = true;
	    }

	    //####### Check Interaction Mode #####################
	    //####################################################
	
	


	    //##### Fill #######

	    IngridBasicReconSummary* recon = new IngridBasicReconSummary();
	    recon  -> clstime          = fcTime;
	    recon  -> clstimecorr      = fccTime;
	    recon  -> nactpln          = nActPln;
	    recon  -> layerpe          = fMeanPe;
	    recon  -> upstreamVETO     = UVETOHit;
	    recon  -> hitmod           = mod;
	    for(Int_t i = 0; i < nCls; i++){
	      IngridHitSummary* inghitsum_t 
		= (IngridHitSummary*)evt->GetIngridModHit( hitclster[i].id, mod, cyc );
	      recon -> AddIngridHit( inghitsum_t );
	    }
	    evt -> AddBasicRecon( recon );


	    //if(fMeanPe>6.5&&nActPln>=2&&nSpill==50935){
	    /*
	    if(nSpill==50935){
	      cout<<"-----------"<< evt->nd280nspill <<endl;
	      for(Int_t i = 0; i < nCls; i++){
		BeamInfoSummary* info = (BeamInfoSummary*)evt->GetBeamSummary(0);
		cout 	<< "mod:"  << fMod   << "\t"
			<< "view:" << hitclster[i].view  << "\t"
			<< "pln:"  << hitclster[i].pln   << "\t"
			<< "ch:"   << hitclster[i].ch    << "\t"
			<< "pe:"   << hitclster[i].pe    << "\t"
			<< "time:" << hitclster[i].time  << endl;
	      }
	    }
	    */
	    Print();
	    
	    
	    hitclster.clear(); //Reset hit clster
	  }//Find Time Clster
	}//good bunch
      }//cyc
    }//mod
    wsummary = evt;


    IngridBasicReconSummary* test = (IngridBasicReconSummary*)wsummary->GetBasicRecon(0);
    if(test){
      //cout<<test->nactpln<<"\t"<<test->layerpe<<endl;
    }

    wtree -> Fill();
  }//Event Loop


  //######## Write and Close ####################
  wtree  -> Write();
  wfile  -> Write();
  wfile  -> Close();

}
 
