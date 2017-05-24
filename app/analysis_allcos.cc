#include<iostream>
#include<vector>
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
#include <TString.h>
#include <TSystem.h>

#include "TApplication.h"
#include "setup.hxx"
//#include "root_setup.hxx"
Int_t             fMod;
Long_t            fTime;
Int_t            Cycle;
vector<int>*       adc;
vector<int>*      view;
vector<int>*       pln;
vector<int>*        ch;
vector<double>*     pe;
vector<int>*      nsec;
 

vector<double>  anape;
vector<int>   anaview;
vector<int>    anapln;
vector<int>     anach;


int main(int argc,char *argv[]){

  char buff1[200],buff2[200];
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  Int_t run_number;
  Int_t c=-1;  char FileName[300];
  bool cosmicflag=false;
  while ((c = getopt(argc, argv, "r:h")) != -1) {
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

  //sprintf(buff1,"/home/daq/data/test/ingrid.root");
  sprintf(buff1,"/home/daq/data/root_new/ingrid_%08d_0128.root",run_number);
  FileStat_t fs;
  if(gSystem->GetPathInfo(buff1,fs)){
    cout<<"Cannot open file "<<buff1<<endl;
    exit(1);
  }

  TFile*               f  = new TFile(buff1,"read");
  TTree*            tree  = (TTree*)f->Get("tree");
  Int_t        NEvt       = tree->GetEntries();
  cout<<"---- # of Evt    = "<<NEvt<<"---------"<<endl;
  cin.get();
  Int_t           NumEvt;
  pe   = 0;
  nsec = 0;
  view = 0;
  pln  = 0;
  ch   = 0;
  float            ax ;
  float            bx ;
  float            ay ;
  float            by ;

  tree->SetBranchBranch(     "ax",       &ax,    "ax/F");
  tree->SetBranchBranch(     "ay",       &ay,    "ay/F");
  tree->SetBranchBranch(     "bx",       &bx,    "bx/F");
  tree->SetBranchBranch(     "by",       &by,    "by/F");
  tree->SetBranchBranch(   "fMod",     &fMod,  "fMod/I");
  tree->SetBranchBranch(  "anape",    &anape);
  tree->SetBranchBranch(   "view",     &view);
  tree->SetBranchBranch(    "pln",      &pln);
  tree->SetBranchBranch(     "ch",       &ch);
  
  

  for(int n=0; n<NEvt; n++){
    //if(n>1000000)break;
    tree->GetEntry(n);
    if(Cycle==17){
      if(n%1000==0)cout<<"-- analysis Evt # = "<<n<<" ---"<<endl;
      //cout<<"-- analysis Evt # = "<<n<<" ---"<<endl;
      Int_t nHit = pe->size();
 
      //######### fill hit class ####################
      allhit.clear();
      for(Int_t i=0;i<nHit;i++){
	Hit hit  ;
	hit.pe   = pe  ->at(i);
	hit.time = nsec->at(i);
	hit.view = view->at(i);
	hit.pln  = pln ->at(i);
	hit.ch   = ch  ->at(i);
	allhit.push_back(hit);
      }
      fSortTime(allhit);

      vector<Hit> hitclster;
      hitclster.clear();

      //########## time clustering ###################
      while(fFindTimeClster(allhit, hitclster, fTime)){
	fSortTime(hitclster);
	Int_t nCls = hitclster.size();

	//######### check active plane ################
	//######### for easy cut       ################
	Int_t   trgbit  = 0;
	Int_t   nActPln = 0;
	for(Int_t i=0;i<nCls;i++){
	  for(Int_t j=i+1;j<nCls;j++){
	    if(hitclster[i].pln==hitclster[j].pln){
	      if(hitclster[i].view!=hitclster[j].view){
		trgbit=trgbit|((0x400)>>(10-hitclster[i].pln));
	      }//view difference
	    }//pln coince
	  }//j
	}//i
	for(Int_t i=1;i<=1024;i=i<<1){
	  if(trgbit&i){
	    nActPln++;
	  }
	}


	//########### tracking after easy cut #########
	if(nActPln>=3){ // easy cut
	  anape  .clear();
	  anaview.clear();
	  anapln .clear();
	  anach  .clear();


	  Bool_t draw_flag = false; //for debug

	  fSortPln(hitclster);
	  if(LinFit(hitclster, &ax, &bx, &ay, &by)){
	    //if(fabs(TMath::ATan(1./ax))<0.3426)draw_flag=true;
	    for(Int_t i=0; i<nCls; i++){
	      Int_t expch;
	      if(hitclster[i].view==0){//X
		if(fINGRID_Dimension->get_expchXY(fMod, 
						  hitclster[i].view,
						  hitclster[i].pln ,
						  &expch           ,
						  ax               ,
						  bx               
						  )
		   )
		  {
		    if(abs(expch-hitclster[i].ch)<=0){
		      anape.push_back(hitclster[i].pe);
		      anaview.push_back(hitclster[i].view);
		      anapln.push_back(hitclster[i].pln);
		      anach.push_back(hitclster[i].ch);
		    }
		  }

	      }//X
	      if(hitclster[i].view==1){//Y
		if(fINGRID_Dimension->get_expchXY(fMod, 
						  hitclster[i].view,
						  hitclster[i].pln ,
						  &expch           ,
						  ay               ,
						  by               
						  )
		   )
		  {
		    if(abs(expch-hitclster[i].ch)<=0){
		      anape.push_back(hitclster[i].pe);
		      anaview.push_back(hitclster[i].view);
		      anapln.push_back(hitclster[i].pln);
		      anach.push_back(hitclster[i].ch);
		    }
		  }
	      }//Y
	    }

	    fHEvtRate -> Fill(fMod);
	    rtree     -> Fill();


	    if(draw_flag){
	      cout<<"X slope:"     <<TMath::ATan(1./ax)
		  <<"\tX  b   :"   <<bx
		  <<"\tY slope   :"<<ay
		  <<"\tY  b   :"   <<by<<endl;
	      Draw_Module();
	      Draw_Hit(hitclster);
	      Print(hitclster);
	      LinFit(hitclster, &ax, &bx, &ay, &by);
	      cin.get();
	    }

	  }//Tracking end
	  //HoughTrans(hitclster);
	  //cin.get();
	}
	hitclster.clear();
      }//clustering
    }//Cycle==17
  }//Event loop

  //######## Write and Close ####################


  rtree   -> Write();
  rf      -> Write();
  rf      -> Close();
  f       -> Close();

}
 
