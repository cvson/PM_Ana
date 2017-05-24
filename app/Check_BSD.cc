//##### Standard C++ lib. ######
#include<iostream>
#include<sstream>
#include<fstream>
using namespace std;
#include <iomanip.h>
#include <sys/stat.h>
#include <math.h> 
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
FileStat_t fs;

Double_t          ct_np[5][9];
Double_t            hct[3][5];
Int_t           trg_sec[3];
Int_t             nurun;
Int_t    good_gps_flag;
Int_t  good_spill_flag;
Int_t       spill_flag;
Int_t         run_type;
Int_t         spillnum;
Double_t          mumon[12];
bool normal_cond;
bool beamtrg;
bool mucenter;
bool muct;
bool GoodSpillv01();
bool GoodSpill();
bool HornOff();
bool Horn320();
bool v01    ;
bool hornoff;
bool horn320;


int main(int argc, char *argv[]){
  TROOT        root ("GUI","GUI");
  TApplication app  ("App",0,0);
 
  int             c  =     -1;
  char FileName[300];
  v01 = false; hornoff = false; horn320 = false;
  while ((c = getopt(argc, argv, "f:voa")) != -1) {
    switch(c){
    case 'f':
      sprintf(FileName,"%s", optarg);
      break;
    case 'v':
      v01 = true;
      break;
    case 'o':
      hornoff = true;
      break;
    case 'a':
      horn320 = true;
      break;
    }
  }
  if(gSystem -> GetPathInfo(FileName, fs)){
    exit(1);
  }

  TFile* rfile  = new TFile( FileName, "read" );
  TTree* tree   = (TTree*)rfile->Get("bsd");
  tree -> SetBranchAddress("ct_np",                     ct_np);
  tree -> SetBranchAddress("nurun",                   &nurun );
  tree -> SetBranchAddress("hct",                         hct);
  tree -> SetBranchAddress("trg_sec",                 trg_sec);
  tree -> SetBranchAddress("good_gps_flag",    &good_gps_flag);
  tree -> SetBranchAddress("good_spill_flag",&good_spill_flag);
  tree -> SetBranchAddress("spill_flag",          &spill_flag);
  tree -> SetBranchAddress("run_type",              &run_type);
  tree -> SetBranchAddress("spillnum",              &spillnum);
  tree -> SetBranchAddress("mumon",                     mumon);
  int        nevt   = tree -> GetEntries();

  int    count_good_spill = 0;
  double count_good_pot   = 0;
  int    count_normal_spill = 0;
  double count_normal_pot   = 0;


  ofstream wfile("BSD_GoodSpill.txt");
  //output file "<nu DAQ run #>\t<spill #>"
  for(int ievt = 0; ievt < nevt; ievt++){  
    if(ievt%100000==0){
      cout<<"\tcheck event:"<<ievt<<endl;
    }
    tree     -> GetEntry(ievt);


    bool ok = false;
    if     ( v01 && GoodSpillv01()&& !hornoff && !horn320 )
      ok = true;
    else if( !v01 && GoodSpill()  && !hornoff && !horn320 )
      ok = true;
    else if( horn320 && Horn320() && nurun == 340140)
      ok = true;
    else if( hornoff && HornOff() && (nurun==340112||nurun==340113||nurun==340114))
      ok = true;


    if( ok ){
       count_good_spill ++;
       count_good_pot   += ct_np[4][0];
       wfile << nurun << "\t" << spillnum << endl;


    }

  }




  wfile.close();
  cout << "-------------- good -------------------------------" << endl;
  cout << "total spill: "  << count_good_spill 
       << "\t total pot: " << count_good_pot
       << endl;  
  cout << "-------------- normal -----------------------------" << endl;
  cout << "total spill: "  << count_normal_spill 
       << "\t total pot: " << count_normal_pot
       << endl;  

  cout << "create BSD_GoodSpill.txt" << endl;
}

bool GoodSpillv01(){
  if( good_spill_flag )
    return true;
  else
    return false;
}


bool GoodSpill(){
  //#### Bad Spill because MUSi y at Run29 ~ 30#####   
  if( 598833  <= spillnum && spillnum <= 598837  && trg_sec[0] < 1269071040 )
    return false;
  if( 598937  <= spillnum && spillnum <= 598941  && trg_sec[0] < 1269071040 )
    return false;
  if( 1201242 <= spillnum && spillnum <= 1201270 && trg_sec[0] < 1269071040 )
    return false;
  if( 1202461 <= spillnum && spillnum <= 1202533 && trg_sec[0] < 1269071040 )
    return false;
  if( 1234283 <= spillnum && spillnum <= 1234304 && trg_sec[0] < 1269071040 )
    return false;
  if( 1235436 <= spillnum && spillnum <= 1235450 && trg_sec[0] < 1269071040 )
    return false;

  //#### Bad Spill at Run31 #####   
  if( 1269071040  <= trg_sec[0] && trg_sec[0] <= 1269071460  )
    return false;
  if( 1269097200  <= trg_sec[0] && trg_sec[0] <= 1269124200  )
    return false;
  if( 1269143340  <= trg_sec[0] && trg_sec[0] <= 1269143460  )
    return false;

  normal_cond = 
    ( run_type == 1  ) &&
    ( hct[0][0] > 10 ) &&
    ( hct[1][0] > 10 ) &&
    ( hct[2][0] > 10 );

  beamtrg     =
    ( ct_np[4][0] > 1e11 );

  mucenter    = 
    ( sqrt( mumon[2] * mumon[2] + mumon[4] * mumon[4] ) < 10 );

  muct        =
    ( 31.7 < ( mumon[0] / ct_np[4][0] * 1e9 ) ) &&
    ( ( mumon[0] / ct_np[4][0] * 1e9 ) < 35.1 );

  if( normal_cond && beamtrg && mucenter && muct && good_gps_flag )
    return true;
  else
    return false;
}

bool Horn320(){
  if( ct_np[4][0] > 1e11 ){
    if( fabs( hct[0][0] - 320 ) < 5  &&
	fabs( hct[1][0] - 320 ) < 5  &&
	fabs( hct[2][0] - 320 ) < 5 ){
      return true; 
   }
  }
  return false;
}
bool HornOff(){
  if( ct_np[4][0] > 1e11 ){
    if( fabs( hct[0][0] - 0 ) < 5  &&
	fabs( hct[1][0] - 0 ) < 5  &&
	fabs( hct[2][0] - 0 ) < 5 ){
      return true; 
   }
  }
  return false;
}

