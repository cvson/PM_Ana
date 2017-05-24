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
#include <TProfile.h>

#include <TGraph.h>
#include <TGaxis.h>
#include <TMarker.h>
#include <TText.h>
#include <TMath.h>
#include <TSpectrum.h>
#include <TString.h>
#include <TSystem.h>

#include "TApplication.h"
#include "setup.hxx"
#include "root_setup.hxx"

const static double basis_gain   =  11;
const static double basis_ped    = 150;
const static double basis_loped  = 150;
const static double error_gain   =   6;
const static double error_ped    =  50;
const static double error_loped  =  50;

int main(int argc,char *argv[]){
  char buff1[200],buff2[200];
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  root_setup froot_setup;

  //read run number
  Int_t run_number;
  Int_t sub_run_number=0;
  Int_t c=-1;
  char FileName[300];
  Bool_t flWrite=false;
  while ((c = getopt(argc, argv, "r:s:hw")) != -1) {
    switch(c){
    case 'r':
      run_number=atoi(optarg);
      break;
    case 's':
      sub_run_number=atoi(optarg);
      break;
    case 'w':
      flWrite=true;
      break;
    case 'h':
      cout<<"help menu"<<endl;
      cout<<"-r [run_number]"<<endl;
      break;
    }
  }
 
  //##### check file existence #####
  sprintf(buff1,
	  "/home/daq/data/calib_table/ingrid_%08d_%04d_MPPCcalib.txt",
	  run_number, sub_run_number);
  FileStat_t fs;
  if(gSystem->GetPathInfo(buff1,fs)){
    cout<<"Cannot open file "<<buff1<<endl;
    exit(1);
  }
  //##### check calib. path  #######
  ifstream fcalpath("/home/daq/data/calib_table/calib_path.txt");
  int cal_path_runnum, cal_path_subnum;
  while( fcalpath >> cal_path_runnum >> cal_path_subnum ){
    if( cal_path_runnum == run_number ){
      cout << "already there is calib. path of run# " << run_number 
	   << " : sub run number = " << cal_path_subnum
	   << endl;
      exit(1);
    }
  }


  ifstream file(buff1);
  int status, start_time, end_time;
  file >> status >> start_time >> end_time;
  cout << status     << "\t"
       << start_time << "\t"
       << end_time   << "\t"
       << endl;
  //read hist
  int mod, view, pln, ch;
  double gain, pedestal,lopedestal, temp;
  bool badflag = false;
  while(file>>mod>>view>>pln>>ch){
    file>>pedestal>>lopedestal>>gain;
    if( fabs( basis_gain  - gain )       > error_gain ||
	fabs( basis_ped   - pedestal )   > error_ped  ||
	fabs( basis_loped - lopedestal ) > error_loped 
	){
	cout<< "Mod:"       << mod
	    << "\tview:"    << view
	    << "\tpln:"     << pln
	    << "\tch.: "    << ch
	    << "\tpedestal:"<< pedestal
	    << "\tlo:"      << lopedestal
	    << "\tgain:"    << gain
	    << endl;	
	if( !(mod==5 && view==1 && pln==4  && ch==2) && 
	    !(mod==7 && view==0 && pln==14 && ch==7) ) //## sub bad channel
	  badflag = true;
    }

  }
  if( !badflag ){
    ofstream wfcalpath("/home/daq/data/calib_table/calib_path.txt", 
		       std::ios::out | std::ios::app);
    wfcalpath << run_number << "\t"
	      << sub_run_number << "\t"
	      << endl;
    wfcalpath.close();
    cout << "ok. writing path..." << endl;
  }
}
