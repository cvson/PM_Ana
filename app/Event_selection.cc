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

#include "INGRIDEVENTSUMMARY.h"
#include "IngridHitSummary.h"
#include "IngridSimHitSummary.h"
#include "IngridSimVertexSummary.h"
#include "IngridSimParticleSummary.h"
#include "BeamInfoSummary.h"
#include "IngridBasicReconSummary.h"
#include "PMReconSummary.h"
#include "NeutInfoSummary.h"


const static char *dst_data          = "/home/kikawa/scraid1/data/pm_ingrid";
const static char *cosmic_data       = "/home/kikawa/scraid1/data/pm_ingrid";
//static const char *data_file_folder  = "/home/kikawa/scraid1/data/pm_ingrid";
//static const char *calib_file_folder = "/home/kikawa/scraid1/data/pm_ingrid";
//static const char *dailycheck_folder = "/home/kikawa/scraid1/data/pm_ingrid";



//#include "IngridConstants.h"
long fcTime;
int main(int argc,char *argv[]){

  double Nall=0,Nactpln=0, Nlayerpe=0, Ntrack=0, Nmatch=0, Nveto=0, Nedge=0;
  float weight,norm,totcrsne;
  int hitcyc,hitmod;

  char buff1[200];//,buff2[200];
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  Int_t run_number;
  Int_t sub_run_number=0;
  Int_t c=-1;
  char Output[300];
  bool rename = false;
  bool renamef= false;
  bool fileout= false;
  Int_t Scyc =  4;
  Int_t Ncyc = 12;
  //Int_t Nmod = 17;
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
      fileout = true;
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
    sprintf(Output, "%s/ingrid_%08d_%04d.txt", 
	    dst_data, run_number, sub_run_number); 
  }
  if(cosmic){
    sprintf(Output, "%s/ingrid_%08d_%04d.txt", 
	    cosmic_data, run_number, sub_run_number); 
  }

  

  IngridBasicReconSummary* recon;
  IngridSimVertexSummary*  simver;

  for(int ievt=Nini; ievt<nevt; ievt++){

    if(ievt%10000==0)cout << "analyze event# " << ievt<<endl;
    evt      -> Clear();
    tree     -> GetEntry(ievt);


    int ningtrack = evt -> NIngridBasicRecons();
    simver  = (IngridSimVertexSummary*)( evt -> GetSimVertex(0) );
    norm=simver->norm;
    totcrsne=simver->totcrsne;
    weight=norm*totcrsne;
    Nall+=weight;
    for(int i=0; i<ningtrack; i++){
      recon   = (IngridBasicReconSummary*) (evt -> GetBasicRecon(i) );
      hitcyc=recon->hitcyc;
      hitmod=recon->hitmod;
      if(hitmod<14&&hitcyc<23){
	//if(recon->nactpln<3)continue;
	Nactpln+=weight;
	//if(recon->layerpe<6.5)continue;
	Nlayerpe+=weight;
	if(!recon->hastrk)continue;
	Ntrack+=weight;
	if(!recon->matchtrk)continue;
	Nmatch+=weight;
	if(recon->vetowtracking)continue;
	Nveto+=weight;
	if(recon->edgewtracking)continue;
	Nedge+=weight;
      }
    }



  }//Event Loop
  
  //######## Write and Close ####################

  if(fileout){
    ofstream outfile(Output);
    outfile<<Nall<<endl;
    outfile<<Nactpln<<endl;
    outfile<<Nlayerpe<<endl;
    outfile<<Ntrack<<endl;
    outfile<<Nmatch<<endl;
    outfile<<Nveto<<endl;
    outfile<<Nedge<<endl;
  }

  cout<<endl;
  cout<<"All events    : "<<Nall<<endl;
  cout<<"active plane  : "<<Nactpln<<endl;
  cout<<"PE / layer    : "<<Nlayerpe<<endl;
  cout<<"Tracking      : "<<Ntrack<<endl;
  cout<<"Track matching: "<<Nmatch<<endl;
  cout<<"Veto cut      : "<<Nveto<<endl;
  cout<<"FV cut        : "<<Nedge<<endl;
  
  
}
