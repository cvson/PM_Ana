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
#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
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
 // TROOT        root ("GUI","GUI");
 // TApplication app  ("App",0,0);

 //Int_t c=-1;  
 char FileName[800];
 Int_t runID=atoi(argv[1]);
 Int_t aj=0, az=0;
 if (argc>2) aj=atoi(argv[2]);
 if (argc>3) az=atoi(argv[3]);
  
  double total_pot=0;
 for (int ifile=az;ifile<aj+1;ifile++)
 {  
   sprintf(FileName,"/home/daq/data/dst/ingrid_%08d_%04d_processed.root",runID, ifile);

  FileStat_t fs;
  cout<<"reading "<<FileName<<"....."<<endl;
  if(gSystem->GetPathInfo(FileName,fs)){
    cout<<"Cannot open file "<<FileName<<endl;
  //  exit(1);
  }
  
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
  BeamInfoSummary*          beamsum;

  for(int ievt = 0; ievt < nevt; ++ievt)
  {
    if(ievt%1000==0)
      cout << "check event# :" << ievt
	   << "\tpot:"         << total_pot
	   << endl;
  
    evt  -> Clear();
    tree -> GetEntry(ievt);
  
    beamsum    = (BeamInfoSummary*) ( evt -> GetBeamSummary( 0 ) );
    cout<<beamsum->mumon[0]<<" "<<beamsum->mumon[2]<<" "<<beamsum->mumon[4]<<" "<<beamsum->ct_np[4][0]<<endl;

    if( beamsum->spill_flag &&
        sqrt(beamsum->mumon[2]*beamsum->mumon[2] + beamsum->mumon[4]*beamsum->mumon[4])<10 &&
	//31.7<beamsum->mumon[0]/beamsum->ct_np[4][0]*1e9 && 35.1>beamsum->mumon[0]/beamsum->ct_np[4][0]*1e9 &&
	beamsum->ct_np[4][0]>TMath::Power(10,11)&&
	beamsum->hct[0][0] > 240 && 
	beamsum->hct[0][0] < 260 && 
	beamsum->hct[1][0] > 240 && 
	beamsum->hct[1][0] < 260 && 
	beamsum->hct[2][0] > 240 && 
	beamsum->hct[2][0] < 260 )
    {
     total_pot += beamsum->ct_np[4][0];
    } 
  }//evt
   rfile -> Close();
 }

  cout << "----- Summary ----------------------------------"    << endl;
  cout << "\tTotal pot measured by CT05 = " << total_pot << endl;
  cout << "------------------------------------------------"    << endl;

}
 
