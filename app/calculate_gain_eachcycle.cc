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
#include <TEventList.h>
#include <TBranch.h>
#include <TH1.h>
#include <TGraph.h>
#include <TGaxis.h>
#include <TMarker.h>
#include <TText.h>
#include <TSpectrum.h>
#include <TF1.h>
#include <TMath.h>
#include <TSpectrum.h>
#include <TString.h>
#include <TSystem.h>
#include <TPostScript.h>

#include "TApplication.h"

#include"setup.hxx"
#include"root_setup.hxx"
#include"ana_MPPC.cxx"



int main(int argc,char *argv[]){
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  root_setup froot_setup;
  char buff1[300],buff2[300];

  char FileName[300];
  Int_t run_number;
  Int_t c=-1;
  Double_t without_sigma=3.5;
  while ((c = getopt(argc, argv, "r:h")) != -1) {
    switch(c){
    case 'r':
      run_number = atoi(optarg);
      sprintf(FileName,
	      "/home/daq/data/MPPC_calib/cyc/ingrid_%08d_0000.root",
	      run_number);
      break;
    case 'h':
      cout<<"r:[runnumber]"<<endl;
      break;
    }
  }

  FileStat_t fs;
  cout<<"File Name is "  <<  FileName
      <<" ...."          <<  endl;
  if(gSystem->GetPathInfo(FileName,fs)){
    cout<<"Cannot open file "<<FileName<<endl;
    exit(1);
  }

  TFile*    f     = new TFile       (FileName,"read");
  TTree*    tree  = (TTree*)f->Get  ("tree");
  Int_t nHighAdc[NumMod][2][NumTFB][NumCh][NumCyc];
  tree    -> SetBranchAddress("nHighAdc", nHighAdc);
  Int_t    Nevent = tree->GetEntries();
  
  //######## define histgrams ###################
  //#############################################
  cout<<"define histgrams...."<<endl;
  TH1F*    mppc    [NumMod][2][NumTFB][24][NumCyc];
  for(Int_t nummod=0; nummod<NumMod; nummod++){
    for(Int_t view=0; view<2; view++){
      for(Int_t numtfb=0; numtfb<NumTFB; numtfb++){
	for(Int_t numch=0; numch<24; numch++){
	  for(Int_t numcyc=0; numcyc<NumCyc; numcyc++){
	    if(view == 0)sprintf(buff1,"Mod%02dXTPL%02dCh%02dCyc%02d",
				 nummod, numtfb, numch ,numcyc);
	    if(view == 1)sprintf(buff1,"Mod%02dYTPL%02dCh%02dCyc%02d",
				 nummod, numtfb, numch ,numcyc);
	    mppc[nummod][view][numtfb][numch][numcyc] = new TH1F(buff1,buff1,
								 HIST_MAX-HIST_MIN+1,
								 HIST_MIN,HIST_MAX);
	  }//numcyc
	}//numch
      }//numtfb
    }//view
  }//nummod

  //######## Fill the dat to histgram ###########
  //#############################################
  cout<<"Fill the data to histgram..."<<endl;
  for(Int_t nevent=0; nevent<Nevent; nevent++){
    //if(nevent>100)break;
    if((nevent+1)%100==0)cout<<nevent+1<<"\t end"<<endl;
    tree->GetEntry(nevent);
    for(Int_t nummod=0; nummod<NumMod;nummod++){
      for(Int_t view=0; view<2; view++){
	for(Int_t numtfb=0; numtfb<NumTFB; numtfb++){
	  for(Int_t numch=0; numch<24; numch++){
	    for(Int_t numcyc=0; numcyc<NumCyc; numcyc++){

	      mppc[nummod][view][numtfb][numch][numcyc]
		-> Fill( nHighAdc[nummod][view][numtfb][numch][numcyc] );
	    }//numcyc
	  }//numch
	}//numtfb
      }//view
    }//nummod
  }//event

  //####### analysis and culculate gain #########
  //#############################################
  ana_MPPC* fana_mppc = new ana_MPPC();
  cout<<"analyzing and culculate gain..."<<endl;
  sprintf(buff1, 
	  "/home/daq/data/MPPC_calib/cyc/ingrid_%08d_0000.txt",run_number);
  ofstream wf(buff1);
  //Double_t gain    [NumMod][2][NumTFB][NumCh][NumCyc];
  //Double_t noise   [NumMod][2][NumTFB][NumCh][NumCyc];
  //Double_t pedestal[NumMod][2][NumTFB][NumCh][NumCyc];
  double gain;
  double noise;
  double pedestal;
  for(Int_t nummod=5; nummod<10; nummod++){
    for(Int_t view=0; view<2; view++){
      for(Int_t numtfb=0; numtfb<NumTFB; numtfb++){
	for(Int_t numch=0;numch<24;numch++){
	  for(Int_t numcyc=0; numcyc<NumCyc; numcyc++){

	    if( fana_mppc->analysis_old_version(mppc[nummod][view][numtfb][numch][numcyc]) ){
	      //gain      [nummod][view][numtfb][numch][numcyc]
	      gain
		= fana_mppc -> get_gain();
	      //pedestal  [nummod][view][numtfb][numch][numcyc]
	      pedestal
		= fana_mppc -> get_pedestal();
	      //noise     [nummod][view][numtfb][numch][numcyc]
	      noise
		= fana_mppc -> get_noise(100);
	    }
	    else {
	      //gain      [nummod][view][numtfb][numch][numcyc]
	      gain
		= 777;
	      //pedestal  [nummod][view][numtfb][numch][numcyc]
	      pedestal
		= -777;
	      //noise     [nummod][view][numtfb][numch][numcyc]
	      noise 
		= -777;
	      
	    }
	
	
	    wf   << nummod   <<"\t"
		 << view     <<"\t"
		 << numtfb   <<"\t"
		 << numch    <<"\t"
		 << numcyc   <<"\t"
	      //<< pedestal  [nummod][view][numtfb][numch][numcyc] <<"\t"
		 << pedestal <<"\t"
	      //<< gain      [nummod][view][numtfb][numch][numcyc] <<"\t" 
		 << gain     <<"\t"
		 << endl;

	  }//numcyc
	}//numch
      }//numtfb
    }//view
  }//nummod


  wf.close();
  f  -> Close();

} 
