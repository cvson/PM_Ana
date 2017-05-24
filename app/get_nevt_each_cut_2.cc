//##### Standard C++ lib. ######
#include<iostream>
#include<sstream>
#include<fstream>
using namespace std;
#include <iomanip.h>
#include <math.h>
#include <sys/stat.h>
//##### Root Library ###########
#include <TROOT.h>
#include <TStyle.h>
#include <TH1.h>
#include <TH2.h>

#include <TApplication.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TClonesArray.h>
#include <TObject.h>
#include <TEventList.h>
#include <TBranch.h>
#include <TSystem.h>
#include <TBrowser.h>
//##### INGRID Library #########
#include "INGRIDEVENTSUMMARY.h"
#include "IngridHitSummary.h"
#include "IngridSimHitSummary.h"
#include "IngridBasicReconSummary.h"





int main(int argc,char *argv[]){
  TROOT        root ("GUI","GUI");
  TApplication app  ("App",0,0);

  Int_t c=-1;  char FileName[300]; char Output[300];
  Int_t run_number=0;
  bool mc = false;
  sprintf(Output,"temp.root");
  while ((c = getopt(argc, argv, "r:f:o:m")) != -1) {
    switch(c){
    case 'r':
      run_number=atoi(optarg);
      //sprintf(FileName,"/home/daq/data/dst/ingrid_%08d_0000_recon.root",run_number);
      sprintf(FileName,"/home/daq/data/dst/ingrid_%08d_0000_processed.root",run_number);
      break;
    case 'f':
      sprintf(FileName,"%s",optarg);
      break;
    case 'o':
      sprintf(Output,"%s",optarg);
      break;
    case 'm':
      mc = true;
      break;
    }
  }
  FileStat_t fs;
  cout<<"reading "<<FileName<<"....."<<endl;
  if(gSystem->GetPathInfo(FileName,fs)){
    cout<<"Cannot open file "<<FileName<<endl;
    exit(1);
  }
  
  //#### Read ROOT File after basic reconstruction ######
  //#####################################################
  TFile*            rfile = new TFile(FileName,"read");
  TTree*             tree = (TTree*)rfile -> Get("tree");
  TBranch*          EvtBr = tree->GetBranch("fDefaultReco.");
  IngridEventSummary* evt = new IngridEventSummary();
  EvtBr                   ->SetAddress(&evt);
  tree                    ->SetBranchAddress("fDefaultReco.", &evt);
  int               tnevt = (int)tree -> GetEntries();
  cout << "Total # of events = " << tnevt <<endl;
  BeamInfoSummary*          beamsum;

  //### neutrino event ####
  //#######################
  int nevt_mod[14][10];  //### 14 : number of modules
                         //### 10 : number of kinds of cut
                         //###  0 : good spills @ INGRID 
                         //###  1 : nactpln > 2
                         //###  2 : layerpe > 6.5
                         //###  3 : tracking successful
                         //###  4 : track matching
                         //###  5 : on time
                         //###  6 : upstream VETO selection
                         //###  7 : Fiducial cut

  int nevt    [10];      //### for all module

  //### beam event ########
  //#######################
  int nevt_beam_mod[14][10];  //### 14 : number of modules
                              //### 10 : number of kinds of cut
                              //###  0 : good spills @ INGRID 
                              //###  1 : nactpln > 1
                              //###  2 : layerpe > 6.5
                              //###  3 : on time
  int nevt_beam    [10];      //### for all module

  //### back ground event #
  //#######################
  int nevt_bg_mod[14];        //### same selection for neutrino beam selection
                              //### apply to cyc 17~22(time>10100)


  int nevt_bg;      //### for all module



  nevt_bg = 0;
  for(int i=0; i<10; i++){
    nevt[i] = 0;
    nevt_beam[i] = 0;
    for(int j=0; j<14; j++){
      nevt_bg_mod[j] = 0;
      nevt_mod[j][i] = 0;
      nevt_beam_mod[j][i] = 0;
    }
  }

  IngridBasicReconSummary* brec;
  IngridSimVertexSummary*  simver;
  for(int ievt = 0; ievt < tnevt; ++ievt){
    if(ievt%100==0)cout<<ievt<<endl;
    evt                   -> Clear();
    tree                  -> GetEntry(ievt);
    int nbasicrecon = evt -> NIngridBasicRecons();   
    beamsum    = (BeamInfoSummary*) ( evt -> GetBeamSummary( 0 ) );




    if( beamsum->spill_flag  ){ //### good spills @ INGRID
      for(int i=0; i<14; i++)nevt_mod[i][0]++;
      for(int i=0; i<14; i++)nevt_beam_mod[i][0]++;
      nevt[0]++;
      nevt_beam[0]++;



      for( int i=0; i<nbasicrecon; i++ ){
	brec = (IngridBasicReconSummary*) ( evt -> GetBasicRecon( i ) );
	int mod = brec -> hitmod;


	//#### beam related event selection ####
	//######################################
	if( brec -> nactpln > 1 ){ //### nactpln > 2
	  nevt_beam_mod[mod][1]++;
	  nevt_beam[1]++;

	  if( brec -> layerpe > 6.5 ){ //### layerpe > 6.5
	    nevt_beam_mod[mod][2]++;
	    nevt_beam[2]++;

	    if( brec -> ontime ){ //### layerpe > 6.5
	      nevt_beam_mod[mod][3]++;
	      nevt_beam[3]++;
	    }

	  }

	}

	//#### neutrino event selection #####
	//###################################
	if( brec -> nactpln > 2 ){ //### nactpln > 2
	  nevt_mod[mod][1]++;
	  nevt[1]++;

	  if( brec -> layerpe > 6.5 ){ //### layerpe > 6.5
	    nevt_mod[mod][2]++;
	    nevt[2]++;

	    if( brec -> hastrk ){ //### tracking successful
	      nevt_mod[mod][3]++;
	      nevt[3]++;

	      if( brec -> matchtrk ){ //### track matching
		nevt_mod[mod][4]++;
		nevt[4]++;

		if( brec -> ontime ){ //### on time
		  nevt_mod[mod][5]++;
		  nevt[5]++;

		  if( !( brec -> vetowtracking ) ){ //### upstream VETO selection
		    nevt_mod[mod][6]++;
		    nevt[6]++;

		    if( !( brec -> edgewtracking ) ){ //### fiducial cut
		      nevt_mod[mod][7]++;
		      nevt[7]++;

		      if( ( fabs( brec -> angle ) > 20 ) ){ //### angle cut
			nevt_mod[mod][8]++;
			nevt[8]++;
		      }

		    }

		  }

		}// ontime

		//#### back ground ####
		//#####################
		if( brec -> clstime > 10100 ){//### cyc. 17 ~ 22
		  if( !( brec -> vetowtracking ) ){ //### upstream VETO selection
		    if( !( brec -> edgewtracking ) ){ //### fiducial cut
		      nevt_bg_mod[mod]++;
		      nevt_bg++;
		    }
		  }
		}


	      }

	    }

	  }//### layerpe > 6.5
	  
	}//### nactpln > 2

      }//### loop basicrecon

    }//### good spill

  }//### loop event


  //### output to text file
  ofstream wf("nevt_neut.txt");
  for(int i=0; i<10; i++){
    if(nevt[i]==0)
      break;
    cout << nevt[i] << "\t";
    wf   << nevt[i] << "\t";

    if(i>0)
      wf   << 100.0 * nevt[i] / nevt[i-1] << "\t";
    else
      wf   << 100 << "\t";

    for(int j=0; j<14; j++){
      cout << nevt_mod[j][i] << "\t";
      wf   << nevt_mod[j][i] << "\t";
      if(i>0)
	wf   << 100.0 * nevt_mod[j][i] / nevt_mod[j][i-1] << "\t";
      else
	wf   << 100 << "\t";
    }
    cout << endl;
    wf   << endl;
  }
  wf.close();

  ofstream wff("nevt_beam.txt");
  for(int i=0; i<10; i++){
    if(nevt_beam[i]==0)
      break;

    cout << nevt_beam[i] << "\t";
    wff  << nevt_beam[i] << "\t";

    if(i>0)
      wff   << 100.0 * nevt_beam[i] / nevt_beam[i-1] << "\t";
    else
      wff   << 100 << "\t";

    for(int j=0; j<14; j++){
      cout << nevt_beam_mod[j][i] << "\t";
      wff  << nevt_beam_mod[j][i] << "\t";
      if(i>0)
	wff   << 100.0 * nevt_beam_mod[j][i] / nevt_beam_mod[j][i-1] << "\t";
      else
	wff   << 100 << "\t";
    }
    cout << endl;
    wff  << endl;
  }
  wff.close();
	
  ofstream wfff("nevt_bg.txt");
  wfff << nevt_bg << "\t\t";
  for(int i=0; i<14; i++){
    wfff << nevt_bg_mod[i] << "\t\t";
  }
  wfff<<endl;
  wfff.close();

}
 
