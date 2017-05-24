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

int main(int argc,char *argv[]){

  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  char FileName[300];
  int c=-1; 
  int Year,Mon,Day;
  bool disp=false;
  while ((c = getopt(argc, argv, "y:m:d:s")) != -1) {
    switch(c){
    case 'y':
      Year = atoi( optarg );
      break;
   
    case 'm':
      Mon = atoi( optarg );
      break;

    case 'd':
      Day = atoi( optarg );
      break;

    case 's':
      disp=true;
      break;
    }
  } 

  //time_t ltime;
  //localtime(&ltime);
  time_t ltime,stime,etime;
  ltime = time(NULL)-604800*7+86400*2;
  stime=(ltime-226800)/604800;
  stime=226800+604800*stime-604800;
  etime=stime+604800;
  sprintf(FileName, "DQFlags_INGRID_%04d%02d%02d.dat",Year, Mon, Day);
  ofstream wfile(FileName);
  wfile << "'INGRID'" << " | "
	<< stime      << " | "
	<< etime      << " | "
	<< "0"        << " | "
    	<< ltime      << " | "
	<< "'ver.1.0'"     << " | "
	<< "'Tatsuya Kikawa'"      << " | "
	<< "'0 is OK'"      << " | "
	<< "' '"      << " | "
	<< "'All OK'"      << " | "
	<< endl;
  cout<<FileName<<" was made."<<endl;

  if(disp){



  }

}
