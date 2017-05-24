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
#include "PMReconSummary.h"
#include "PMAnaSummary.h"
#include "NeutInfoSummary.h"



int main(int argc,char *argv[]){
    
    TROOT root("GUI","GUI");
    TApplication theApp("App",0,0);
    
    
    Int_t c=-1;  char FileName[300];
    char Output[300];
    while ((c = getopt(argc, argv, "f:o:")) != -1) {
        switch(c){
            case 'f':
                sprintf(FileName, "%s",optarg);
                break;
            case 'o':
                sprintf(Output, "%s",optarg);
                break;
        }
    }
    
    //######## read root file #############################
    //#####################################################
    FileStat_t fs;
    cout<<"reading "<<FileName<<"....."<<endl;
    if(gSystem->GetPathInfo(FileName,fs)){
        cout<<"Cannot open file "<<FileName<<endl;
        exit(1);
    }
    IngridEventSummary* evt = new IngridEventSummary();
    TFile*            rfile = new TFile(FileName,"read");
    TTree*             tree = (TTree*)rfile -> Get("tree");
    TBranch*          EvtBr = tree->GetBranch("fDefaultReco.");
    EvtBr                   ->SetAddress(&evt);
    tree                    ->SetBranchAddress("fDefaultReco.", &evt);
    int                nevt = (int)tree -> GetEntries();
    cout << "Total # of events = " << nevt <<endl;
    if( nevt==0 )
        exit(1);
    
    
    float norm,totcrsne,nuE;
    float muang_true, muang_rec, muang_diff;
    float pang_true, pang_rec, pang_diff;
    float muang_t[3], muang_r[3];
    float pang_t[3], pang_r[3];
    int inttype;
    int length;
    TFile* wfile = new TFile(Output, "recreate");
    TTree* wtree = new TTree("tree","tree");
    wtree->Branch("muang_true",&muang_true,"muang_true/F");
    wtree->Branch("muang_rec",&muang_rec,"muang_rec/F");
    wtree->Branch("muang_diff",&muang_diff,"muang_diff/F");
    
    wtree->Branch("pang_true",&pang_true,"pang_true/F");
    wtree->Branch("pang_rec",&pang_rec,"pang_rec/F");
    wtree->Branch("pang_diff",&pang_diff,"pang_diff/F");
    
    wtree->Branch("norm",&norm,"norm/F");
    wtree->Branch("totcrsne",&totcrsne,"totcrsne/F");
    wtree->Branch("inttype",&inttype,"inttype/I");
    wtree->Branch("nuE",&nuE,"nuE/F");
    
    wtree->Branch("length",&length,"length/I");
    
    IngridSimParticleSummary* simpar;
    IngridSimVertexSummary*  simver;
    
    for( int ievt = 0; ievt < nevt; ievt++  ){
        if(ievt%100==0)cout << "analyze event# " << ievt<<endl;
        evt      -> Clear();
        tree -> GetEntry(ievt);
        
        
        simver  = (IngridSimVertexSummary*)( evt -> GetSimVertex(0) );
        norm=simver->norm;
        totcrsne=simver->totcrsne;
        nuE=simver->nuE;
        inttype=simver->inttype;
        
        
        float pmax=0;
        float mumax=0;
        pang_true=-1e5;
        muang_true=-1e5;
        muang_t[0]=0;
        muang_t[1]=0;
        muang_t[2]=-1;
        pang_t[0]=0;
        pang_t[1]=0;
        pang_t[2]=-1;
        int nsimpar = evt -> NIngridSimParticles();
        for( int ipar = 0; ipar < nsimpar; ipar++  ){
            
            simpar = (IngridSimParticleSummary*)(evt->GetSimParticle(ipar));
            
            if(simpar -> pdg == 13 || simpar -> pdg == -13){
                if(fabs(simpar -> momentum[2]) > mumax){
                    mumax = fabs(simpar -> momentum[2]);
                    muang_true = acos(fabs(simpar -> momentum[2])/sqrt(simpar -> momentum[1]*simpar -> momentum[1]+simpar -> momentum[2]*simpar -> momentum[2]+simpar -> momentum[0]*simpar -> momentum[0]))*180/3.14159265;
                    muang_t[0]=simpar ->momentum[0];
                    muang_t[1]=simpar ->momentum[1];
                    muang_t[2]=simpar ->momentum[2];
                }
            }
            
            if(simpar -> pdg == 2212){
                if(fabs(simpar -> momentum[2]) > pmax){
                    pmax = fabs(simpar -> momentum[2]);
                    pang_true = acos(fabs(simpar -> momentum[2])/sqrt(simpar -> momentum[1]*simpar -> momentum[1]+simpar -> momentum[2]*simpar -> momentum[2]+simpar -> momentum[0]*simpar -> momentum[0]))*180/3.14159265;
                    pang_t[0]=simpar ->momentum[0];
                    pang_t[1]=simpar ->momentum[1];
                    pang_t[2]=simpar ->momentum[2];
                }
            }
            
        }
        
        
        PMAnaSummary*  pmana;
        int npm = evt -> NPMAnas();
        muang_rec=-1e5;
        pang_rec=-1e5;
        muang_r[0]=0;
        muang_r[1]=0;
        muang_r[2]=-1;
        pang_r[0]=0;
        pang_r[1]=0;
        pang_r[2]=-1;
        if(npm>0){
            pmana  = (PMAnaSummary*)( evt -> GetPMAna(0) );
            
            if(pmana->vetowtracking)continue;
            if(pmana->edgewtracking)continue;
            
            if(pmana->Ntrack>0){
                muang_rec = pmana -> angle[0];
                muang_r[0]= tan(pmana -> thetay[0]*3.14169265/180.);
                muang_r[1]= tan(pmana -> thetax[0]*3.14169265/180.);
                muang_r[2]= 1;
            }
            if(pmana->Ntrack==2&&pmana->Ningtrack==1){
                pang_rec = pmana -> angle[1];
                pang_r[0]= tan(pmana -> thetay[1]*3.14169265/180.);
                pang_r[1]= tan(pmana -> thetax[1]*3.14169265/180.);
                pang_r[2]= 1;
                
                if(abs(pmana ->endxpln[1] - pmana ->startxpln[1])>abs(pmana ->endypln[1] - pmana ->startypln[1]))
                    length = pmana ->endypln[1] - pmana ->startypln[1];
                else
                    length = pmana ->endxpln[1] - pmana ->startxpln[1];
                
                if(pmana -> angle[1]>90){
                    pang_r[0]=(-1)*pang_r[0];
                    pang_r[1]=(-1)*pang_r[1];
                    pang_r[2]=(-1)*pang_r[2];
                }
            }
        }
        
        
        pang_diff=-1e5;
        muang_diff=-1e5;
        if(muang_t[2]!=-1&&muang_r[2]!=-1){
            muang_diff=
            acos(
                 (
                  muang_t[0]*muang_r[0]+
                  muang_t[1]*muang_r[1]+
                  muang_t[2]*muang_r[2]
                  )
                 /sqrt(muang_t[0]*muang_t[0]+muang_t[1]*muang_t[1]+muang_t[2]*muang_t[2])
                 /sqrt(muang_r[0]*muang_r[0]+muang_r[1]*muang_r[1]+muang_r[2]*muang_r[2])
                 )*180/3.141692;
        }
        
        if(pang_t[2]!=-1&&pang_r[2]!=-1){
            pang_diff=
            acos(
                 (
                  pang_t[0]*pang_r[0]+
                  pang_t[1]*pang_r[1]+
                  pang_t[2]*pang_r[2]
                  )
                 /sqrt(pang_t[0]*pang_t[0]+pang_t[1]*pang_t[1]+pang_t[2]*pang_t[2])
                 /sqrt(pang_r[0]*pang_r[0]+pang_r[1]*pang_r[1]+pang_r[2]*pang_r[2])
                 )*180/3.141692;
        }
        
        wtree    -> Fill();
    }//Event Loop
    
    
    //######## Write and Close ####################
    wtree  -> Write();
    wfile  -> Write();
    wfile  -> Close();
    rfile  -> Close();
}
