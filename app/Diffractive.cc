#define USEINGTRK
#define USEINGPID
#define USEBACKTRK
#define USEPARTRK
#define USESHORTTRK

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
#include "PMrecon.hxx"
#include "PMAnaRevRev.hxx"

#include "INGRIDEVENTSUMMARY.h"
#include "IngridHitSummary.h"
#include "IngridSimHitSummary.h"
#include "IngridSimVertexSummary.h"
#include "IngridSimParticleSummary.h"
#include "BeamInfoSummary.h"
#include "NeutInfoSummary.h"
#include "IngridBasicReconSummary.h"
#include "PMReconSummary.h"
#include "PMAnaSummary.h"

const static int   GapbwBunch     = 581 ;    //[nsec]
const static int   TDCOffset      = 300;     //[nsec]
const static int   fExpBeamTime   = 200;     //[nsec]
const static int   beamontime     = 100;     //ontime

//#define  STUDY_BADCHANNEL
vector<int>  bad_mod;
vector<int>  bad_pln;
vector<int>  bad_view;
vector<int>  bad_ch;
void Add_Bad_Channel();
bool Is_Bad_Channel(IngridHitSummary* thit);
void GetNonRecHits(IngridEventSummary* evt, int cyc);//newly added

const static char *dst_data          = "/home/kikawa/scraid1/data/pm_ingrid";
const static char *cosmic_data       = "/home/kikawa/scraid1/data/pm_ingrid";
//static const char *data_file_folder  = "/home/kikawa/scraid1/data/pm_ingrid";
//static const char *calib_file_folder = "/home/kikawa/scraid1/data/pm_ingrid";
//static const char *dailycheck_folder = "/home/kikawa/scraid1/data/pm_ingrid";



//#include "IngridConstants.h"
long fcTime;
int main(int argc,char *argv[]){

#ifdef STUDY_BADCHANNEL
  cout << "It is BAD channel study mode. Is it O.K.?" << endl;
  //cin.get();
  Add_Bad_Channel();
#endif

  char buff1[200];//,buff2[200];
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  Int_t run_number;
  Int_t sub_run_number=0;
  Int_t c=-1;
  char Output[300];
  bool rename = false;
  bool renamef= false;
  Int_t Scyc =  4;
  Int_t Ncyc = 12;
  Int_t Nmod = 17;
  Int_t Nini = 0;
  bool disp = false; 
  bool cosmic = false;
  while ((c = getopt(argc, argv, "r:s:f:cado:i:")) != -1) {
    switch(c){
    case 'r':
      run_number=atoi(optarg);
      break;
    case 'o':
      sprintf(Output,"%s",optarg);
      rename  = true;
      break;
    case 's':
      sub_run_number=atoi(optarg);
      break;
    case 'f':
      sprintf(buff1,"%s",optarg);
      run_number=0;
      renamef = true;
      break;
    case 'a':
      Scyc = 0;
      Ncyc = 23;
      break;
    case 'c':
      Scyc = 14;
      Ncyc = 16;
      cosmic = true;
      break;
    case 'd':
      disp = true;
      break;
    case 'i':
      Nini=atoi(optarg);
      break;

    }
  }


  ifstream pefile("/home/cvson/cc1picoh/frkikawa/PM_Ana/app/pe.txt");
  for(int imod=0;imod<17;imod++){
    for(int iview=0;iview<2;iview++){
      for(int ipln=0;ipln<18;ipln++){
        for(int ich=0;ich<32;ich++){
          pefile>>perat[imod][iview][ipln][ich];
        }
      }
    }
  }


  FileStat_t fs;
  // ifstream timing;

  //sprintf(buff1,"%s/ingrid_%08d_%04d_processed.root",
  //  dst_data, run_number,sub_run_number);
  if(!renamef){
    sprintf(buff1,"%s/ingrid_%08d_%04d_recon.root",
	    dst_data, run_number,sub_run_number);
  }
  if(cosmic){
    sprintf(buff1,"%s/ingrid_%08d_%04d_recon.root",
	    cosmic_data, run_number,sub_run_number);
  }
  
  //######## read root file #############################
  //#####################################################
  cout<<"reading "<<buff1<<"....."<<endl;
  if(gSystem->GetPathInfo(buff1,fs)){
    cout<<"Cannot open file "<<buff1<<endl;
    exit(1);
  }
  cout << buff1 << endl;
  IngridEventSummary* evt = new IngridEventSummary();
  //TFile*            rfile = new TFile(Form("/home/daq/data/dst/ingrid_%08d_%04d_processed.root", 
  TFile*            rfile = new TFile(buff1,
				      "read");
  TTree*             tree = (TTree*)rfile -> Get("tree");
  TBranch*          EvtBr = tree->GetBranch("fDefaultReco.");
  EvtBr                   ->SetAddress(&evt);
  tree                    ->SetBranchAddress("fDefaultReco.", &evt);
  int                nevt = (int)tree -> GetEntries();
  cout << "Total # of events = " << nevt <<endl;

  //#### make rootfile after analysis #####
  //#######################################
  //sprintf(buff1,"/home/daq/data/dst/ingrid_%08d_0000_tclster.root",run_number);
  if( !rename ){
    sprintf(Output, "%s/ingrid_%08d_%04d_anas.root", 
	    dst_data, run_number, sub_run_number); 
  }
  if(cosmic){
    sprintf(Output, "%s/ingrid_%08d_%04d_anas.root", 
	    cosmic_data, run_number, sub_run_number); 
  }

  TFile*            wfile = new TFile(Output, "recreate");
  TTree*            wtree = new TTree("tree","tree");
  wtree -> SetMaxTreeSize(5000000000);
  IngridEventSummary* wsummary = new IngridEventSummary(); 
  wtree              -> Branch   ("fDefaultReco.","IngridEventSummary", 
				  &wsummary,  64000,  99);
  IngridHitSummary*    inghitsum;
  PMReconSummary* recon;
  //PMReconSummary* recon = new PMReconSummary();
  PMAnaSummary* pmanasum = new PMAnaSummary();
  //Hit                        hit;
  TrackPM                      track;
  TrackIng                     ingtrack;

  Hits                        hit;

  for(int ievt=Nini; ievt<nevt; ievt++){

    if(ievt%100==0)cout << "analyze event# " << ievt<<endl;
    wsummary -> Clear();
    evt      -> Clear();
    tree     -> GetEntry(ievt);

    IngridSimVertexSummary* simver =  (IngridSimVertexSummary*)( evt -> GetSimVertex(0) );

    int CYC=4;
    float vact=0;
    int ninghit = evt -> NIngridModHits(16, CYC);
    for(int ihit=0; ihit<ninghit; ihit++){
      inghitsum  = (IngridHitSummary*) (evt -> GetIngridModHit(ihit,16,CYC));
      if(Is_Bad_Channel( inghitsum ))continue;
      vact += veract(inghitsum,7,7,xyposi(16,6,28),xyposi(16,6,16));
    }    
    simver->totcrsne = vact;
    wsummary -> AddSimVertex(simver);
    //wsummary = evt;
    wtree -> Fill();
  }//Event Loop
  
  //######## Write and Close ####################
  wtree  -> Write();
  wfile  -> Write();
  wfile  -> Close();
  
}

const int bad_num = 20;
int       bad_id[bad_num][4] =
  {// mod, view,  pln,  hh, 
    {    0,    1,    8,    2}, //cable damage channel 
    {    0,    1,    8,   14}, //cable damage channel
    {    0,    0,   13,   12}, //cable damage channel
    {    1,    1,    1,   14}, //cable damage channel
    {    5,    1,    2,    0}, //cable damage channel
    {    5,    1,    4,    2}, //high gain channel  
    {    5,    1,    4,   18}, //cable damage channel
    {    5,    1,    4,   20}, //cable damage channel
    {    5,    1,    7,    9}, //cable damage channel
    {    7,    0,    5,    0}, //pedestal channel    
    {    7,    0,    6,    9}, //pedestal channel   
    {    9,    1,    0,   12}, //cable damage chnnale
    {    9,    0,    4,   11}, //cable damage chnnale
    {   11,    0,    5,   13}, //pedestal shift channel
    {   15,    1,    8,   14}, //added 2010/11/1 cable damaged
    {   16,    1,    9,   21}, //added 2010/12/4 ??                
    {   16,    0,   14,    0}, //added 2010/12/4 ??               
    {   14,    1,   10,    4}, //added 2010/12/4 ??               
    {   14,    0,   10,   15}, //added 2010/12/4 ??               
    {   15,    0,   10,   15}  //added 2010/12/4 ??               
    //{    3,    1,    1,   11}  //added 2010/12/4 ??               
  };

void Add_Bad_Channel(){
  bad_mod. clear();
  bad_view.clear();
  bad_pln. clear();
  bad_ch.  clear();
  for(int ibad=0; ibad < bad_num; ibad++){
    bad_mod.  push_back ( bad_id[ibad][0] );
    bad_view. push_back ( bad_id[ibad][1] );
    bad_pln.  push_back ( bad_id[ibad][2] );
    bad_ch.   push_back ( bad_id[ibad][3] );
  }
}

bool Is_Bad_Channel(IngridHitSummary* thit){
  for(int ibad=0; ibad < bad_num; ibad++){
    if( thit -> mod  == bad_id[ibad][0]  &&
	thit -> view == bad_id[ibad][1]  &&
	thit -> pln  == bad_id[ibad][2]  &&
	thit -> ch   == bad_id[ibad][3]
	)
      return true;
  }
  return false;
}


//newly added
void GetNonRecHits(IngridEventSummary* evt, int cyc){
  IngridHitSummary*    inghitsum;
  int imod,view,pln,ch;
  float pe;
  int ninghit;
  memset(nonrechits,0,sizeof(nonrechits));
  memset(ingnonrechits,0,sizeof(ingnonrechits));
  ninghit = evt -> NIngridModHits(16, cyc);
  for(int ihit=0; ihit<ninghit; ihit++){
    inghitsum  = (IngridHitSummary*) (evt -> GetIngridModHit(ihit,16,cyc));
    if(Is_Bad_Channel( inghitsum ))continue;
    view = inghitsum->view;
    ch   = inghitsum->ch;
    pln  = inghitsum->pln;
    pe   = PE(inghitsum->pe,inghitsum->lope,16,view,pln,ch);
    if(view<2&&pln<18&&ch<32)
      nonrechits[view][pln][ch]=pe;
  }

  for(int mod=0; mod<7; mod++){
    ninghit = evt -> NIngridModHits(mod, cyc);
    for(int ihit=0; ihit<ninghit; ihit++){
      inghitsum  = (IngridHitSummary*) (evt -> GetIngridModHit(ihit,mod,cyc));
      if(Is_Bad_Channel( inghitsum ))continue;
      view = inghitsum->view;
      ch   = inghitsum->ch;
      pln  = inghitsum->pln;
      pe   = PE(inghitsum->pe,inghitsum->lope,mod,view,pln,ch);
      if(mod<7&&view<2&&pln<2&&ch<24)
	ingnonrechits[mod][view][pln][ch]=pe;
    }
  }

  int hn,vn;
  hn=htrack.size();
  vn=vtrack.size();

  for(int i=0;i<hn;i++){
    int nhit=htrack[i].hit.size();
    for(int j=0;j<nhit;j++){
      view=htrack[i].hit[j].view;
      pln=htrack[i].hit[j].pln;
      ch=htrack[i].hit[j].ch;
      if(view<2&&pln<18&&ch<32)
	nonrechits[view][pln][ch]=0;
    }
  }

  for(int i=0;i<vn;i++){
    int nhit=vtrack[i].hit.size();
    for(int j=0;j<nhit;j++){
      view=vtrack[i].hit[j].view;
      pln=vtrack[i].hit[j].pln;
      ch=vtrack[i].hit[j].ch;
      if(view<2&&pln<18&&ch<32)
	nonrechits[view][pln][ch]=0;
    }
  }


  hn=hingtrack.size();
  vn=vingtrack.size();
  for(int i=0;i<hn;i++){
    int nhit=hingtrack[i].hit.size();
    for(int j=0;j<nhit;j++){
      imod=hingtrack[i].hit[j].mod;
      view=hingtrack[i].hit[j].view;
      pln=hingtrack[i].hit[j].pln;
      ch=hingtrack[i].hit[j].ch;
      if(imod<7&&view<2&&pln<2&&ch<24)
	ingnonrechits[imod][view][pln][ch]=0;
    }
  }

  for(int i=0;i<vn;i++){
    int nhit=vingtrack[i].hit.size();
    for(int j=0;j<nhit;j++){
      imod=vingtrack[i].hit[j].mod;
      view=vingtrack[i].hit[j].view;
      pln=vingtrack[i].hit[j].pln;
      ch=vingtrack[i].hit[j].ch;
      if(imod<7&&view<2&&pln<2&&ch<24)
	ingnonrechits[imod][view][pln][ch]=0;
    }
  }


};
