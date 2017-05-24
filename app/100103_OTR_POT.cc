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
#include "analysis_beam.hxx"
#include "setup.hxx"
#include "INGRID_Dimension.hxx"
//#include "root_setup.hxx"

//##### The Variable of INGRID Data  ############
//###############################################
Int_t           nSpill;
Long_t           UTime;
Int_t           NumEvt;
Int_t            Cycle;
Int_t             fMod;

//##### The Variable of Beam Summary file #######
//###############################################
Int_t            spill_flag;
Int_t       good_spill_flag;
Int_t               trg_sec;
Int_t              spillnum;
Int_t         nd280spillnum;
Double_t          beam_time [5] [9];
Double_t              ct_np [5] [9];
Double_t              mumon [12];
Double_t                hct [3] [5];
Double_t                otr [13];
Bool_t                whorn;
Bool_t               wohorn;
Bool_t                horn1;
Bool_t                horn250;

//#### INGRID Local Beam summary variable #######
//###############################################
Bool_t           bunch_flag;


//#### The variable after analyzed ##############
//###############################################
vector<double>*  anape;
Long_t           fTime;
Long_t          fcTime;      //#### with weightened by # of pe
Double_t         fccTime;    //#### with weightened by # of pe
Int_t          nActPln;
Float_t        fMeanPe;
Float_t        fMeanPe_Old;  //#### Old definition of Mean ( before 091229 )
Int_t          nActTPL;      //#### TPL  means usual TPL 1 ~ 9
Int_t          nActVeto;     //#### Veto means usual TPL 0, 10 and VETO
int             MUpAct;
bool          UVETOHit;
double          CTtime;
int              dimch;
bool            ontime;
const static int   DefOnTime=100;

int                  n;
Int_t       Nhitclster;
int            counter;



int main(int argc,char *argv[]){

  char buff1[200],buff2[200];
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  Int_t run_number;
  Int_t c=-1;  char FileName[300];
  bool sflag = false;
  bool bflag = false; //before processing
  bool tflag = false; //before processing
  while ((c = getopt(argc, argv, "sr:hbt")) != -1) {
    switch(c){
    case 's':
      sflag = true;
      break;
    case 'r':
      run_number=atoi(optarg);
      break;
    case 't':
      tflag = true;
      break;
    case 'b':
      bflag = true;
      break;
    
    case 'h':
      cout<<"help menu"<<endl;
      cout<<"-r [run_number]"<<endl;
      break;
    }
  }
  FileStat_t fs;
  ifstream timing;



  //######## read root file #############################
  //#####################################################
  sprintf(buff1,"/home/daq/data/root_new/ingrid_processed_%08d_0000.root",run_number);
  cout<<"reading "<<buff1<<"....."<<endl;
  if(gSystem->GetPathInfo(buff1,fs)){
    cout<<"Cannot open file "<<buff1<<endl;
    exit(1);
  }
  TFile*               f  = new TFile(buff1,"read");
  TTree*            tree  = (TTree*)f->Get("tree");
  Int_t        Evt        = tree->GetEntries();
  cout<<"---- # of Evt    = "<<Evt<<"---------"<<endl;
  tree -> SetBranchAddress ("Cycle"   ,&Cycle         );
  tree -> SetBranchAddress ("fMod"    ,&fMod          );
  tree -> SetBranchAddress ("spill_flag"      , &spill_flag      );
  tree -> SetBranchAddress ("good_spill_flag" , &good_spill_flag );
  tree -> SetBranchAddress ("spillnum"        , &spillnum        );
  tree -> SetBranchAddress ("nd280spillnum"   , &nd280spillnum   );
  tree -> SetBranchAddress ("trg_sec"         , &trg_sec         );
  tree -> SetBranchAddress ("beam_time"       ,  beam_time       );
  tree -> SetBranchAddress ("ct_np"           ,  ct_np           );
  tree -> SetBranchAddress ("mumon"           ,  mumon           );
  tree -> SetBranchAddress ("hct"             ,  hct             );
  tree -> SetBranchAddress ("otr"             ,  otr             );
  tree -> SetBranchAddress ("bunch_flag"      , &bunch_flag      );
  tree -> SetBranchAddress ("whorn"           , &whorn           );
  tree -> SetBranchAddress ("wohorn"          , &wohorn          );
  tree -> SetBranchAddress ("horn1"           , &horn1           );
  tree -> SetBranchAddress ("horn250"         , &horn250           );
 


  TFile*              rf  = new TFile("new.root","recreate");
  TH2F *fHotrxpotwhorn  = new TH2F("OTRwhorn","OTRwhorn",
				   151, -15.5, 15.5, 
				   151, -15.5, 15.5);
  TH2F *fHotrxpotwohorn = new TH2F("OTRwohorn","OTRwohorn",
				   151, -15.5, 15.5, 
				   151, -15.5, 15.5);
  TH2F *fHotrxpothorn1  = new TH2F("OTRhorn1","OTRhorn1",
				   151, -15.5, 15.5, 
				   151, -15.5, 15.5);
  TH2F *fHotrxpothorn250= new TH2F("OTRhorn250","OTRhorn250",
				   151, -15.5, 15.5, 
				   151, -15.5, 15.5);

  for(n=0; n<Evt; n++){
    tree->GetEntry(n);
    if( n % 10000 == 0 )cout<<n<<endl;
    if( fMod==0 && Cycle ==0 ){
      double x   = otr  [0];
      double y   = otr  [1];
      double pot = ct_np[4][0];
      if( x < 16 && y < 16 ){
	if     ( whorn  )  fHotrxpotwhorn    -> Fill(x, y, pot);
	else if( horn1  )  fHotrxpothorn1    -> Fill(x, y, pot);
	else if( wohorn )  fHotrxpotwohorn   -> Fill(x, y, pot);
	if     (horn250 )  fHotrxpothorn250  -> Fill(x, y, pot);
      }
    }
  }

  //######## Write and Close ####################
  fHotrxpotwhorn  -> Write();
  fHotrxpothorn1  -> Write();
  fHotrxpotwohorn -> Write();
  rf      -> Write();
  rf      -> Close();
  f       -> Close();
  cout<<counter<<endl;
}
 
