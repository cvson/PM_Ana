//##### Standard C++ lib. ######
#include<iostream>
#include<sstream>
#include<fstream>
using namespace std;
#include <iomanip.h>
#include <sys/stat.h>
//##### Root Library ###########
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
#include <TSystem.h>
#include <TF1.h>
#include <TMath.h>
//##### INGRID Library #########
#include "INGRIDEVENTSUMMARY.h"
#include "IngridHitSummary.h"
#include "IngridSimHitSummary.h"
//##### INGRID Software ########
//#include "INGRID_Dimension.cxx"

//INGRID_Dimension* fINGRID_Dimension;

bool Is_Bad_Channel(IngridHitSummary* thit);


int main(int argc,char *argv[]){
  TROOT        root ("GUI","GUI");
  TApplication app  ("App",0,0);
  //fINGRID_Dimension = new INGRID_Dimension();
  int run_number;
  int calib_run_number;
  int c=-1;
  int sub_run_number;
  int nmodhits[14];
  char FileName[300], Output[300];
  while ((c = getopt(argc, argv, "f:o:")) != -1) {
    switch(c){

    case 'f':
      sprintf(FileName,"%s",optarg);
      break;
    case 'o':
      sprintf(Output  ,"%s",optarg);
      break;
    }
  }




  //#### Read file before calibration ######
  //########################################

  int nhit;

  FileStat_t fs;
  IngridEventSummary* summary = new IngridEventSummary();

  if(gSystem->GetPathInfo(FileName,fs)){
    cout<<"Cannot open file "<< FileName <<endl;
    exit(1);
  }
  TFile*            rfile = new TFile(FileName,"read");
  TTree*             tree = (TTree*)rfile -> Get("tree");
  TBranch*          EvtBr = tree->GetBranch("fDefaultReco.");
  EvtBr                   ->SetAddress(&summary);
  tree                    ->SetBranchAddress("fDefaultReco.", &summary);
  int                nevt = (int)tree -> GetEntries();
  cout << "Total # of events = " << nevt <<endl;

  //#### Create file after calibration ######
  //#########################################

  TFile*            wfile = new TFile(Output,"recreate");
  TTree*            wtree = new TTree("tree","tree");

  IngridHitSummary*    inghitsum;

  wtree->Branch("nmodhits",&nmodhits,"nmodhits[14]/I");


  for(int ievt = 0; ievt < nevt; ++ievt){
  
    if(ievt%1000==0)cout<<"Run#"<<run_number<<"\tadd noise event:"<<ievt<<endl;
    summary -> Clear();
    tree    -> GetEntry(ievt);
    for(int cyc=0; cyc<3; cyc++){
      for(int mod=0; mod<14; mod++){
	nhit = summary -> NIngridModHits(mod, cyc);
	nmodhits[mod] = 0;
	for(int hit=0; hit<nhit; hit++){
	  inghitsum   = (IngridHitSummary*) (summary -> GetIngridModHit(hit, mod, cyc) );
          //if(Is_Bad_Channel( inghitsum ))continue;
          if(inghitsum -> pe < 2.5) continue;
	  if(inghitsum -> pln > 10) continue;
	  nmodhits[mod]++;
	}
      }
      wtree -> Fill();
    }


  }


  wtree -> Write();
  wfile -> Write();
  wfile -> Close();
  //app.Run();
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
