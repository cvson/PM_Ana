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
#include "PMreconRev.hxx"
#include "PMdisp.h"

#include "INGRIDEVENTSUMMARY.h"
#include "IngridHitSummary.h"
#include "IngridSimHitSummary.h"
#include "IngridSimVertexSummary.h"
#include "IngridSimParticleSummary.h"
#include "BeamInfoSummary.h"
#include "IngridBasicReconSummary.h"
#include "PMReconSummary.h"
#include "NeutInfoSummary.h"

//#define  STUDY_BADCHANNEL
vector<int>  bad_mod;
vector<int>  bad_pln;
vector<int>  bad_view;
vector<int>  bad_ch;
void Add_Bad_Channel();
bool Is_Bad_Channel(IngridHitSummary* thit);
void fMode(int mode);

const static char *dst_data          = "/home/t2kingrid/data/dst";
const static char *out_data          = "/home/t2kingrid/data/dst";
const static char *cosmic_data       = "/home/t2kingrid/data/cosmic";
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
  //Int_t Nmod = 14;
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
  FileStat_t fs;
  // ifstream timing;

  if(!renamef){
    sprintf(buff1,"%s/ingrid_%08d_%04d_form.root",
	    dst_data, run_number,sub_run_number);
  }
  if(cosmic){
    sprintf(buff1,"%s/ingrid_%08d_%04d_form.root",
	    cosmic_data, run_number,sub_run_number);
  }
  
  //######## read root file #############################
  //#####################################################
  cout<<"reading "<<buff1<<"....."<<endl;
  if(gSystem->GetPathInfo(buff1,fs)){
    cout<<"Cannot open file "<<buff1<<endl;
    exit(1);
  }

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
  if( !rename ){
    sprintf(Output, "%s/ingrid_%08d_%04d_nactpln.root", 
	    out_data, run_number, sub_run_number); 
  }
  if(cosmic){
    sprintf(Output, "%s/ingrid_%08d_%04d_nactpln.root", 
	    cosmic_data, run_number, sub_run_number); 
  }

  TFile*            wfile = new TFile(Output, "recreate");
  TTree*            wtree = new TTree("tree","tree");

  float norm,totcrsne,nuE,inttype;
  int nactpln,hitmod;

  wtree->Branch("nactpln",&nactpln,"nactpln/I");
  wtree->Branch("hitmod",&hitmod,"hitmod/I");
  wtree->Branch("norm",&norm,"norm/F");
  wtree->Branch("totcrsne",&totcrsne,"totcrsne/F");
  wtree->Branch("nuE",&nuE,"nuE/F");
  wtree->Branch("inttype",&inttype,"inttype/F");

  IngridHitSummary*    inghitsum;
  IngridSimVertexSummary*   simver;
  //IngridBasicReconSummary*    ingbasicrecon;
  //PMReconSummary*    ingbasicrecon;
  //IngridHitSummary*  inghitsum_t;
  //IngridBasicReconSummary* recon = new IngridBasicReconSummary();
  PMReconSummary* recon = new PMReconSummary();
  Hit                        hit;
  for(int ievt=Nini; ievt<nevt; ievt++){

    if(ievt%100==0)cout << "analyze event# " << ievt<<endl;
    evt      -> Clear();
    tree     -> GetEntry(ievt);

    if(evt->NIngridSimVertexes()>0){
      simver = (IngridSimVertexSummary*)(evt -> GetSimVertex(0) );
      norm     = simver -> norm;
      totcrsne = simver -> totcrsne;
      nuE = simver -> nuE;
      inttype = simver -> inttype;
    }
    else{
      norm=1;
      totcrsne=1;
      nuE = 1;
      inttype = 1;
    }

    for( int mod=0; mod<Nmod; mod++ ){   //### Module Loop
      for( int cyc=Scyc; cyc<Ncyc; cyc++ ){  //### Cycle Loop
	
        allhit.clear();
	int ninghit = evt -> NIngridModHits(mod, cyc);

	if(ninghit<6)continue;

	for(int i=0; i<ninghit; i++){
	  inghitsum   = (IngridHitSummary*) (evt -> GetIngridModHit(i, mod, cyc) );
	  if(inghitsum->cyc==-2)continue; //This is killed hit 
	  if(Is_Bad_Channel( inghitsum ))continue;
	  if(inghitsum -> pe < 2.5) continue;

	  inghitsum -> addbasicrecon = false;

	  hit.id    = i;
	  hit.pe    = inghitsum -> pe;
	  hit.lope  = inghitsum -> lope;
	  hit.time  = inghitsum -> time;
	  hit.view  = inghitsum -> view;
	  hit.pln   = inghitsum -> pln;
	  hit.ch    = inghitsum -> ch;
	  /*
	  if((inghitsum -> NSimHits()) > 0)
	    hit.pdg   = inghitsum -> GetIngridSimHit(0)->pdg;
	  else
	    hit.pdg   = 0;
	  */
	  hit.posxy = xyposi(mod, hit.view, hit.pln);
	  hit.posz  = zposi(mod, hit.pln, hit.ch);

	  allhit.push_back(hit);
	}

	if(allhit.size()<6)continue;
	fSortTime(allhit);
	while(fFindTimeClster(allhit, hitcls, fcTime)){

	  nactpln = fNactpln(mod);
	  hitmod=mod;
	  wtree->Fill();
	  
	  hitcls.clear(); //Reset hit clster
	}//Find Time Clster
      }//cyc
     }//mod

    //fAnalysis(evt,Scyc,Ncyc);

    //wtree -> Fill();
  }//Event Loop

  //######## Write and Close ####################
  wtree  -> Write();
  wfile  -> Write();
  wfile  -> Close();

}

const int bad_num = 20;
int       bad_id[bad_num][4] =
  {// mod, view,  pln,  ch, 
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

