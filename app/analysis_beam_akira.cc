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
const static int gateW = 1500;
const static int resetW = 100;
const static int expcyc = 11;  // cycle # start from 0
const static int bunchW = 581;

int                  n;
Int_t       Nhitclster;
int            counter;

vector<Hit> hitclster;
void Print(){
  //if( cutwotr && whorn && fabs(fccTime)>50 &&fMeanPe>6.5 && nActPln>=2 ){
  if( fMeanPe>6.5 && nActPln>=2 ){
      
    cout<<"entry  :"     << n      
        <<"\tMod :"     << fMod
        <<"\tCyclc :"     << Cycle
	<<"\tMean :"     << fMeanPe     
        <<"\tAct :"      << nActPln
        <<"\tVETO :"     << UVETOHit
        <<"\tspill :"    << nSpill        
	<<"\tfccTime :"  << fccTime

	<< endl;
    
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
  bool sflag = false;
  bool bflag = false; //before processing
  bool tflag = false; //before processing
  while ((c = getopt(argc, argv, "sr:hbt")) != -1) {
    switch(c){
    case 's':
      sflag = true;
      break;
    case 'r':
      run_number=atoi(optarg);
      break;
    case 't':
      tflag = true;
      break;
    case 'b':
      bflag = true;
      break;
    
    case 'h':
      cout<<"help menu"<<endl;
      cout<<"-r [run_number]"<<endl;
      break;
    }
  }
  FileStat_t fs;
  ifstream timing;




  //######## read root file #############################
  //#####################################################
  sprintf(buff1,"/home/daq/data/root_new/ingrid_processed_%08d_0000.root",run_number);
  if(sflag){
    sprintf(buff1,"/home/daq/data/root_new/0912beam.root.onlybeam");
  }
  if(bflag){
    sprintf(buff1,"/home/daq/data/root_new/ingrid_%08d_0000.root",run_number);
  }
  cout<<"reading "<<buff1<<"....."<<endl;
  if(gSystem->GetPathInfo(buff1,fs)){
    cout<<"Cannot open file "<<buff1<<endl;
    exit(1);
  }
  TFile*               f  = new TFile(buff1,"read");
  TTree*            tree  = (TTree*)f->Get("tree");
  Int_t        Evt        = tree->GetEntries();
  cout<<"---- # of Evt    = "<<Evt<<"---------"<<endl;
  tree -> SetBranchAddress ("adc"     ,&adc           );
  tree -> SetBranchAddress ("pe"      ,&pe            );
  tree -> SetBranchAddress ("nsec"    ,&nsec          );
  tree -> SetBranchAddress ("view"    ,&view          );
  tree -> SetBranchAddress ("pln"     ,&pln           );
  tree -> SetBranchAddress ("ch"      ,&ch            );
  tree -> SetBranchAddress ("posxy"   ,&posxy         );
  tree -> SetBranchAddress ("posz"    ,&posz          );
  tree -> SetBranchAddress ("nSpill"  ,&nSpill        );
  tree -> SetBranchAddress ("UTime"   ,&UTime         );
  tree -> SetBranchAddress ("NumEvt"  ,&NumEvt        );
  tree -> SetBranchAddress ("Cycle"   ,&Cycle         );
  tree -> SetBranchAddress ("fMod"    ,&fMod          );
  tree -> SetBranchAddress ("spill_flag"      , &spill_flag      );
  tree -> SetBranchAddress ("good_spill_flag" , &good_spill_flag );
  tree -> SetBranchAddress ("spillnum"        , &spillnum        );
  tree -> SetBranchAddress ("nd280spillnum"   , &nd280spillnum   );
  tree -> SetBranchAddress ("trg_sec"         , &trg_sec         );
  tree -> SetBranchAddress ("beam_time"       ,  beam_time       );
  tree -> SetBranchAddress ("ct_np"           ,  ct_np           );
  tree -> SetBranchAddress ("mumon"           ,  mumon           );
  tree -> SetBranchAddress ("hct"             ,  hct             );
  tree -> SetBranchAddress ("otr"             ,  otr             );
  tree -> SetBranchAddress ("bunch_flag"      , &bunch_flag      );
  tree -> SetBranchAddress ("wohorn"          , &wohorn          );
  tree -> SetBranchAddress ("whorn"           , &whorn           );
  tree -> SetBranchAddress ("horn1"           , &horn1           );
  tree -> SetBranchAddress ("cutwotr"         , &cutwotr         );
  tree -> SetBranchAddress ("scutwotr"        , &scutwotr        );
 


  sprintf(buff1,"/home/daq/data/backgroudstudy/ingrid_%08d_akira.root",run_number);
  if(sflag){
    sprintf(buff1,"/home/daq/data/root_new/0912beamana.root",run_number);
  }

  if(bflag){
    sprintf(buff1,"/home/daq/data/backgroudstudy/ingrid_%08d_0001.root",run_number);
  }
  TFile*              rf  = new TFile(buff1,"recreate");
  TTree*                ModTree;
  Int_t            nlActPln[15];
  Float_t          flMeanPe[15];
  Bool_t            bActPln[15];
  Long_t                 fModif;
  Int_t                   Entry;
  ModTree =  new TTree("ModTree","ModTree");
  ModTree -> Branch("Cycle"       ,&Cycle   ,"Cycle/I"            );
  ModTree -> Branch("NumEvt"      ,&NumEvt  ,"NumEvt/I"           );
  ModTree -> Branch("nSpill"      ,&nSpill  ,"nSpill/I"           );
  ModTree -> Branch("UTime"       ,&UTime   ,"UTime/L"            );
  ModTree -> Branch("fTime"       ,&fTime   ,"fTime/L"            );
  ModTree -> Branch("fModif"      ,&fModif  ,"fModif/L"           );
  ModTree -> Branch("fcTime"      ,&fcTime  ,"fcTime/L"           );
  ModTree -> Branch("fccTime"      ,&fccTime  ,"fccTime/D"        );
  ModTree -> Branch("fcbTime"      ,&fcbTime  ,"fcbTime/D"        );
  ModTree -> Branch("fMod"        ,&fMod    ,"fMod/I"             );
  ModTree -> Branch("nActPln"     ,&nActPln ,"nActPln/I"          );
  ModTree -> Branch("nActTPL"     ,&nActTPL ,"nActTPL/I"          );
  ModTree -> Branch("nActVeto"    ,&nActVeto,"nActVeto/I"         );
  ModTree -> Branch("nlActPln[15]", nlActPln,"nlActPln[15]/I"     );
  ModTree -> Branch("bActPln[15]" , bActPln, "bActPln[15]/B"      );
  ModTree -> Branch("fMeanPe"     ,&fMeanPe ,"fMeanPe/F"          );
  ModTree -> Branch("fMeanPe_Old" ,&fMeanPe_Old ,"fMeanPe_Old/F"  );
  ModTree -> Branch("flMeanPe[15]", flMeanPe,"flMeanPe[15]/F"     );
  ModTree -> Branch("UVETOHit"    ,&UVETOHit,"UVETOHit/B"         );
  ModTree -> Branch("MUTPLHit"    ,&MUTPLHit,"MUTPLHit/B"         );
  ModTree -> Branch("CTtime"      ,&CTtime,"CTtime/D"             );
  ModTree -> Branch("dimch"       ,&dimch,"dimch/I"               );
  ModTree -> Branch("MUpAct"      ,&MUpAct,  "MUpAct/I"           );
  ModTree -> Branch("beam_time"   , beam_time,"beam_time[5][9]/D" );
  ModTree -> Branch("ct_np"       , ct_np    ,"ct_np[5][9]/D"     );
  ModTree -> Branch("otr"         , otr      ,"otr[13]/D"         );
  ModTree -> Branch("hct"         , hct      ,"hct[3][5]/D"       );
  ModTree -> Branch("Entry"       ,&Entry     ,"Entry/I"          );
  ModTree -> Branch("ontime"      ,&ontime    ,"ontime/B"         );
  ModTree -> Branch("wohorn"      ,&wohorn    ,"wohorn/B"         );
  ModTree -> Branch("whorn"       ,&whorn     ,"whorn/B"          );
  ModTree -> Branch("horn1"       ,&horn1     ,"horn1/B"         );
  ModTree -> Branch("cutwotr"     ,&cutwotr   ,"cutwotr/B"        );
  ModTree -> Branch("scutwotr"    ,&scutwotr  ,"scutwotr/B"       );
  ModTree -> Branch("veto_posz"   ,&veto_posz ,"veto_posz/D"        );
  ModTree -> Branch("tracking_posz"    ,&tracking_posz  ,"tracking_posz/D"       );

  //########## make Reset time histogram #####################
  //##########################################################
  TH1F*      fHreset = new TH1F("reset","reset",368,300,37100);
  for(Int_t i=0; i<23; i++){
    fHreset -> Fill( 300 + 1600 * (i+1) +50, 10000);
  }
  fHreset   -> SetLineColor(4);
  fHreset   -> Write();
  //########## make Beam timing histogram ####################
  //##########################################################
  TH1F*      fHbeam  = new TH1F("beam","beam",90,16943-80*10,16943+80*80);
  //April 16940
  for(Int_t i=0; i<6; i++){
    fHbeam -> Fill( 16900 + 580*i , 10000);
  }
  fHbeam   -> SetLineColor(2);
  fHbeam   -> Write();




  Int_t totalSpill=0;
  Int_t tnclster=0;
  for(n=0; n<Evt; n++){
    tree->GetEntry(n);
    //if(n%10000==0)cout<<"-- analysis Clster # = "<<n<<" ---"<<endl;
    if( n==10000*23*14 && tflag )break;
    if(bunch_flag||bflag){

    nActPln     = 0;
    fMeanPe     = 0;
    fMeanPe_Old = 0;
    for(int i=0;i<15;i++){
      nlActPln[i]=0;
      flMeanPe[i]=0;
    }
    Entry = n;
    Int_t nHit = pe->size();

    //######### fill hit class ####################
    allhit.clear();
    for(Int_t i=0;i<nHit;i++){
      nActPln     = 0;
      fMeanPe     = 0;
      fMeanPe_Old = 0;
      Hit hit  ;
      hit.pe   = pe  ->at(i);
      hit.time = nsec->at(i);
      hit.view = view->at(i);
      hit.pln  = pln ->at(i);
      hit.ch   = ch  ->at(i);
      
      hit.posxy= posxy->at(i);
      hit.posz = posz  ->at(i);
      allhit.push_back(hit);
    }
    fSortTime(allhit);
    //cout<<"--event:"<<Evt<<" Cycle:"<<Cycle<<" Mod:"<<fMod<<endl;


    hitclster.clear();
  
    tnclster = 0;
    Int_t actpln[100];
    while(fFindTimeClster(allhit, hitclster, fTime, fcTime, dimch)){
      if(fTime>1000000000)  fTime  = fTime  - 1000000000;
      if(fcTime>1000000000) fcTime = fcTime - 1000000000;
      fSortTime(hitclster);
      Int_t nCls = hitclster.size();

      //######### check active tracking plane ################
      //######################################################
      Int_t   trgbit  = 0;
      nActPln = 0;
      nActTPL=0;
      nActVeto=0;
      MUpAct=10;
      for(int i=0;i<15;i++){
	bActPln[i]=false;
      }
      for( Int_t i = 0 ; i < nCls ; i++){
	for( Int_t j=i+1 ; j<nCls ; j++){
	  if( hitclster[i].pln == hitclster[j].pln){
	    if(hitclster[i].view != hitclster[j].view){
	      trgbit=trgbit|((0x400)>>(10-hitclster[i].pln));
	      bActPln[hitclster[i].pln]=true;
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
      fMeanPe_Old      = 0;
      Int_t      nEnt  = 0;
      for(Int_t i = 0 ; i < nCls ; i++){
	//if( trgbit && (  (0x400)>>(10 - hitclster[i].pln) ) ){
	if( trgbit & (  (0x400)>>(10 - hitclster[i].pln) ) ){
	  fMeanPe     += hitclster[i].pe;
	  fMeanPe_Old += hitclster[i].pe;
	  nEnt++;
	}
      }
      if(nEnt>0)fMeanPe_Old = 1.0 * fMeanPe_Old /nEnt;           //#### Before Updated at 091229
      else if(nEnt==0)fMeanPe_Old=0;
      if(nEnt>0)fMeanPe     = 1.0 * fMeanPe / ( nActPln * 2 );   //#### Updated at 091229
      else if(nEnt==0)fMeanPe=0;




      //########## calculate number of activities  #######
      //########## with several threshold          #######
      //########## also calculate mean p.e.        #######
      //##################################################
      for(Int_t thre=2; thre<15;thre++){
	nlActPln[thre]=0;
	double threshold = 0.5 + thre;
	trgbit=0;
	for(Int_t i=0;i<nCls;i++){
	  for(Int_t j=i+1;j<nCls;j++){
	    if(hitclster[i].pln==hitclster[j].pln){
	      if(hitclster[i].view!=hitclster[j].view){
		if(hitclster[i].pe>threshold&&hitclster[j].pe>threshold){
		  trgbit=trgbit|((0x400)>>(10-hitclster[i].pln));
		}
	      }//view difference
	    }//pln coince
	  }//j
	}//i
	for(Int_t i=1;i;i=i<<1){
	  if(trgbit&i){
	    nlActPln[thre]++;
	  }
	}
	flMeanPe[thre]          = 0;
	nEnt  = 0;
	for(Int_t i=0;i<nCls;i++){
	  if(trgbit&& (  (0x400)>>(10 - hitclster[i].pln) ) ){
	    flMeanPe[thre] += hitclster[i].pe;
	    nEnt++;
	  }
	}
	if(nEnt>0)flMeanPe[thre] = flMeanPe[thre]/nEnt;
	else if(nEnt==0)flMeanPe[thre] = 0;
      }//thre


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


      //##### Fill #######
      if( ct_np[0][ Cycle-4+1 ] > 1e11 ) {
	CTtime = beam_time[0][Cycle-4+1] - 581e-9 * (Cycle-4);
      }
      else 
	CTtime = beam_time[0][0];

      fccTime = ( fcTime - 300    )%581 - 200 
	- ( CTtime - 1.08e-6)*1000000000;
      fcbTime = ( fcTime - 300    )%581 - 200; 
     
      if( fabs(    ( fcTime - 300    )%581 - 200 
		 - ( CTtime - 1.08e-6)*1000000000
		   ) < DefOnTime )
	ontime = true;
      else 
	ontime = false;

      if(bflag){
	fccTime = ( fcTime - 300    )%581 - 200 ;
	 
	if(Cycle==4 || Cycle==5 || Cycle==6 || Cycle==7 || Cycle==8 || Cycle==9 )
	  ontime = true;
	else 
	  ontime = false;

      }
      fcTime  = fcTime - ( CTtime - 1.08e-6)*1000000000;
      ModTree -> Fill();
      //Print();

      /*
      if(fMeanPe>6.5&&nActPln>=2){
	cout<<"-----------"<< nSpill <<endl;
	for(Int_t i = 0; i < nCls; i++){
	  cout << hitclster[i].view  << "\t"
	       << hitclster[i].pln   << "\t"
	       << hitclster[i].ch    << "\t"
	       << hitclster[i].pe    << "\t"
	       << hitclster[i].time  << endl;
	}
      }
      */

      hitclster.clear(); //Reset hit clster
      actpln[tnclster] = nActPln;
      tnclster++;
    }//Find Time Clster
    }}//Event Loop


  //######## Write and Close ####################
  ModTree -> Write();
  rf      -> Write();
  rf      -> Close();
  f       -> Close();
  cout<<counter<<endl;
}
 
