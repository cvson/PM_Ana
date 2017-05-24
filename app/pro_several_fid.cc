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
#include <TLegend.h>
//##### INGRID Library #########
#include "INGRIDEVENTSUMMARY.h"
#include "IngridHitSummary.h"
#include "IngridSimHitSummary.h"
#include "IngridBasicReconSummary.h"
#include "root_setup.hxx"




int    fid;
int    verxymin;
int    verxymax;
int    verzmin;
int    verzmax;
int    type;
string fidname;


int    mod;
void plot_vertex    (string str, int Fid, int Mod, int Type);
void plot_vertex_ref(string str, int Fid, int Mod, int Type);
void getfid(int Fid, int Mod,int Type);
TH1F*    fHh;

TH1F*    fHhref;
TH1F*    fHref;
TH1F*    fHdiff;
TFile*    f1;
TFile*    f2;
TTree* tree1;
TTree* tree2;

TCanvas* c1;


int main(int argc, char *argv[]){
  TROOT        root ("GUI","GUI");
  TApplication app  ("App",0,0);
  root_setup* froot_setup = new root_setup();
  Int_t c=-1;  char FileName1[300]; char FileName2[300];
  int fid, type;
  while ((c = getopt(argc, argv, "f:g:i:t:")) != -1) {
    switch(c){
    case 'f':
      sprintf(FileName1,"%s",optarg);
      break;
    case 'g':
      sprintf(FileName2,"%s",optarg);
      break;
    case 'i':
      fid = atoi(optarg);
      break;
    case 't':
      type = atoi(optarg);
      break;
    }
  }
  c1     = new TCanvas("c1","c1",10,10, 900, 600);

  f1     = new TFile  (FileName1, "read");
  tree1  = (TTree*)(f1->Get("tree"));
  f2     = new TFile  (FileName2, "read");
  tree2  = (TTree*)(f2->Get("tree"));

  for(int imod=0; imod<14; imod++){
    plot_vertex(FileName1, fid, imod, type);
    plot_vertex_ref(FileName2, fid, imod, type);
    c1 -> Update();
   
    if(imod==0)
      c1->Print(Form("~/temppdf/100815_Fid_%d_%d.ps(",fid,type));
    else if(imod==13)
      c1->Print(Form("~/temppdf/100815_Fid_%d_%d.ps)",fid,type));
    else
      c1->Print(Form("~/temppdf/100815_Fid_%d_%d.ps",fid,type));
    //cout << "module# = " <<imod << endl;
    //cin.get();
  }

}




void getfid(int Fid, int Mod,int Type){
  fid = Fid;
  mod = Mod;

  type     = Type;

  switch(fid){
  case 0:
    fidname  = "nominal";
    verxymax = 21;
    verxymin = 2;
    verzmax  = 1;
    verzmax  = 8;
    break;
  case 1:
    fidname  = "internal_1";
    verxymax = 20;
    verxymin = 3;
    verzmax  = 1;
    verzmax  = 8;
    break;
  case 2:
    fidname  = "internal_2";
    verxymax = 19;
    verxymin = 4;
    verzmax  = 1;
    verzmax  = 8;
    break;
  case 3:
    fidname  = "upstream";
    verxymax = 21;
    verxymin = 2;
    verzmax  = 1;
    verzmax  = 3;
    break;
  case 4:
    fidname  = "middle";
    verxymax = 21;
    verxymin = 2;
    verzmax  = 4;
    verzmax  = 5;
    break;
  case 5:
    fidname  = "downstream";
    verxymax = 21;
    verxymin = 2;
    verzmax  = 6;
    verzmax  = 8;
    break;

  }

}

//______________________________________________________________________
//______________________________________________________________________


void plot_vertex(string str, int Fid, int Mod, int Type){
  getfid(Fid, Mod, Type);
  //void plot_vertex(string str){
  int xbin, xmax, xmin;
  string fHname, fHdraw;
  switch(type){
  case 0:
    xbin = 11; xmin = 0; xmax = 11;
    fHname = "# of penetrated irons";
    fHdraw = "max(endxpln,endypln)-min(vertexxz, vertexyz)";
    break;
  case 1:
    xbin = 18; xmin = 0; xmax = 92;
    fHname = "angle";
    fHdraw = "angle";
    break;
  }


  gStyle          -> SetOptStat(0000);


  fHh    = new TH1F   ("fHh",
		       Form("%s(mod.%d)",fHname.c_str(),Mod),
				xbin, xmin, xmax);
  fHh -> SetLineWidth(3);



  char   nu[300],fidcut[300],cut[300], draw[300];
  

  string fidname;



  sprintf(nu,    "hastrk&&matchtrk&&ontime&&!vetowtracking&&!edgewtracking");
  sprintf(fidcut, "%d<=vertexx&&vertexx<=%d&&%d<=vertexy&&vertexy<=%d&&%d<=vertexxz&&vertexxz<=%d&&%d<=vertexyz&&vertexyz<=%d", verxymin, verxymax, verxymin, verxymax, verzmin, verzmax, verzmin, verzmax );
  sprintf(cut,    "%s&&%s&&hitmod==%d",  nu, fidcut, mod);
  sprintf(draw,   "%s>>fHh", fHdraw.c_str());
  tree1     -> Draw(draw, cut);
  cout << fHh -> GetEntries()<<endl;
  cout << "DATA draw" << endl;
}

//______________________________________________________________________
//______________________________________________________________________

void plot_vertex_ref(string str, int Fid, int Mod, int Type){
  //void plot_vertex_ref(string str){
  getfid(Fid, Mod, Type);
  int xbin, xmax, xmin;
  string fHname, fHdraw;
  switch(type){
  case 0:
    xbin = 11; xmin = 0; xmax = 11;
    fHname = "# of penetrated irons";
    fHdraw = "max(endxpln,endypln)-min(vertexxz, vertexyz)";
    break;
  case 1:
    xbin = 18; xmin = 0; xmax = 92;
    fHname = "angle";
    fHdraw = "angle";
    break;
  }


  fHref  = new TH1F   ("fHref",
				"fHref",
				xbin, xmin, xmax);
  fHdiff = new TH1F   ("fHdiff",
				"DATA/MC",
				xbin, xmin, xmax);
  fHhref  = new TH1F   ("fHhref",
				 "fHhref",
				 xbin, xmin, xmax);

  fHhref -> SetLineWidth(2);
  fHhref -> SetLineColor(2);
  fHhref -> SetLineStyle(2);


  char   nu[300],fidcut[300],cut[300], draw[300];
  string fidname;



  sprintf(nu,    "hastrk&&matchtrk&&!vetowtracking&&!edgewtracking");
  sprintf(fidcut, "%d<=vertexx&&vertexx<=%d&&%d<=vertexy&&vertexy<=%d&&%d<=vertexxz&&vertexxz<=%d&&%d<=vertexyz&&vertexyz<=%d", verxymin, verxymax, verxymin, verxymax, verzmin, verzmax, verzmin, verzmax );
  //sprintf(cut,    "(%s&&%s&&hitmod==%d)*%lf",  nu, fidcut, mod, pot);//8.45e19);
  sprintf(cut,    "((%s&&%s&&hitmod==%d))*norm*totcrsne*weight",  nu, fidcut, mod);//8.45e19);
  sprintf(draw,   "%s>>fHref", fHdraw.c_str());
  tree2     -> Draw(draw, cut, "same");
  float norm = 1.0*fHh->GetEntries()/fHref->GetEntries();
  cout << "norm:" << norm << endl;
  fHhref -> Add ( fHref, norm );

  fHh    -> SetXTitle(fHname.c_str());
  fHh    -> SetYTitle("# of events");

  c1     -> Clear();
  c1     -> Divide(2,1);
  c1     -> cd(1);
  TLegend* leg = new TLegend(0.75,0.75,0.95,0.95);
  leg    -> AddEntry(fHh,"DATA");
  leg    -> AddEntry(fHhref,"MC");
  leg    -> SetFillColor(0);
  //fHh    -> Draw();
  //fHhref -> Draw("same");
  fHhref -> Draw();
  leg    -> Draw();

  fHh    -> Sumw2();
  fHhref -> Sumw2();
  fHdiff -> Add(fHh);
  fHdiff -> Add(fHhref,-1);
  fHdiff -> Divide(fHhref);

  c1     -> cd(2);
  fHdiff -> SetMinimum(-0.3);
  fHdiff -> SetMaximum(0.3);
  fHdiff -> SetLineWidth(3);
  fHdiff -> SetYTitle("(DATA-MC)/MC");
  fHdiff -> SetXTitle(fHname.c_str());
  fHdiff -> Draw();

  cout << fHref -> GetEntries()<<endl;
  cout << "MC draw" << endl;

  float sumw=0;
  for(int ibin=1; ibin<=xbin; ibin++){
    sumw += pow(fHdiff->GetBinContent(ibin),2);
  }

}




void summary(int xbin){
  cout << "--- number of events ----" << endl;
  cout << fHh -> GetEntries() << "\t" << fHhref -> GetEntries() << endl;
  cout << "--- mean ----" << endl;
  float meanh    = fHh   -> GetMean   ();
  float meanref  = fHhref-> GetMean   ();
  float rmsh     = fHh   -> GetRMS    ();
  float rmsref   = fHhref-> GetRMS    ();
  float errorh   = rmsh   / fHh  ->GetEntries();
  float errorref = rmsref / fHhref->GetEntries();
  cout << meanh   << " +- " << errorh   << "\t"
       << meanref << " +- " << errorref << "\t" << endl;

  float chi2=0;
  cout << "------- calculating -------- " << endl;
  for(int i=0; i<xbin; i++){
    if(fHh->GetBinContent(i+1)<1)
      continue;
    float error = fHh    -> GetBinError  (i+1);
    float yh    = fHh    -> GetBinContent(i+1);
    float yref  = fHhref -> GetBinContent(i+1);
    float a     = (yh-yref)*(yh-yref)/(error*error);
    cout << error << "\t"
	 << yh    << "\t"
	 << yref  << "\t"
	 << a     << "\t"
	 << endl;
    chi2 = chi2+a;
  }
  cout << "reduced chi2 = " << chi2 << endl;
}
