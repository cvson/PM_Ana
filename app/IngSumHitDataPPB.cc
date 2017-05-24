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
#include "BeamInfoSummary.h"
#include "IngridBasicReconSummary.h"
#include "IngridSimParticleSummary.h"
#include "IngridSimVertexSummary.h"
#include "IngridTrackSummary.h"
#include "NeutInfoSummary.h"


FileStat_t fs;

int main(int argc,char *argv[]){

  TROOT        root ("GUI","GUI");
  TApplication app  ("App",0,0);
  //fINGRID_Dimension = new INGRID_Dimension();
  int  c=-1;
  char FileName[300], Output[300], Output2[300], Output3[300];
  bool usecyc[8];
  bool useevt;
  Int_t Scyc=4;
  Int_t Ncyc=12;
  Int_t ppbbin=-1;
  Float_t lppb, hppb;
  int oevt=-1,ocyc=-1;
  int ndummyhit,nallhits;

  while ((c = getopt(argc, argv, "r:f:o:p:q:")) != -1) {
    switch(c){
    case 'r':
      ppbbin=atoi(optarg);
      break;
    case 'f':
      sprintf(FileName,"%s",optarg);
      break;
    case 'o':
      sprintf(Output,"%s",optarg);
      break;
    case 'p':
      sprintf(Output2,"%s",optarg);
      break;
    case 'q':
      sprintf(Output3,"%s",optarg);
      break;
    }
  }

  if(ppbbin<0||ppbbin>10){
  //if(ppbbin<0||ppbbin>54){
    cout<<"r must be 0 to 10"<<endl;
    //cout<<"r must be 0 to 54"<<endl;
    exit(1);
  }
  
  lppb=2e12+1e12*ppbbin;
  hppb=lppb+1e12;
  //lppb=2e12+0.2e12*ppbbin;
  //hppb=lppb+0.2e12;

  //#### Read file before summing dummy hits ####
  //#############################################
  
  if(gSystem->GetPathInfo(FileName,fs)){
    cout<<"Cannot open file "<<FileName<<endl;
    exit(1);
  }
  
  TFile*            rfile = new TFile(FileName,"read");
  TTree*             tree = (TTree*)rfile -> Get("tree");
  IngridEventSummary* summary = new IngridEventSummary();
  TBranch*          EvtBr = tree->GetBranch("fDefaultReco.");
  EvtBr                   ->SetAddress(&summary);
  tree                    ->SetBranchAddress("fDefaultReco.", &summary);
  int                nevt = (int)tree -> GetEntries();
  
  
  TFile*            afile = new TFile(FileName,"read");
  TTree*            atree = (TTree*)afile -> Get("tree");
  IngridEventSummary* asummary = new IngridEventSummary();
  TBranch*          aEvtBr = atree->GetBranch("fDefaultReco.");
  aEvtBr                   ->SetAddress(&asummary);
  atree                    ->SetBranchAddress("fDefaultReco.", &asummary);
  int                nevt2 = (int)atree -> GetEntries();
  
  
  int Nevt;
  if(nevt<nevt2) Nevt=nevt;
  else           Nevt=nevt2;
  cout << "Total # of events = " << Nevt <<endl;
  
  //#### Create file after adding noise ######
  //##########################################
  

  
  TFile*              nfile    = new TFile(Output2,"recreate");
  TTree*              ntree    = new TTree("tree","tree");
  IngridEventSummary* nsummary = new IngridEventSummary(); 
  ntree              -> Branch   ("fDefaultReco.","IngridEventSummary", 
				  &nsummary,  64000,   99);

  TFile*              wfile    = new TFile(Output,"recreate");
  TTree*              wtree    = new TTree("tree","tree");
  IngridEventSummary* wsummary = new IngridEventSummary(); 
  wtree              -> Branch   ("fDefaultReco.","IngridEventSummary", 
				  &wsummary,  64000,   99);
  
  TFile*              ofile    = new TFile(Output3,"recreate");
  TTree*              otree    = new TTree("tree","tree");
  IngridEventSummary* osummary = new IngridEventSummary(); 
  otree              -> Branch   ("fDefaultReco.","IngridEventSummary", 
				  &osummary,  64000,   99);
  
  
  IngridHitSummary* inghitsum;  IngridHitSummary* inghitsum2;
  BeamInfoSummary* binfo; BeamInfoSummary* binfo2;
  
  for(int ievt = 0; ievt < Nevt; ++ievt){
    
    if(ievt%1000==0)cout<<"Event:"<<ievt<<endl;
    
    summary -> Clear();
    asummary -> Clear();
    wsummary -> Clear();
    nsummary -> Clear();
    osummary -> Clear();
    
    tree    -> GetEntry(ievt);
        
    binfo = (BeamInfoSummary*)( summary -> GetBeamSummary(0) );
    
    memset(usecyc,false,sizeof(usecyc));
    useevt=false;
    binfo->ct_np[4][0]=0;
    for(int i=0;i<8;i++){
      if(binfo->ct_np[4][i+1]>lppb&&binfo->ct_np[4][i+1]<hppb){
	usecyc[i]=true;
	useevt=true;
      }
      else{
	binfo->ct_np[4][i+1]=0;
      }
      binfo->ct_np[4][0]+=binfo->ct_np[4][i+1];
    }
    nsummary -> AddBeamSummary(binfo);
    osummary -> AddBeamSummary(binfo);

    if(!useevt)continue;
      
    for(int cyc=Scyc; cyc<Ncyc; cyc++){
      if((!usecyc[cyc-4]))continue;
      else if(oevt<0){
	oevt=ievt;
	ocyc=cyc;
	continue;
      }

      atree -> GetEntry(oevt);
      binfo2 = (BeamInfoSummary*)( asummary -> GetBeamSummary(0) );
      binfo->ct_np[4][1+cyc-4]+=binfo2->ct_np[4][1+ocyc-4];
      binfo->ct_np[4][0]      +=binfo2->ct_np[4][1+ocyc-4];

      for(int mod=0; mod<14; mod++){

	int ninghit = summary -> NIngridModHits(mod, cyc);
	
	//#### Set true hit's dummy flag = false ###
	//##########################################
        for(int i=0; i<ninghit; i++){
          inghitsum   = (IngridHitSummary*) (summary -> GetIngridModHit(i, mod, cyc) );
	  wsummary   -> AddIngridModHit(inghitsum,mod, cyc);
	  nsummary   -> AddIngridModHit(inghitsum,mod, cyc);
	  osummary   -> AddIngridModHit(inghitsum,mod, cyc);

	  inghitsum  -> dummy = false;
	}
	
	
	//#### generate dummy noise hit ####
	//##################################
	
	for(int k=0;k<2;k++){
	  ndummyhit = asummary -> NIngridModHits(mod, ocyc+8*k);
	  for(int idummyhit = 0; idummyhit < ndummyhit; idummyhit++){
	    inghitsum   = (IngridHitSummary*) (asummary -> GetIngridModHit(idummyhit, mod, ocyc+8*k) );
	    inghitsum -> cyc = cyc;
	    inghitsum -> time += (cyc-ocyc-8*k)*581;
	    if(k==0)
	      wsummary   -> AddIngridModHit(inghitsum,mod, cyc);
	    else
	      nsummary   -> AddIngridModHit(inghitsum,mod, cyc);
	  }//idummyhit
	  

	  
      	  if(k==0)
	    nallhits=wsummary->NIngridModHits(mod,cyc);
	  else
	    nallhits=nsummary->NIngridModHits(mod,cyc);
	  
	  for(int ihit1=0; ihit1 < nallhits; ihit1++){
	    
	    if(k==0)
	      inghitsum = (IngridHitSummary*)(wsummary->GetIngridModHit(ihit1, mod, cyc));
	    else
	      inghitsum = (IngridHitSummary*)(nsummary->GetIngridModHit(ihit1, mod, cyc));

	    for(int ihit2=ihit1+1; ihit2 < nallhits; ihit2++){

	      if(k==0)
		inghitsum2 = (IngridHitSummary*)(wsummary->GetIngridModHit(ihit2, mod, cyc));
	      else
		inghitsum2 = (IngridHitSummary*)(nsummary->GetIngridModHit(ihit2, mod, cyc));
	      
	      if( inghitsum -> pln  == inghitsum2  -> pln  &&
		  inghitsum -> ch   == inghitsum2  -> ch   &&
		  inghitsum -> view == inghitsum2  -> view 		  
		  ){
		if( inghitsum -> time > inghitsum2 -> time ){
		  inghitsum  -> cyc = -2;
		  inghitsum2  -> pe += inghitsum  -> pe;
		}
		if( inghitsum -> time < inghitsum2 -> time ){
		  inghitsum2 -> cyc = -2;
		  inghitsum  -> pe += inghitsum2  -> pe;
		  
		}
	      }
	    }
	  }
	  
	}
	


      }//mod
      ocyc=cyc;
      oevt=ievt;
    }//cyc

    wsummary -> AddBeamSummary(binfo);

    wtree -> Fill();
    ntree -> Fill();
    otree -> Fill();
  }


  wtree -> Write();
  wfile -> Write();
  wfile -> Close();


  ntree -> Write();
  nfile -> Write();
  nfile -> Close();

  otree -> Write();
  ofile -> Write();
  ofile -> Close();


}
