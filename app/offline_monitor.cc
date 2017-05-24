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
#include <TH2.h>

#include <TGraph.h>
#include <TGaxis.h>
#include <TMarker.h>
#include <TText.h>
#include <TMath.h>
#include <TSpectrum.h>
#include <TString.h>
#include <TSystem.h>
#include <TColor.h>

#include "TApplication.h"
#include "setup.hxx"
#include "root_setup.hxx"
#include "ana_MPPC.cxx"


ana_MPPC *fana_MPPC;

int main(int argc,char *argv[]){
  char buff1[200],buff2[200];
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  gStyle->SetPalette(1,0);
  root_setup froot_setup;

  fana_MPPC = new ana_MPPC();

  //read run number
  Int_t run_number;
  Int_t c=-1;
  char FileName[300];
  while ((c = getopt(argc, argv, "r:h:")) != -1) {
    switch(c){
    case 'r':
      run_number=atoi(optarg);
      break;
    case 'h':
      cout<<"help menu"<<endl;
      cout<<"-r [run_number]"<<endl;
      break;
    }
  }
  char temp[200];

  sprintf(buff1,"%s/ingrid_%08d_0000.daq.mid.new.root",root_file_folder,run_number);
  FileStat_t fs;
  if(gSystem->GetPathInfo(buff1,fs)){
    cout<<"Cannot open file "<<buff1<<endl;
    exit(1);
  }

  TFile *f = new TFile(buff1,"read");
  TTree *IngEvt = (TTree*)f->Get("IngEvt");
  Int_t Adc[NumMod][NumTFB][NumCh][NumCyc];
  sprintf(buff1,"Adc[%d][%d][%d][%d]",NumMod,NumTFB,NumCh,NumCyc);
  IngEvt->SetBranchAddress(buff1,Adc);
  Int_t Nevent = IngEvt->GetEntries();


  //start of create hist
  cout<<"creat hist..."<<endl;
  TH1F *hist_adc[NumMod][NumTFB][NumCh];
  for(Int_t nummod=0;nummod<NumMod;nummod++){
    for(Int_t numtfb=0;numtfb<NumTFB;numtfb++){
      for(Int_t numch=0;numch<NumCh;numch++){
	if(numtfb<11){
	  sprintf(buff1,"Mod%02d_TPL%02d_Ch%02d",nummod,numtfb,numch);
	}
	if(numtfb>=11){
	  sprintf(buff1,"Mod%02d_VETO%02d_Ch%02d",nummod,numtfb-11,numch);
	}
	hist_adc[nummod][numtfb][numch] = new TH1F(buff1,buff1,100,100,200);
	hist_adc[nummod][numtfb][numch]->SetName(buff1);
	hist_adc[nummod][numtfb][numch]->SetXTitle("Adc counts");
	hist_adc[nummod][numtfb][numch]->SetYTitle("# of events");
      }//numch
    }//numtfb
  }//nummod
  //end of create hist
  //start of fill hist from data
  cout<<"fill data..."<<endl;
  TCanvas *c1 = new TCanvas("c1","c1",10,10,800,800);

  
  for(Int_t nevent=0;nevent<Nevent;nevent++){
    IngEvt->GetEntry(nevent);
    for(Int_t nummod=0;nummod<NumMod;nummod++){
	for(Int_t numtfb=0;numtfb<NumTFB;numtfb++){
	    for(Int_t numch=0;numch<NumCh;numch++){
	      //for(Int_t numcyc=0;numcyc<NumCyc;numcyc++){
	      for(Int_t numcyc=0;numcyc<NumCyc;numcyc++){
		if(numtfb<11&&numch<48){//tracking plane
		  hist_adc[nummod][numtfb][numch]->Fill(Adc[nummod][numtfb][numch][numcyc]);
		}
		else if(numtfb>=11&&numch<22){//veto plane
		  hist_adc[nummod][numtfb][numch]->Fill(Adc[nummod][numtfb][numch][numcyc]);
		}
	      }//numcyc
	    }//numch
	  }//numtfb
      }//nummod
  }//nevent
  //end of fill data to hist

  //start of analysis
  Double_t gain,crosstalk_and_afterpulse,noise;
  for(Int_t nummod=0;nummod<NumMod;nummod++){
    for(Int_t numtfb=0;numtfb<15;numtfb++){

      Int_t Ch;
      if(numtfb<11)Ch=48;
      else{Ch=22;}
      bool Flag=true;

      if((nummod<7&&nummod>0)&&numtfb==11)Flag=false;
      if((nummod>7)&&numtfb==13)Flag=false;
      if((nummod==7)&&numtfb==13)Flag=false;

      if(Flag){for(Int_t numch=0;numch<Ch;numch++){

	bool flag=false;

	if(!fana_MPPC->analysis_old_version(hist_adc[nummod][numtfb][numch]))flag=true;
	//if(nummod==5&&numtfb==4&&numch==18)flag=true;
	

	if(nummod==7&&numtfb==14&&numch==7)flag=true;
	if(flag){
	  cout<<nummod<<" "<<numtfb<<" "<<numch<<endl;
	  hist_adc[nummod][numtfb][numch]->Draw();
	  cout<<"chi2:"<<fana_MPPC->get_chi2()<<endl;
	  cout<<"NDF:"<<fana_MPPC->get_ndf()<<endl;
	  c1->Update();
	  cin.get();
	    gain=fana_MPPC->get_gain();
	    //noise=fana_MPPC->get_noise(Nevent*NumCyc);
	    noise=fana_MPPC->get_noise(Nevent*2);
	    //crosstalk_and_afterpulse=fana_MPPC->get_crosstalk_and_afterpulse(Nevent*NumCyc);
	    crosstalk_and_afterpulse=fana_MPPC->get_crosstalk_and_afterpulse(Nevent*2);
	    //}
	cout<<nummod<<"\t"<<numtfb<<"\t"<<numch<<"\t";
	cout<<-444<<"\t"<<-444<<"\t"<<-444<<endl;

	}
	
	else{
	gain=fana_MPPC->get_gain();
	//noise=fana_MPPC->get_noise(Nevent*NumCyc);
	noise=fana_MPPC->get_noise(Nevent*2);
	//crosstalk_and_afterpulse=fana_MPPC->get_crosstalk_and_afterpulse(Nevent*NumCyc);
	crosstalk_and_afterpulse=fana_MPPC->get_crosstalk_and_afterpulse(Nevent*2);
	cout<<nummod<<"\t"<<numtfb<<"\t"<<numch<<"\t";
	cout<<gain<<"\t"<<noise<<"\t"<<crosstalk_and_afterpulse<<endl;
	}


	double pedestal;
	if(nummod==6){
	  gain=fana_MPPC->get_gain();
	  pedestal=fana_MPPC->get_pedestal();
	  noise=fana_MPPC->get_noise(Nevent*NumCyc);
	  crosstalk_and_afterpulse=fana_MPPC->get_crosstalk_and_afterpulse(Nevent*NumCyc);
	  cout<<nummod<<"\t"<<numtfb<<"\t"<<numch<<"\t";
	  cout<<pedestal<<"\t"<<gain<<"\t"<<noise<<"\t"<<crosstalk_and_afterpulse<<endl;

	}
	}}
    }
  }


  TH2F *h2 = new TH2F("","",11*48,0,11*48,100,100,200);
  //fill hist_adc to h2
  cout<<"fill hist_adc to h2..."<<endl;
  for(Int_t nummod=0;nummod<NumMod;nummod++){
    h2->Clear();
    //for(Int_t numtfb=0;numtfb<NumTFB;numtfb++){
    for(Int_t numtfb=0;numtfb<11;numtfb++){
      for(Int_t numch=0;numch<48;numch++){
	
	for(Int_t j=1;j<100;j++){
	  Int_t Nbin = hist_adc[nummod][numtfb][numch] -> GetBinContent(j);
	  h2 -> Fill(48*numtfb+numch,j+100,Nbin);
	}
      }
    }

    h2 -> Draw("colz");
    c1->Update();
    cin.get();
  }
  f->Close();     
}
