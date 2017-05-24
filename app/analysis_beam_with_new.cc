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

Double_t         fccTime;   //#### 
Double_t         fbTime;    //####
Int_t          nActPln;
Float_t        fMeanPe;
Float_t        fMeanPe_Old;  //#### Old definition of Mean ( before 091229 )
Int_t          nActTPL;      //#### TPL  means usual TPL 1 ~ 9
Int_t          nActVeto;     //#### Veto means usual TPL 0, 10 and VETO
int             MUpAct;
int             MUpActX;
int             MUpActY;
int          nhitclster;
bool           UVETOHit;
bool            edgehit;     //#### 100205 
                             //#### it is found that edge ch. at upstream TPL has a lot of hit 
bool          MUTPLHit;
double          CTtime;
int              dimch;
bool            ontime;
int         vertexz;
vector<int> vertexx;
vector<int> vertexy;

bool           vinternal;	 
const static int   DefOnTime=100;

int                  n;
int            counter;

int inttype; //### interaction mode(MC)
vector<Hit> hitclster;
const static int TDCOffset  = 300;    //[nsec] there is a offset at start of TDC
const static float CTtimeBase = 1.08e-6;//base time of CT 
const static  int   GapbwBunch = 581;    //Gap b/w proton bunches [nsec]
const static  int bunch1st_cyc = 4;
void Print(){
  //if( cutwotr && whorn && fabs(fccTime)>50 &&fMeanPe>6.5 && nActPln>=2 ){
  if( fMeanPe>6.5 && nActPln>=2 && !UVETOHit && MUpAct==1){
    bool flag = false;
    for( int i=0; i<vertexx.size(); i++ ){
      if( vertexx[i]==0 || vertexx[i]==23 )flag=true;
    }
    for( int i=0; i<vertexy.size(); i++ ){
      if( vertexy[i]==0 || vertexy[i]==23 )flag=true;
    }
    cout << "entry:  " << n << "\tMod:"<< fMod << "\tCyc:" << Cycle
	 << "\t" << flag << endl;



    //cout << "Spill: " << nSpill <<endl;
    /*
    cout<<"entry  :"     << n      
        <<"\tMod :"      << fMod
        <<"\tCyclc :"    << Cycle
        <<"\tmode :"     << inttype
	<<"\tMean :"     << fMeanPe     
        <<"\tAct :"      << nActPln
        <<"\tspill :"    << nSpill        
	<< endl;
    */
    /*
    cout<<"-----------------"<<endl;
    cout<<"entry  :"<< n      << "\t";
    cout<<"nSpill :"<< nSpill << "\t";
    cout<<"fcTime :"<< fcTime << "\t";
    cout<<"Cycle  :"<< Cycle  << "\t";
    cout<<"MUp  :"  << MUpAct << "\t";
    cout<<"Mod  :"  << fMod   << "\t";
    if(ontime)cout  <<"ontime"<< "\t";
    if(!UVETOHit)
      cout<<" maru"<<endl;
    else if(UVETOHit)
      cout<<" batsu"<<endl;
    counter++;
    */
    //}
  }
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
  while ((c = getopt(argc, argv, "r:f:b:")) != -1) {
    switch(c){
    case 'r':
      run_number=atoi(optarg);
      sprintf(buff1,"/home/daq/data/dst/ingrid_%08d_0000_processed.root",run_number);
      break;
    case 'b':
      run_number=atoi(optarg);
      sprintf(buff1,"/home/daq/data/dst/ingrid_%08d_0000_calib.root",run_number);
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
  sprintf(buff1,"/home/daq/data/dst/ingrid_%08d_0000_recon.root",run_number);
  TFile*            wfile = new TFile(buff1,"recreate");
  TTree*            wtree = new TTree("tree","tree");
  IngridEventSummary* wsummary = new IngridEventSummary(); 
  wtree              -> Branch   ("fDefaultReco.","IngridEventSummary", 
				 &wsummary,  64000,   99);




  Bool_t       isActive[15];
  Long_t            fModif;



  Int_t totalSpill=0;

  BeamInfoSummary*        bsd;
  IngridHitSummary* inghitsum;
  Hit hit;
  IngridHitSummary* inghitsum_t; 
  IngridBasicReconSummary* recon = new IngridBasicReconSummary();
  BeamInfoSummary* info;
  for(int ievt=0; ievt<nevt; ievt++){
  //for(int ievt=0; ievt<200; ievt++){
    n = ievt;
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
	  allhit.push_back(hit);
	}
	fSortTime(allhit);


	hitclster.clear();
	while(fFindTimeClster(allhit, hitclster, fTime, fcTime, dimch,3)){	   

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
	    isActive[i]=false;
	  }
	  for( Int_t i = 0 ; i < nCls ; i++){
	    for( Int_t j=i+1 ; j<nCls ; j++){
	      if( hitclster[i].pln == hitclster[j].pln ){
		//### updated 100210
		if(hitclster[i].view != hitclster[j].view){
		  trgbit  =  trgbit | ((0x400)>>(10-hitclster[i].pln));
		  isActive[hitclster[i].pln] = true;
		  //if( MUpAct > hitclster[i].pln && hitclster[i].pln>=1 )
		  if( MUpAct > hitclster[i].pln )
		    MUpAct = hitclster[i].pln;

		}//view difference
	      }//pln coince
	    }//j
	  }//i

	  for(Int_t i=1;i;i=i<<1){
	    if(trgbit&i){
	      if(i!=1)nActPln++;
	    }
	  }

	  //######### check vertex distribution #################
	  //#####################################################
	  vertexx.clear();
	  vertexy.clear();
	  for(Int_t i = 0; i < nCls; i++){
	    int p = hitclster[i].pln;
	    int v = hitclster[i].view;
	    if( p == MUpAct ){
	      int ch = hitclster[i].ch;
	      if(v==0)vertexx.push_back(ch);
	      if(v==1)vertexy.push_back(ch);
	    
	    }
	  }



	  //######### check active Veto plane ####################
	  //######################################################
	  Int_t   trgvetobit  = 0;
	  for( Int_t i = 0 ; i < nCls ; i++){
	    if( hitclster[i].pln > 10 ) {
	      trgvetobit = trgvetobit | ((0x8)>>(hitclster[i].pln-11));
	      isActive[hitclster[i].pln]=true;
	    }
	  }//i




	  //######### calculate mean p.e. ###############
	  //#############################################
	  fMeanPe          = 0;
	  Int_t      nEnt  = 0;
	  for(Int_t i = 0 ; i < nCls ; i++){
	    if( trgbit & (  (0x400)>>(10 - hitclster[i].pln) ) ){
	      if( hitclster[i].pln != 0 )fMeanPe     += hitclster[i].pe;
	      //fMeanPe     += hitclster[i].pe;
	      fMeanPe_Old += hitclster[i].pe;
	      nEnt++;
	    }
	  }
	  nhitclster = nEnt;
	  if(nEnt>0&&nActPln>=1)fMeanPe     = 1.0 * fMeanPe / ( nActPln * 2 );   //#### Updated at 091229
	  else if(nEnt==0)fMeanPe=0;





	  //####### Upstream VETO  #############################
	  //####################################################
	  /*
	  UVETOHit = false;
	  MUTPLHit = false;
	  edgehit  = false;
	  fSortPosz(hitclster);
	  tracking_posz = -777;
	  veto_posz     =  120;
	  for(Int_t i = 0 ; i < nCls ; i++){
	    int check_pln  = hitclster[i].pln;
	    int check_view = hitclster[i].view;
	    if( check_pln >= 1 && check_pln <= 10 ){      // Tracking Plane ( #1 ~ #10 )
	      if(isActive[ check_pln ]){ //Active Plane
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
	      isActive [0] ){
	    UVETOHit = true;
	  }
	  if(isActive[0]){ 
	    veto_posz = 0;
	    MUTPLHit  = true;
	  }
*/
	  //######## updated 100209 ########
	  fSortPosz(hitclster);
	  //### check the z position of TPL 0x, 0y, VETOs ######
	  bool   flag_hitveto = false;
	  float  veto_posz;
	  for( int i=0; i < hitclster.size(); i++ ){
	    int p = hitclster[i].pln;
	    if( p == 0 || p == 11 || p == 12 || p == 13 || p == 14 ){
	      veto_posz     = hitclster[i].posz;
	      flag_hitveto  = true;
	      break;
	    }
	  }
	  //### check the z position of active TPLs       ######
	  bool   flag_hittpl = false;
	  float  tpl_posz;
	  for( int i=0; i < hitclster.size(); i++ ){
	    int p = hitclster[i].pln;
	    if( !( p == 0 || p == 11 || p == 12 || p == 13 || p == 14 ) &&
		isActive[p] 
		){
	      tpl_posz    = hitclster[i].posz;
	      flag_hittpl = true;
	      break;
	    }
	  }
	  if( flag_hitveto && flag_hittpl &&
	      veto_posz    <  tpl_posz
	      ){
	    UVETOHit = true;
	  }
	  else
	    UVETOHit = false;

	  //####### Check the upstream hit is edge or not ######
	  //####################################################
	  fSortPosz(hitclster);
	  //### check the z position of TPL 0x, 0y, VETOs ######
	  edgehit = false;
	  for( int i=0; i < hitclster.size(); i++ ){
	    int p = hitclster[i].pln;
	    if( p == MUpAct ){//## current TPL is upstream TPL
	      int c = hitclster[i].ch;
	      if( c == 0 || c == 23 ){//## edge channel
		edgehit = true;
	      }//edge
	    }//upstream TPL
	  }//hit channel loop





	  //####### Check Interaction Mode(MC data) ############
	  //####################################################
	  inttype = 0;
	  IngridSimVertexSummary* simvertex 
	    = (IngridSimVertexSummary*)evt->GetSimVertex(0);//basicaly only 1
	  if( simvertex ){
	    inttype = simvertex -> inttype;
	  }






	  //##### Fill #######
	  bool horn250 = false;
	  int trg_sec=0;;
	  if ( evt -> bunch_flag[cyc] ){
	    info = (BeamInfoSummary*) ( evt -> GetBeamSummary(0) );
	    trg_sec = info -> trg_sec;
	    if( info -> ct_np[0][ cyc - bunch1st_cyc + 1 ] > 1e11 ) {
	      CTtime = info -> beam_time[0][cyc - bunch1st_cyc + 1] - GapbwBunch * 1e-9 * ( cyc - bunch1st_cyc );

	    }
	    else 
	      //CTtime = info -> beam_time[0][0];
	      CTtime = CTtimeBase ;
	    /*
	    fccTime = ( fcTime - TDCOffset    ) % GapbwBunch - 200 
	      - ( CTtime - CTtimeBase ) * 1e9;
	    fcTime  = fcTime - ( CTtime - CTtimeBase ) * 1e9;
	    */

	    fcTimecorr = fcTime - ( CTtime - CTtimeBase ) * 1e9; 
	                                //## CTtime and CTtimeBase is [sec] and 
	                                //## fcTime and fcTimecorr is [nsec]
	    fccTime    = ( fcTimecorr - TDCOffset    ) % GapbwBunch - 200 ;

	    horn250    = ( info -> hct[0][0] < 240 &&
			   info -> hct[0][0] > 260 &&
			   info -> hct[1][0] < 240 &&
			   info -> hct[1][0] > 260 &&
			   info -> hct[2][0] < 240 &&
			   info -> hct[2][0] > 260 
			   );
	  }
	  else{
	    fcTimecorr = fcTime;
	    //fccTime    = ( fcTime - TDCOffset    ) % GapbwBunch - 200 ;
	    fccTime    = ( fcTimecorr - TDCOffset    ) % GapbwBunch - 200 ;
	  }

	  if( fabs( fccTime ) < DefOnTime )
	    ontime = true;
	  else 
	    ontime = false;

	  horn250 = false;
	  if( info -> hct[0][0] > 240 &&
	      info -> hct[0][0] < 260 &&
	      info -> hct[1][0] > 240 &&
	      info -> hct[1][0] < 260 &&
	      info -> hct[2][0] > 240 &&
	      info -> hct[2][0] < 260 
	      )
	    horn250 = true;
	  //info  -> horn250 = horn250;

	  recon  -> Clear();
	  recon  -> clstime          = fcTime;
	  recon  -> clstimecorr      = fcTimecorr;
	  recon  -> exptime          = fccTime;
	  recon  -> nactpln          = nActPln;
	  recon  -> layerpe          = fMeanPe;
	  recon  -> upstreamVETO     = UVETOHit;
	  recon  -> upstreamedge     = edgehit;
	  //recon  -> upstreamVETO     = (UVETOHit||edgehit);
	  recon  -> hitmod           = mod;
	  recon  -> bunch_flag       = evt -> bunch_flag[cyc];
	  recon  -> upstreamtpl      = MUpAct;
	  recon  -> vertexz          = MUpAct;
	  recon  -> vertexx          = vertexx;
	  recon  -> vertexy          = vertexy;
	  recon  -> ontime           = ontime;
	  recon  -> nhitclster       = nhitclster;
	  recon  -> horn250          = horn250;
	  recon  -> trg_sec          = trg_sec;

	  vinternal = true;
	  if(MUpAct == 1)vinternal = false;
	  for(int i=0; i<vertexx.size(); i++){
	    if(vertexx[i]==0 || vertexx[i]==23)
	      vinternal = false;
	  }
	  for(int i=0; i<vertexy.size(); i++){
	    if(vertexy[i]==0 || vertexy[i]==23)
	      vinternal = false;
	  }
	  recon  -> vinternal         = vinternal;

	  for(Int_t i = 0; i < nCls; i++){
	    inghitsum_t 
	      = (IngridHitSummary*)evt->GetIngridModHit( hitclster[i].id, mod, cyc );
	    recon -> AddIngridHit( inghitsum_t );
	  }
	  
	  evt -> AddBasicRecon( recon );
	  Print();



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
 
