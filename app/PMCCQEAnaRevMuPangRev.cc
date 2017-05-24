#define NWEIGHTS 175

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
//#include "analysis_beam.hxx"
//#include "PMrecon.hxx"
//#include "IngAna.hxx"
//#include "PMdisp.h"

#include "INGRIDEVENTSUMMARY.h"
#include "IngridHitSummary.h"
#include "IngridSimHitSummary.h"
#include "IngridSimVertexSummary.h"
#include "IngridSimParticleSummary.h"
#include "BeamInfoSummary.h"
#include "IngridBasicReconSummary.h"
#include "PMReconSummary.h"
#include "NeutInfoSummary.h"


int main(int argc,char *argv[]){


  char buff1[200];//,buff2[200];
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  Int_t c=-1;
  char Output[300];
  char KinFile[300];
  char WeightFile[300];
  bool uset2krwt=false;
  bool usekin=false;
  bool interaction=false;
  while ((c = getopt(argc, argv, "o:f:k:w:i")) != -1) {
    switch(c){
    case 'o':
      sprintf(Output,"%s",optarg);
      break;
    case 'f':
      sprintf(buff1,"%s",optarg);
      break;
    case 'k':
      sprintf(KinFile,"%s",optarg);
      usekin=true;
      break;
    case 'w':
      sprintf(WeightFile,"%s",optarg);
      uset2krwt=true;
      break;
    case 'i':
      interaction=true;
      break;
    }
  }
  FileStat_t fs;
  
  //######## read root file #############################
  //#####################################################
  cout<<"reading "<<buff1<<"....."<<endl;
  if(gSystem->GetPathInfo(buff1,fs)){
    cout<<"Cannot open file "<<buff1<<endl;
    exit(1);
  }
  cout << buff1 << endl;
  IngridEventSummary* evt = new IngridEventSummary();
  //TFile*            rfile = new TFile(Form("/home/daq/data/dst/ingrid_%08d_%04d_processed.root", 
  TFile*            rfile = new TFile(buff1,
				      "read");
  TTree*             tree = (TTree*)rfile -> Get("tree");
  TBranch*          EvtBr = tree->GetBranch("fDefaultReco.");
  EvtBr                   ->SetAddress(&evt);
  tree                    ->SetBranchAddress("fDefaultReco.", &evt);
  int                nevt = (int)tree -> GetEntries();
  cout << "Total # of events = " << nevt <<endl;

  if( nevt==0 )
    exit(1);

  //##########    SK h1 file    ###########
  //#######################################
  TFile*          kinfile;
  TTree*          kintree;
  int               nevt2;
  int mode;
  int numnu;
  int ipnu[50];
  float pnu[50];
  float dirnu[50][3];

  float true_muang,true_pang;
  float true_mumom,true_pmom;

  
  if(usekin){
    kinfile = new TFile(KinFile,"read");
    kintree = (TTree*)kinfile -> Get("h1");

    kintree -> SetBranchAddress("mode", &mode);//Neutrino interaction mode      
    kintree -> SetBranchAddress("numnu", &numnu);//Number of particle           
    kintree -> SetBranchAddress("ipnu", ipnu);//Particle code                   
    kintree -> SetBranchAddress("pnu", pnu);//Abs. momentum                     
    kintree -> SetBranchAddress("dirnu", dirnu);//Particle direction  

    nevt2  = (int)kintree -> GetEntries();
    if(nevt!=nevt2){
      cout<<"Number of events it does not match."<<endl;
      exit(1);
    }
  }


  //########## T2KReWeight file ###########
  //#######################################
  TFile*            rwfile;
  TTree*      weightstree;
  int               nevt3;
  int            nweights;
  TArrayF        *weights=0;
  if(uset2krwt){
    rwfile = new TFile(WeightFile,"read");
    weightstree = (TTree*)rwfile -> Get("weightstree");
    weightstree            -> SetBranchAddress("nweights", &nweights);
    weightstree            -> SetBranchAddress("weights", &weights);
    nevt3  = (int)weightstree -> GetEntries();
    if(nevt!=nevt3){
      cout<<"Number of events it does not match."<<endl;
      exit(1);
    }
  }


  float mu[4],p[4],nu[4],nup[4],numu[4];
  int x,y,z;


  //#### make rootfile after analysis #####
  //#######################################
  
  float norm,totcrsne,muang,pang,mupe,ppe,opening,coplanarity,nuE,veract,mumucl,pmucl,muang_t,pang_t;
  int Ntrack,Ningtrack,inttype,mufc,pfc,mupdg,ppdg;
  float reweight[NWEIGHTS];
  float range,prange;
  int pene,ppene;
  float xnu,ynu,znu,exptime;
  float startxch,startych;
  int startxpln,startypln;


  mu[3]=1;p[3]=1;nu[1]=0;nu[2]=-tan(3.65/180*3.141592);nu[3]=1;
  nu[0]=sqrt(nu[1]*nu[1]+nu[2]*nu[2]+nu[3]*nu[3]);

  TFile* wfile = new TFile(Output, "recreate");
  TTree* wtree = new TTree("tree","tree");

  wtree->Branch("Ntrack",&Ntrack,"Ntrack/I");
  wtree->Branch("Ningtrack",&Ningtrack,"Ningtrack/I");
  wtree->Branch("muang",&muang,"muang/F");
  wtree->Branch("pang",&pang,"pang/F");
  wtree->Branch("ppe",&ppe,"ppe/F");
  wtree->Branch("mupe",&mupe,"mupe/F");
  wtree->Branch("opening",&opening,"opening/F");
  wtree->Branch("coplanarity",&coplanarity,"coplanarity/F");
  wtree->Branch("norm",&norm,"norm/F");
  wtree->Branch("totcrsne",&totcrsne,"totcrsne/F");
  wtree->Branch("inttype",&inttype,"inttype/I");
  wtree->Branch("nuE",&nuE,"nuE/F");
  wtree->Branch("mufc",&mufc,"mufc/I");
  wtree->Branch("pfc",&pfc,"pfc/I");
  wtree->Branch("mupdg",&mupdg,"mupdg/I");
  wtree->Branch("ppdg",&ppdg,"ppdg/I");
  wtree->Branch("veract",&veract,"veract/F");
  wtree->Branch("mumucl",&mumucl,"mumucl/F");
  wtree->Branch("pmucl",&pmucl,"pmucl/F");

  wtree->Branch("muang_t",&muang_t,"muang_t/F");
  wtree->Branch("pang_t",&pang_t,"pang_t/F");
  wtree->Branch("xnu",&xnu,"xnu/F");
  wtree->Branch("ynu",&ynu,"ynu/F");
  wtree->Branch("znu",&znu,"znu/F");
  wtree->Branch("exptime",&exptime,"exptime/F");
  wtree->Branch("startxch",&startxch,"startxch/F");
  wtree->Branch("startych",&startych,"startych/F");
  wtree->Branch("startxpln",&startxpln,"startxpln/I");
  wtree->Branch("startypln",&startypln,"startypln/I");
  wtree->Branch("range",&range,"range/F");
  wtree->Branch("pene",&pene,"pene/I");
  wtree->Branch("prange",&range,"prange/F");
  wtree->Branch("ppene",&pene,"ppene/I");

  if(usekin){
    wtree->Branch("true_muang",&true_muang,"true_muang/F");
    wtree->Branch("true_pang",&true_pang,"true_pang/F");
    wtree->Branch("true_mumom",&true_mumom,"true_mumom/F");
    wtree->Branch("true_pmom",&true_pmom,"true_pmom/F");
  }

  if(uset2krwt){
    wtree->Branch("reweight",&reweight,Form("reweight[%d]/F",NWEIGHTS));
  }


  //PMReconSummary* recon;
  PMAnaSummary* pmana;
  IngridSimVertexSummary* simver;

  for(int ievt=0; ievt<nevt; ievt++){

    if(ievt%100==0)cout << "analyze event# " << ievt<<endl;

    if(usekin){
      kintree -> GetEntry(ievt);
      //if(mode!=1||numnu!=4)continue;
      //true_muang = 180/3.141592*acos(dirnu[2][2]/sqrt(dirnu[2][0]*dirnu[2][0]+dirnu[2][1]*dirnu[2][1]+dirnu[2][2]*dirnu[2][2]));
      //true_pang = 180/3.141592*acos(dirnu[3][2]/sqrt(dirnu[3][0]*dirnu[3][0]+dirnu[3][1]*dirnu[3][1]+dirnu[3][2]*dirnu[3][2]));

      //true_mumom = pnu[2];
      //true_pmom  = pnu[3];

      true_muang = 0;
      true_pang  = 0;
      true_mumom = 0;
      true_pmom  = 0;
      if(mode>30)continue;
      for(int n=0;n<numnu;n++){
	if(ipnu[n]==13){
	  true_muang = 180/3.141592*acos(dirnu[n][2]/sqrt(dirnu[n][0]*dirnu[n][0]+dirnu[n][1]*dirnu[n][1]+dirnu[n][2]*dirnu[n][2]));
	}
      }

    }

    if(uset2krwt){
      weightstree -> GetEntry(ievt);
      for(int nw=0;nw<NWEIGHTS;nw++){
        reweight[nw]=weights->At(nw);
      }
    }

    if(interaction){
      evt      -> Clear();
      tree     -> GetEntry(ievt);

      if(evt->NIngridSimVertexes()>0){
	simver = (IngridSimVertexSummary*)(evt -> GetSimVertex(0) );
	norm     = simver -> norm;
	totcrsne = simver -> totcrsne;
	inttype  = simver -> inttype;
	nuE      = simver -> nuE;

	xnu      = simver -> xnu;
	ynu      = simver -> ynu;
	znu      = simver -> znu;

      }
      else{
	norm=1;
	totcrsne=1;
	inttype=0;
	nuE   =0;
	xnu=0;
	ynu=0;
	znu=0;
      }


      wtree -> Fill();
    }
    else{

    evt      -> Clear();
    tree     -> GetEntry(ievt);
    
    int npmana = evt -> NPMAnas();
    for(int i=0; i<npmana; i++){
      pmana   = (PMAnaSummary*) (evt -> GetPMAna(i) );
      if((pmana->vetowtracking)||(pmana->edgewtracking))continue;
      if(evt->NIngridBeamSummarys()>0&&!pmana->ontime)continue;

      veract    = pmana -> clstimecorr;//temporary
      Ntrack    = pmana -> Ntrack;
      Ningtrack = pmana -> Ningtrack;


      muang_t   = 180/3.14159265*atan(sqrt(pow(tan((pmana->thetax[0]+3.65)*3.14159265/180),2)+pow(tan(pmana->thetay[0]*3.14159265/180),2)));
      muang     = pmana -> angle[0];


      mupe      = pmana -> trkpe[0];
      mufc      = pmana -> ing_stop[0];      
      mupdg     = pmana -> pdg[0];
      mumucl    = pmana -> mucl[0];
      
      range = 2 * 1.032 / 7.86 /1.43*1.77 * pmana->sci_range[0]
	+ (6.5 + 2 * 1.032 / 7.86 /1.43*1.77) * pmana->iron_range[0];
      if(pmana->iron_pene[0]>9)
	pene = 9;
      else
	pene = pmana->iron_pene[0];

      startxpln    = pmana -> startxpln[0];
      startypln    = pmana -> startypln[0];
      startxch     = pmana -> startxch[0];
      startych     = pmana -> startych[0];
      exptime      = pmana -> exptime;
      

      if(Ntrack>1){
	pang_t  = 180/3.14159265*atan(sqrt(pow(tan((pmana->thetax[1]+3.65)*3.14159265/180),2)+pow(tan(pmana->thetay[1]*3.14159265/180),2)));
	pang    = pmana -> angle[1];

	ppe     = pmana -> trkpe[1];
	pfc     = pmana -> pm_stop[1];
	ppdg    = pmana -> pdg[1];
	pmucl   = pmana -> mucl[1];

	prange = 2 * 1.032 / 7.86 /1.43*1.77 * pmana->sci_range[1]
	  + (6.5 + 2 * 1.032 / 7.86 /1.43*1.77) * pmana->iron_range[1];
	if(pmana->iron_pene[0]>9)
	  ppene = 9;
	else
	  ppene = pmana->iron_pene[0];

      }
      else{
	pang_t   = 180/3.14159265*atan(sqrt(pow(tan((pmana->thetax[0]+3.65)*3.14159265/180),2)+pow(tan(pmana->thetay[0]*3.14159265/180),2)));
	pang     = pmana -> angle[0];
	ppe     = pmana -> trkpe[0];
	pfc     = 0;
	ppdg    = 0;
	pmucl  = 0;

	prange = 0;
	ppene = 0;
      }

      mu[1]=tan(pmana->thetay[0]/180*3.141592);
      mu[2]=tan(pmana->thetax[0]/180*3.141592);
      mu[3]=1;
      if(fabs(pmana->thetax[0])>90||fabs(pmana->thetay[0])>90){
	mu[1]=-mu[1];
	mu[2]=-mu[2];
	mu[3]=-mu[3];
      }
      mu[0]=sqrt(mu[1]*mu[1]+mu[2]*mu[2]+mu[3]*mu[3]);

      p[1]=tan(pmana->thetay[1]/180*3.141592);
      p[2]=tan(pmana->thetax[1]/180*3.141592);
      p[3]=1;
      if(fabs(pmana->thetax[1])>90||fabs(pmana->thetay[1])>90){
	p[1]=-p[1];
	p[2]=-p[2];
	p[3]=-p[3];
      }
      p[0]=sqrt(p[1]*p[1]+p[2]*p[2]+p[3]*p[3]);

      for(int j=0;j<3;j++){
	x=1+j%3;y=1+(j+1)%3;z=1+(j+2)%3;
	nup[z]=nu[x]*p[y]-nu[y]*p[x];
	numu[z]=nu[x]*mu[y]-nu[y]*mu[x];
      }
      numu[0]=sqrt(numu[1]*numu[1]+numu[2]*numu[2]+numu[3]*numu[3]);
      nup[0]=sqrt(nup[1]*nup[1]+nup[2]*nup[2]+nup[3]*nup[3]);

      opening     = 180/3.141692*acos((p[1]*mu[1]+p[2]*mu[2]+p[3]*mu[3])/p[0]/mu[0]);
      coplanarity = 180/3.141692*acos((nup[1]*numu[1]+nup[2]*numu[2]+nup[3]*numu[3])/nup[0]/numu[0]);

      if(evt->NIngridSimVertexes()>0){
	simver = (IngridSimVertexSummary*)(evt -> GetSimVertex(0) );
	norm     = simver -> norm;
	totcrsne = simver -> totcrsne;
	inttype  = simver -> inttype;
	nuE      = simver -> nuE;

	xnu      = simver -> xnu;
	ynu      = simver -> ynu;
	znu      = simver -> znu;

      }
      else{
	norm=1;
	totcrsne=1;
	inttype=0;
	nuE   =0;
	xnu=0;
	ynu=0;
	znu=0;
      }

      wtree -> Fill();
    }
    }

  }//Event Loop
  
  //######## Write and Close ####################
  wtree  -> Write();
  wfile  -> Write();
  wfile  -> Close();
  
}
