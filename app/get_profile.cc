
//##### Standard C++ lib. ######
#include<iostream>
#include<sstream>
#include<fstream>
#include<math.h>
using namespace std;
#include <iomanip.h>
#include <sys/stat.h>
//##### Root Library ###########
#include <TROOT.h>
#include <TStyle.h>
#include <TH1F.h>
#include <TApplication.h>
#include <TGraphErrors.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TClonesArray.h>
#include <TObject.h>
#include <TEventList.h>
#include <TBranch.h>
#include <TSystem.h>
#include <TBrowser.h>
#include <TMath.h>
#include <TLatex.h>
//##### INGRID Library #########
#include "INGRIDEVENTSUMMARY.h"
#include "IngridHitSummary.h"
#include "IngridSimHitSummary.h"
#include "IngridBasicReconSummary.h"

const static float DISTANCE_MODULE = 150;
const static float WIDTH_MODULE = 120;

int main(int argc,char *argv[]){
  TROOT        root ("GUI","GUI");
  TApplication app  ("App",0,0);

  Int_t c=-1;  char FileName[300]; char Output[300];
  Int_t run_number=0;
  bool  mc = false;
  sprintf(Output,"temp.root");
  while ((c = getopt(argc, argv, "f:b:o:")) != -1) {
    switch(c){
    case 'f':
      sprintf(FileName,"%s",optarg);
      break;

    case 'o':
      sprintf(Output,"%s",optarg);
      break;
    }
  }
  FileStat_t fs;
  cout<<"reading "<<FileName<<"....."<<endl;
  if(gSystem->GetPathInfo(FileName,fs)){
    cout<<"Cannot open file "<<FileName<<endl;
    exit(1);
  }
  double totalpot=0; 
  //#### Read ROOT File after basic reconstruction ######
  //#####################################################
  TFile*            rfile = new TFile(FileName,"read");
  TTree*             tree = (TTree*)rfile -> Get("tree");
  TBranch*          EvtBr = tree->GetBranch("fDefaultReco.");
  IngridEventSummary* evt = new IngridEventSummary();
  EvtBr                   ->SetAddress(&evt);
  tree                    ->SetBranchAddress("fDefaultReco.", &evt);
  int                nevt = (int)tree -> GetEntries();
  cout << "Total # of events = " << nevt <<endl;

  //#### Create file after calibration ######
  //#########################################
  TFile*            f = new TFile(Output,"recreate");

  f->cd();
  TDirectory* histdir = new TDirectory("histdir",
				       "dir. for histogram",
				       "");
  f->cd();
  histdir->Write();
  histdir->cd();

  float nent          [2][7]={0};
  float nent_con      [2][7]={0};
  float nent_v        [2][7]={0};
  float nent_con_20s  [2][7]={0};
  float nent_con_20b  [2][7]={0};
  for(int i=0; i<2; i++){
    for(int j=0; j<2;j++){
      nent[i][j]=0;
      nent_con[i][j]=0;
      nent_v[i][j]=0;
      nent_con_20s[i][j]=0;
      nent_con_20b[i][j]=0;
    }
  }
  IngridBasicReconSummary*  basicrecon;

  for(int ievt = 0; ievt < nevt; ++ievt){
    //if( ievt%100 == 0 )cout<<ievt<<endl;
      int i2print = int(nevt/100.);
      if( ievt%i2print == 0 )cout<<"Processing "<<int(ievt*100/float(nevt))<<"% of events"<<endl;
    evt  -> Clear();
    tree -> GetEntry(ievt);


    for( int ibasic=0; ibasic < evt -> NIngridBasicRecons(); ibasic++ ){

      basicrecon = (IngridBasicReconSummary*) ( evt -> GetBasicRecon( ibasic ) );

      //### beam related event ###
      //##########################
      Bool_t  beamreg  = 
	(  basicrecon -> layerpe > 6.5 ) &&
	(  basicrecon -> nactpln >= 2  ) &&
	(  basicrecon -> ontime        ) ;
      int hitmod = basicrecon->hitmod;
      if( beamreg ){
	if(hitmod<7)
	  nent[0][hitmod]++;
	if(hitmod>=7)
	  nent[1][hitmod-7]++;
      }


      //### neutrino event ###
      //######################
      Bool_t  sigreg  = 
	(  basicrecon  -> layerpe > 6.5 ) &&
	(  basicrecon  -> nactpln >   2 ) &&
	(  basicrecon  -> hastrk        ) &&
	(  basicrecon  -> matchtrk      ) &&
	//(  basicrecon  -> ontime        ) &&
	!(  basicrecon -> vetowtracking ) &&
	!(  basicrecon -> edgewtracking ) ;
      if( sigreg ){
	if(hitmod<7)
	  nent_con[0][hitmod]++;
	if(hitmod>=7)
	  nent_con[1][hitmod-7]++;
	//### angle cut
	double angle   = basicrecon -> angle;


      }

      //### rock muon enhanced sample ###
      //#################################
      Bool_t  rockmu  = 
	(  basicrecon  -> layerpe > 6.5 ) &&
	(  basicrecon  -> nactpln >   2 ) &&
	(  basicrecon  -> hastrk        ) &&
	(  basicrecon  -> matchtrk      ) &&
	(  basicrecon  -> ontime        ) &&
	( (  basicrecon -> vetowtracking ) ||
	  (  basicrecon -> edgewtracking ) ) ;
      if( rockmu ){
	if(hitmod<7)
	  nent_v[0][hitmod]++;
	if(hitmod>=7)
	  nent_v[1][hitmod-7]++;
      }

    }
  }

  for(int i=0; i<2; i++){
    for(int j=0; j<7;j++){
      cout  << nent[i][j]
	    << "\t"      << nent_v[i][j]
	    << endl;
    }
  }


  //#### 
  float x[7]             = {0};
  float x_error[7]       = {0};
  float nent_error[2][7] = {0}; 
  float nent_v_error[2][7] = {0}; 
  float nent_con_error[2][7] = {0}; 
  for(int i=0; i<2; i++){
    for(int j=0; j<7; j++){
      nent_error[i][j]     = sqrt( nent[i][j] );
      nent_con_error[i][j] = sqrt( nent_con[i][j] );
      nent_v_error[i][j] = sqrt( nent_v[i][j] );
      x[j]       = ( j - 3 ) * DISTANCE_MODULE;
      //x_error[j] = 1.0 * WIDTH_MODULE / sqrt(3) / 2;
      x_error[j] = 0;


    }
  }
  f->cd();


  TGraphErrors *h  = new TGraphErrors(7, x,     nent[0],
				      x_error,nent_error[0]);
  TGraphErrors *hv = new TGraphErrors(7, x, nent_v[0], 
				      x_error, nent_v_error[0]);
  TGraphErrors *hc = new TGraphErrors(7, x, nent_con[0], 
				      x_error, nent_con_error[0]);

  TGraphErrors *v  = new TGraphErrors(7, x,  nent[1],
				      x_error, nent_error[1]);
  TGraphErrors *vv = new TGraphErrors(7, x, nent_v[1],
				      x_error, nent_v_error[1]);
  TGraphErrors *vc = new TGraphErrors(7, x, nent_con[1],
				      x_error, nent_con_error[1]);




  TH1F* fHhpro = new TH1F("fHhpro",
			  "horizontal profile",
			  1000,-500,500);
  TH1F* fHvpro = new TH1F("fHvpro",
			  "vertical profile",
			  1000,-500,500);
  TH1F* fHhcenter = new TH1F("fHhcenter","fHhcenter",
			     20000,-5,5);
  TH1F* fHvcenter = new TH1F("fHvcenter","fHvcenter",
			     20000,-5,5);


  fHhcenter -> Fill( 0.0, nent_con[1][3]*1.2);
  fHvcenter -> Fill(-1.9, nent_con[1][3]*1.2);
  fHhcenter -> SetLineStyle(2);
  fHvcenter -> SetLineStyle(2);
  fHhcenter -> SetLineColor(2);
  fHvcenter -> SetLineColor(2);


  //fHhcenter -> cd();
  //TPad* fpadn     = new TPad("fpadn", "fpadn",
  //		     0.2, 0.1, 0.3, 0.2);
  //fpadn -> Draw();
  //fpadn  -> cd();
  TLatex* ftexn = new TLatex();
  ftexn -> SetText(0.0, 0.0, "north");
  //ftexn -> Draw();


  fHhpro -> SetMaximum( nent_con[1][3]*1.2 );
  fHvpro -> SetMaximum( nent_con[1][3]*1.2 );
  fHhpro -> SetMinimum( 0 );
  fHvpro -> SetMinimum( 0 );
  fHhpro -> SetXTitle ("x[cm] from INGRID center");
  fHvpro -> SetXTitle ("y[cm] from INGRID center");
  fHhpro -> SetYTitle ("Number of events");
  fHvpro -> SetYTitle ("Number of events");


  h->SetName("hpro");
  h->Write();
  hv->SetName("hprov");
  hv->Write();
  hc->SetName("hproc");
  hc->Write();

  v->SetName("vpro");
  v->Write();
  vv->SetName("vprov");
  vv->Write();
  vc->SetName("vproc");
  vc->Write();

  f -> cd();
  f -> Write();
  f -> Close();

  rfile -> Close();

}
 
