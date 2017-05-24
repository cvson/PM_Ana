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
#include "PMrecon_pedist.hxx"
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

const static char *dst_data          = "/home/kikawa/scraid1/data/pm_ingrid";
const static char *out_data          = "/home/kikawa/scbn03/pm_ingrid";
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
  //Int_t Nmod = 17;
  Int_t Nmod = 14;
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

  //sprintf(buff1,"%s/ingrid_%08d_%04d_processed.root",
  //  dst_data, run_number,sub_run_number);
  if(!renamef){
    //sprintf(buff1,"%s/ingrid_%08d_%04d_done.root",
    //dst_data, run_number,sub_run_number);
    sprintf(buff1,"%s/ingrid_%08d_%04d.root",
	    out_data, run_number,sub_run_number);
  }
  if(cosmic){
    //sprintf(buff1,"%s/ingrid_%08d_%04d_done.root",
    //cosmic_data, run_number,sub_run_number);
    sprintf(buff1,"%s/ingrid_%08d_%04d.root",
	    out_data, run_number,sub_run_number);

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
  //sprintf(buff1,"/home/daq/data/dst/ingrid_%08d_0000_tclster.root",run_number);
  if( !rename ){
    sprintf(Output, "%s/ingrid_%08d_%04d_recon.root", 
	    //dst_data, run_number, sub_run_number); 
	    out_data, run_number, sub_run_number); 
  }
  if(cosmic){
    sprintf(Output, "%s/ingrid_%08d_%04d_recon.root", 
	    cosmic_data, run_number, sub_run_number); 
  }

  TFile*            wfile = new TFile(Output, "recreate");
  TTree*            wtree = new TTree("tree","tree");


  float hit_pe,hit_lope,hit_ang;
  float norm,totcrsne;
  int hit_pln,hit_mod,hit_view,hit_ch;
  wtree->Branch("hit_pe",&hit_pe,"hit_pe/F");
  wtree->Branch("hit_lope",&hit_lope,"hit_lope/F");
  wtree->Branch("hit_ang",&hit_ang,"hit_ang/F");
  wtree->Branch("hit_mod",&hit_mod,"hit_mod/I");
  wtree->Branch("hit_pln",&hit_pln,"hit_pln/I");
  wtree->Branch("hit_view",&hit_view,"hit_view/I");
  wtree->Branch("hit_ch",&hit_ch,"hit_ch/I");
  wtree->Branch("norm",&norm,"norm/F");
  wtree->Branch("totcrsne",&totcrsne,"totcrsne/F");


  IngridHitSummary*    inghitsum;
  //IngridBasicReconSummary*    ingbasicrecon;
  //PMReconSummary*    ingbasicrecon;
  //IngridHitSummary*  inghitsum_t;
  //IngridBasicReconSummary* recon = new IngridBasicReconSummary();
  PMReconSummary* recon = new PMReconSummary();
  IngridSimVertexSummary*  simver;
  Hit                        hit;
  for(int ievt=Nini; ievt<nevt; ievt++){

    if(ievt%100==0)cout << "analyze event# " << ievt<<endl;
    //wsummary -> Clear();
    evt      -> Clear();
    tree     -> GetEntry(ievt);

    simver  = (IngridSimVertexSummary*)( evt -> GetSimVertex(0) );
    norm=simver->norm;
    totcrsne=simver->totcrsne;


    for( int mod=16; mod<17; mod++ ){   //### Module Loop
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
	  hit.pdg   = 0;
	  hit.posxy = xyposi(mod, hit.view, hit.pln);
	  hit.posz  = zposi(mod, hit.pln, hit.ch);

	  allhit.push_back(hit);
	}

	if(allhit.size()<6)continue;
	fSortTime(allhit);
	while(fFindTimeClster(allhit, hitcls, fcTime)){
	  

	  recon->nactpln = fNactpln(mod);
	  if(recon->nactpln < 3)continue;
	  recon->layerpe = fLayerpe(mod);
	  //if(recon->layerpe < 6.5)continue;
	  
	  if(!fTracking(mod))continue;
	  //fTracking(mod);
	  
	  if(alltrack.size()!=2)continue;
	  if(alltrack[0].view==alltrack[1].view)continue;

	  if(!(alltrack[0].ipln==alltrack[1].ipln)&&
	     !((alltrack[0].ipln==alltrack[1].ipln+1)&&alltrack[0].view==0)&&
	     !((alltrack[1].ipln==alltrack[0].ipln+1)&&alltrack[1].view==0)
	     )continue;


	  if(!(alltrack[0].fpln==alltrack[1].fpln)&&
	     !((alltrack[0].fpln==alltrack[1].fpln+1)&&alltrack[0].view==0)&&
	     !((alltrack[1].fpln==alltrack[0].fpln+1)&&alltrack[1].view==0)
	     )continue;

	  if(!alltrack[0].veto&&!alltrack[1].veto)continue;

	  float corr=1/sqrt(1+alltrack[1].slope*alltrack[1].slope+alltrack[0].slope*alltrack[0].slope);

	  hit_ang=acos(corr)*180/3.141592;

	  for(int i=0;i<alltrack.size();i++){
	    /*
	    recon -> view         = alltrack[i].view;
	    recon -> startpln     = alltrack[i].ipln;
	    recon -> endpln       = alltrack[i].fpln;
	    recon -> startxy      = alltrack[i].ixy;
	    recon -> endxy        = alltrack[i].fxy;
	    recon -> startz       = alltrack[i].iz;
	    recon -> endz         = alltrack[i].fz;
	    recon -> angle        = alltrack[i].ang;
	    recon -> slope        = alltrack[i].slope;
	    recon -> intcpt       = alltrack[i].intcpt;
	    recon -> vetowtracking = alltrack[i].veto;
	    recon -> edgewtracking = alltrack[i].edge;
	    recon -> modfc        = alltrack[i].stop;
	    recon -> vetodist     = alltrack[i].vetodist;
	    recon -> totpe        = alltrack[i].totpe;
	    recon -> isohit       = alltrack[i].isohit;
	    for(TMP=0;TMP<4;TMP++)
	      recon -> pdg[TMP]       = alltrack[i].pdg[TMP];
	    recon -> clstime      = fcTime;
	    recon -> hitmod       = mod;
	    recon -> hitcyc       = cyc;
	    //evt   -> AddPMRecon( recon );
	    evt   -> AddPMModRecon( recon , mod, cyc, alltrack[i].view);
	    */
	    hit_mod=mod;
	    hit_view=alltrack[i].view;
	    for(TMP=0;TMP<alltrack[i].hit_n;TMP++){
	      hit_pln=alltrack[i].hit_pln[TMP];
	      hit_ch=alltrack[i].hit_ch[TMP];
	      hit_pe=alltrack[i].hit_pe[TMP]*corr;
	      hit_lope=alltrack[i].hit_lope[TMP]*corr;
	      wtree->Fill();
	    }


	  }

	  
	  //if(disp&&mod==16){
	  //if(disp&&mod!=16){
	  
	  hitcls.clear(); //Reset hit clster
	}//Find Time Clster
      }//cyc
     }//mod

    //fAnalysis(evt,Scyc,Ncyc);


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

