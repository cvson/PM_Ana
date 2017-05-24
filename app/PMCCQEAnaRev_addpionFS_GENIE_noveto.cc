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
    char buff0[200];//gean4 input
    TROOT root("GUI","GUI");
    TApplication theApp("App",0,0);
    Int_t c=-1;
    char Output[300];
    char WeightFile[300];
    //int ithProcess;
    bool uset2krwt=false;
    while ((c = getopt(argc, argv, "o:f:i:w:")) != -1) {
        switch(c){
            case 'o':
                sprintf(Output,"%s",optarg);
                break;
            case 'f':
                sprintf(buff1,"%s",optarg);
                break;
                /*case 'i':
                 ithProcess = atoi(optarg);
                 break;*/
            case 'i':
                sprintf(buff0,"%s",optarg);
                break;
            case 'w':
                sprintf(WeightFile,"%s",optarg);
                uset2krwt=true;
                break;
        }
    }
    FileStat_t fs;
    
    //######## read RECO root file #############################
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
    
    //######## read GEANT4 root file #############################
    //#####################################################
    cout<<"reading "<<buff0<<"....."<<endl;
    if(gSystem->GetPathInfo(buff0,fs)){
        cout<<"Cannot open file "<<buff0<<endl;
        exit(1);
    }
    cout << buff0 << endl;
    
    IngridEventSummary* mcevt = new IngridEventSummary();
    TFile*            mcfile = new TFile(buff0,
                                         "read");
    TTree*             mctree = (TTree*)mcfile -> Get("tree");
    TBranch*          EvtBrMC = mctree->GetBranch("fDefaultReco.");
    EvtBrMC                   ->SetAddress(&mcevt);
    mctree                    ->SetBranchAddress("fDefaultReco.", &mcevt);
    int                nmcevt = (int)mctree -> GetEntries();
    cout << "Total # of events = " << nmcevt <<endl;
    
    if( nmcevt==0 )
        exit(1);
    
    if (nmcevt!=nevt) {
        cout<<"No matched event number"<<endl;
        exit(1);
    }
    ////THis is for NEUT, but not for GENIE get the h1 tree for kinematics
    int               mode;
    float             Pvc[100][3];
    
    /* TTree*            h1tree = (TTree*)mcfile -> Get("h1");
     int               mode;
     float             Pvc[100][3];
     
     h1tree  -> SetBranchAddress("mode", &mode);
     h1tree  -> SetBranchAddress("Pvc",  &Pvc);
     
     int               nh1evt = (int)h1tree -> GetEntries();
     if (nh1evt!=nevt) {
     cout<<"No matched event number for h1 tree"<<endl;
     exit(1);
     }*/
    
    float Pnu[3],Enu;
    float Pmu[4],Emu,Mmu;
    
    //########## T2KReWeight file ###########
    //#######################################
    TFile*            rwfile;
    TTree*      weightstree;
    int               nevt2;
    int            nweights;
    TArrayF        *weights=0;
    if(uset2krwt){
        rwfile = new TFile(WeightFile,"read");
        weightstree = (TTree*)rwfile -> Get("weightstree");
        weightstree            -> SetBranchAddress("nweights", &nweights);
        weightstree            -> SetBranchAddress("weights", &weights);
        nevt2  = (int)weightstree -> GetEntries();
        if(nevt!=nevt2){
            cout<<"Number of events it does not match."<<endl;
            exit(1);
        }
    }
    
    float mu[4],p[4],nu[4],nup[4],numu[4];
    int x,y,z;
    
    
    //#### make rootfile after analysis #####
    //#######################################
    float norm,totcrsne,muang,pang,mupe,ppe,opening,coplanarity,nuE,veract,mumucl,pmucl,muang_t,pang_t;
    int Ntrack,Ningtrack,inttype,mufc,pfc,pingridfc,mupdg,ppdg;//include pingridfc
    float reweight[NWEIGHTS];
    float range,prange;//for INGRid only
    float muscirange, muironrange;//for scillator and iron plate in INGRID
    float pscirange, pironrange;//for scillator and iron plate in INGRID
    int pene,ppene;
    float xreco, yreco, zreco;
    float xnu,ynu,znu,exptime;
    // for 1st track
    float startxch,startych;
    int startxpln,startypln;
    float endxch,endych;
    int endxpln,endypln;
    //for 2nd track
    float pstartxch,pstartych;
    int pstartxpln,pstartypln;
    float pendxch,pendych;
    int pendxpln,pendypln;
    
    float tmomtrans;
    mu[3]=1;p[3]=1;nu[1]=0;nu[2]=-tan(3.65/180*3.141592);nu[3]=1;
    nu[0]=sqrt(nu[1]*nu[1]+nu[2]*nu[2]+nu[3]*nu[3]);
    
    
    //TFile* wfile = new TFile(TString(Output)+Form("_%d",ithProcess), "recreate");
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
    wtree->Branch("pingridfc",&pingridfc,"pingridfc/I");
    wtree->Branch("mupdg",&mupdg,"mupdg/I");
    wtree->Branch("ppdg",&ppdg,"ppdg/I");
    wtree->Branch("veract",&veract,"veract/F");
    wtree->Branch("mumucl",&mumucl,"mumucl/F");
    wtree->Branch("pmucl",&pmucl,"pmucl/F");
    
    wtree->Branch("muang_t",&muang_t,"muang_t/F");
    wtree->Branch("pang_t",&pang_t,"pang_t/F");
    wtree->Branch("xreco",&xreco,"xreco/F");
    wtree->Branch("yreco",&yreco,"yreco/F");
    wtree->Branch("zreco",&zreco,"zreco/F");
    
    wtree->Branch("xnu",&xnu,"xnu/F");
    wtree->Branch("ynu",&ynu,"ynu/F");
    wtree->Branch("znu",&znu,"znu/F");
    wtree->Branch("exptime",&exptime,"exptime/F");
    //for 1st track
    wtree->Branch("startxch",&startxch,"startxch/F");
    wtree->Branch("startych",&startych,"startych/F");
    wtree->Branch("startxpln",&startxpln,"startxpln/I");
    wtree->Branch("startypln",&startypln,"startypln/I");
    wtree->Branch("endxch",&endxch,"endxch/F");
    wtree->Branch("endych",&endych,"endych/F");
    wtree->Branch("endxpln",&endxpln,"endxpln/I");
    wtree->Branch("endypln",&endypln,"endypln/I");
    //for 2nd track
    wtree->Branch("pstartxch",&pstartxch,"pstartxch/F");
    wtree->Branch("pstartych",&pstartych,"pstartych/F");
    wtree->Branch("pstartxpln",&pstartxpln,"pstartxpln/I");
    wtree->Branch("pstartypln",&pstartypln,"pstartypln/I");
    wtree->Branch("pendxch",&pendxch,"pendxch/F");
    wtree->Branch("pendych",&pendych,"pendych/F");
    wtree->Branch("pendxpln",&pendxpln,"pendxpln/I");
    wtree->Branch("pendypln",&pendypln,"pendypln/I");
    
    //range
    wtree->Branch("range",&range,"range/F");
    wtree->Branch("muscirange",&muscirange,"muscirange/F");
    wtree->Branch("muironrange",&muironrange,"muironrange/F");
    wtree->Branch("pene",&pene,"pene/I");
    wtree->Branch("prange",&prange,"prange/F");
    wtree->Branch("pscirange",&pscirange,"pscirange/F");
    wtree->Branch("pironrange",&pironrange,"pironrange/F");
    wtree->Branch("ppene",&ppene,"ppene/I");
    wtree->Branch("tmomtrans",&tmomtrans,"tmomtrans/F");
    //newadd for FS
    Int_t npioncount;
    wtree->Branch("npioncount",&npioncount,"npioncount/I");
    
    Int_t nmuoncount;
    wtree->Branch("nmuoncount",&npioncount,"nmuoncount/I");
    
    Int_t nprotoncount;
    wtree->Branch("nprotoncount",&nprotoncount,"nprotoncount/I");
    
    //addnew for true info 20170224
    float muenergy_true, mumomentum_true;//GeV
    float penergy_true, pmomentum_true;//GeV
    float Qsq_true, xbjorken_true;
    float tmomtrans_true;
    float Erecoil_true;//E_v - E_mu
    float HadInvWsq;//hadronic invariant mass
    float muang_true, muang_diff;
    float pang_true, pang_diff;
    
    wtree->Branch("muenergy_true",&muenergy_true,"muenergy_true/F");
    wtree->Branch("mumomentum_true",&mumomentum_true,"mumomentum_true/F");
    wtree->Branch("penergy_true",&penergy_true,"penergy_true/F");
    wtree->Branch("pmomentum_true",&pmomentum_true,"pmomentum_true/F");
    wtree->Branch("Qsq_true",&Qsq_true,"Qsq_true/F");
    wtree->Branch("xbjorken_true",&xbjorken_true,"xbjorken_true/F");
    wtree->Branch("tmomtrans_true",&tmomtrans_true,"tmomtrans_true/F");
    
    wtree->Branch("Erecoil_true",&Erecoil_true,"Erecoil_true/F");
    wtree->Branch("HadInvWsq",&HadInvWsq,"HadInvWsq/F");
    
    wtree->Branch("muang_true",&muang_true,"muang_true/F");
    wtree->Branch("muang_diff",&muang_diff,"muang_diff/F");
    
    
    wtree->Branch("pang_true",&pang_true,"pang_true/F");
    wtree->Branch("pang_diff",&pang_diff,"pang_diff/F");
    //////////////////////end add true info
    if(uset2krwt){
        wtree->Branch("reweight",&reweight,Form("reweight[%d]/F",NWEIGHTS));
    }
    
    
    //PMReconSummary* recon;
    PMAnaSummary* pmana;
    IngridSimParticleSummary* simpar;
    IngridSimVertexSummary* simver;
    IngridSimVertexSummary* simvermc;
    
    
    int minevt = 0;
    int maxevt = nevt;
    int nevt2print = int((maxevt-minevt)/100.);
    float muang_tarray[4];//in GeV
    float pang_tarray[4];//in GeV
    float nue_cp;//newadd from mcevnt
    int inttype_cp;
    
    for(int ievt=minevt; ievt<maxevt; ievt++){
        
        if((ievt-minevt)%nevt2print==0)cout << "processing " << int((ievt-minevt)*100./float(maxevt-minevt))<<" % of events"<<endl;
        
        if(uset2krwt){
            weightstree -> GetEntry(ievt);
            for(int nw=0;nw<NWEIGHTS;nw++){
                reweight[nw]=weights->At(nw);
            }
        }
        ////////////////////////////////////////////////////////////////////////////////
        //Get true information
        mcevt   -> Clear();
        mctree  -> GetEntry(ievt);
        
        float pmax=0;
        float mumax=0;
        pang_true=-1e5;
        muang_true=-1e5;
        muang_tarray[0]=0;
        muang_tarray[1]=0;
        muang_tarray[2]=-1;
        muang_tarray[3]=0;
        
        pang_tarray[0]=0;
        pang_tarray[1]=0;
        pang_tarray[2]=-1;
        pang_tarray[3]=0;
        
        
        int nsimpar = mcevt -> NIngridSimParticles();
        Int_t npioncounttmp = 0;//reset 0
        Int_t nmuoncounttmp = 0;//reset 0
        Int_t nprotoncounttmp = 0;//reset 0
        
        //hack
        if(mcevt->NIngridSimVertexes()>0){
            simvermc = (IngridSimVertexSummary*)(mcevt -> GetSimVertex(0) );
            nue_cp      = simvermc -> nuE;
            inttype_cp  = simvermc -> inttype;
            
            norm     = simvermc -> norm;
            totcrsne = simvermc -> totcrsne;
            inttype  = simvermc -> inttype;
            nuE      = simvermc -> nuE;
            
            xnu      = simvermc -> xnu;
            ynu      = simvermc -> ynu;
            znu      = simvermc -> znu;
            
        }
        else{
            nue_cp   =0;
            inttype_cp =0;
            
            norm=1;
            totcrsne=1;
            inttype=0;
            nuE   =0;
            xnu=0;
            ynu=0;
            znu=0;
        }
        Pnu[0] = nue_cp*0;
        Pnu[1] = nue_cp*(-0.064);
        Pnu[2] = nue_cp*(0.998);
        mode = inttype_cp;
        
        for( int ipar = 0; ipar < nsimpar; ipar++  ){
            
            simpar = (IngridSimParticleSummary*)(mcevt->GetSimParticle(ipar));
            
            // counting number of pion
            if(simpar->pdg == 211 || simpar->pdg == -211 || simpar->pdg == 111) npioncounttmp +=1;
            if(simpar->pdg == 2212 || simpar->pdg == -2212) nprotoncounttmp +=1;
            if(simpar->pdg == 13 || simpar->pdg == -13) nmuoncounttmp +=1;
            
            //get muon four-momentum
            if(abs(simpar -> pdg) == 13){
                if(fabs(simpar -> momentum[2]) > mumax){
                    mumax = fabs(simpar -> momentum[2]);
                    muang_true = acos(fabs(simpar -> momentum[2])/sqrt(pow(simpar -> momentum[1],2)+pow(simpar -> momentum[2],2)+pow(simpar -> momentum[0],2)))*180/3.14159265;
                    muang_tarray[0]=simpar ->momentum[0];
                    muang_tarray[1]=simpar ->momentum[1];
                    muang_tarray[2]=simpar ->momentum[2];
                    muang_tarray[3]=simpar ->momentum[3]+0.105658;//muon mass
                }
            }
            //get pion  with highest transver momentum four-momentum
            if(abs(simpar -> pdg) == 211 || simpar->pdg == 111){
                if(fabs(simpar -> momentum[2]) > pmax){
                    pmax = fabs(simpar -> momentum[2]);
                    pang_true = acos(fabs(simpar -> momentum[2])/sqrt(pow(simpar -> momentum[1],2)+pow(simpar -> momentum[2],2)+pow(simpar -> momentum[0],2)))*180/3.14159265;
                    pang_tarray[0]=simpar ->momentum[0];
                    pang_tarray[1]=simpar ->momentum[1];
                    pang_tarray[2]=simpar ->momentum[2];
                    pang_tarray[3]=simpar ->momentum[3]+0.139570;//pion mass
                }
            }
        }//end for ipar
        npioncount = npioncounttmp;
        nmuoncount = nmuoncounttmp;
        nprotoncount = nprotoncounttmp;
        
        //true kinematics info
        muenergy_true = muang_tarray[3];
        if(muang_tarray[2]>0) mumomentum_true = sqrt(pow(muang_tarray[0],2)+pow(muang_tarray[1],2)+pow(muang_tarray[2],2));
        else mumomentum_true = -999;
        
        penergy_true = pang_tarray[3];
        if(pang_tarray[2]>0) pmomentum_true = sqrt(pow(pang_tarray[0],2)+pow(pang_tarray[1],2)+pow(pang_tarray[2],2));
        else pmomentum_true = -999;
        
        
        ///h1 for kinematic
        //h1tree  -> GetEntry(ievt);
        
        if(mode==0){//what the mode==0 for?
            Qsq_true = -999;
            tmomtrans_true = -999;
            xbjorken_true = -999;
            Erecoil_true = -999;
            HadInvWsq = -999;
        }
        else{
            for(int i=0;i<3;i++){
                Pmu[i]=muang_tarray[i];
            }
            if (mode<30) Mmu = 0.105658;
            else Mmu = 0;
            
            Enu = sqrt(Pnu[0]*Pnu[0]+Pnu[1]*Pnu[1]+Pnu[2]*Pnu[2]);
            Erecoil_true = Enu - sqrt(Pmu[0]*Pmu[0]+Pmu[1]*Pmu[1]+Pmu[2]*Pmu[2] + Mmu*Mmu);//recoil energy
            //cout<<"Check Mu Energy consistent "<<muang_tarray[3]<<" fromh1 "<<Emu<<endl;
            if(muang_tarray[2]!=-1) {
                Qsq_true = -1.0*(- pow(muang_tarray[0]-Pnu[0],2) - pow(muang_tarray[1]-Pnu[1],2) - pow(muang_tarray[2]-Pnu[2],2) + pow(muang_tarray[3]-Enu,2));
                HadInvWsq = 0.938272*0.938272 - Qsq_true + 2*0.938272*Erecoil_true;//hadronic invariant mass
            }
            else Qsq_true = -999;
            
            
            //t transfer
            if (pang_tarray[2]!=-1) {
                tmomtrans_true = -1.0*(- pow(muang_tarray[0]+pang_tarray[0]-Pnu[0],2) - pow(muang_tarray[1]+pang_tarray[1]-Pnu[1],2) - pow(muang_tarray[2]+pang_tarray[2]-Pnu[2],2) + pow(muang_tarray[3]+pang_tarray[3]-Enu,2));
            }
            else tmomtrans_true = -999;
            
            
            //bjorken
            if((Enu - muang_tarray[3])!=0) xbjorken_true = Qsq_true/(2*(Enu - muang_tarray[3])*(0.939565379+0.938272046)/2);
            else xbjorken_true = -999;
            
        }//end else mode
        
        //cout<<"Check consistent momentum "<<muang_tarray[0]<<" from h1 "<<Pmu[0]<<endl;
        ////////////////////////////////////////////////////////////////////////////////
        //Get reconstructed information
        evt      -> Clear();
        tree     -> GetEntry(ievt);
        
        int npmana = evt -> NPMAnas();
        for(int i=0; i<npmana; i++){
            pmana   = (PMAnaSummary*) (evt -> GetPMAna(i) );
            //if((pmana->vetowtracking)||(pmana->edgewtracking))continue;
            //if(evt->NIngridBeamSummarys()>0&&!pmana->ontime)continue;
            
            veract    = pmana -> clstimecorr;//temporary
            Ntrack    = pmana -> Ntrack;
            Ningtrack = pmana -> Ningtrack;
            xreco = pmana->x[0];
            yreco = pmana->y[0];
            zreco = pmana->z[0];
            
            muang_t   = 180/3.14159265*atan(sqrt(pow(tan((pmana->thetax[0]+3.65)*3.14159265/180),2)+pow(tan(pmana->thetay[0]*3.14159265/180),2)));
            //new add
            if(fabs(pmana->thetax[0])>90||fabs(pmana->thetay[0])>90){
                muang_t   = 180 - 180/3.14159265*atan(sqrt(pow(tan((pmana->thetax[0]+3.65)*3.14159265/180),2)+pow(tan(pmana->thetay[0]*3.14159265/180),2)));
            }
            muang     = pmana -> angle[0];
            
            
            mupe      = pmana -> trkpe[0];
            mufc      = pmana -> ing_stop[0];
            mupdg     = pmana -> pdg[0];
            mumucl    = pmana -> mucl[0];
            //cosin can make negative value
            /*range = 2 * 1.032 / 7.86 /1.43*1.77 * pmana->sci_range[0]
             + (6.5 + 2 * 1.032 / 7.86 /1.43*1.77) * pmana->iron_range[0];*/
            range = 2 * 1.032 / 7.86 /1.43*1.77 * fabs(pmana->sci_range[0])
            + (6.5 + 2 * 1.032 / 7.86 /1.43*1.77) * fabs(pmana->iron_range[0]);
            
            
            muscirange = pmana->sci_range[0];
            muironrange = pmana->iron_range[0];
            if(pmana->iron_pene[0]>9)
                pene = 9;
            else
                pene = pmana->iron_pene[0];
            
            //for 1strack
            startxpln    = pmana -> startxpln[0];
            startypln    = pmana -> startypln[0];
            startxch     = pmana -> startxch[0];
            startych     = pmana -> startych[0];
            endxpln    = pmana -> endxpln[0];
            endypln    = pmana -> endypln[0];
            endxch     = pmana -> endxch[0];
            endych     = pmana -> endych[0];
            
            
            exptime      = pmana -> exptime;
            opening = -999;
            coplanarity = -999;
            tmomtrans = -999;
            
            if(Ntrack>1){
                pang_t  = 180/3.14159265*atan(sqrt(pow(tan((pmana->thetax[1]+3.65)*3.14159265/180),2)+pow(tan(pmana->thetay[1]*3.14159265/180),2)));
                //newadd for backward second track
                if (fabs(pmana->thetax[1])>90||fabs(pmana->thetay[1])>90) {
                    pang_t  = 180 - 180/3.14159265*atan(sqrt(pow(tan((pmana->thetax[1]+3.65)*3.14159265/180),2)+pow(tan(pmana->thetay[1]*3.14159265/180),2)));
                }
                pang    = pmana -> angle[1];
                
                ppe     = pmana -> trkpe[1];
                pfc     = pmana -> pm_stop[1];
                pingridfc = pmana-> ing_stop[1];//newchange
                ppdg    = pmana -> pdg[1];
                pmucl   = pmana -> mucl[1];
                
                prange = 2 * 1.032 / 7.86 /1.43*1.77 * fabs(pmana->sci_range[1])
                + (6.5 + 2 * 1.032 / 7.86 /1.43*1.77) * fabs(pmana->iron_range[1]);
                
                pscirange = pmana->sci_range[1];
                pironrange = pmana->iron_range[1];
                
                //for 2nd track
                pstartxpln    = pmana -> startxpln[1];
                pstartypln    = pmana -> startypln[1];
                pstartxch     = pmana -> startxch[1];
                pstartych     = pmana -> startych[1];
                pendxpln    = pmana -> endxpln[1];
                pendypln    = pmana -> endypln[1];
                pendxch     = pmana -> endxch[1];
                pendych     = pmana -> endych[1];
                
                ppene = pmana->iron_pene[1];
                //tmomtrans = abs(range*sin(TMath::Pi()*muang_t/180.)-prange*sin(TMath::Pi()*pang_t/180.));
                
            }
            //if just one track, possible it's pion
            else{
                pang_t   = 180/3.14159265*atan(sqrt(pow(tan((pmana->thetax[0]+3.65)*3.14159265/180),2)+pow(tan(pmana->thetay[0]*3.14159265/180),2)));
                if (fabs(pmana->thetax[0])>90||fabs(pmana->thetay[0])>90) {
                    pang_t  = 180 - 180/3.14159265*atan(sqrt(pow(tan((pmana->thetax[0]+3.65)*3.14159265/180),2)+pow(tan(pmana->thetay[0]*3.14159265/180),2)));
                }
                pang     = pmana -> angle[0];
                ppe     = pmana -> trkpe[0];
                pfc     = 0;
                pingridfc = 0;//newchange
                ppdg    = 0;
                pmucl  = 0;
                pstartxpln    = 0;
                pstartypln    = 0;
                pstartxch     = 0;
                pstartych     = 0;
                pendxpln    = 0;
                pendypln    = 0;
                pendxch     = 0;
                pendych     = 0;
                prange = 0;
                ppene = 0;
            }
            
            //for opening and coplanarity
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
            
            if(p[0]*mu[0]!=0) opening     = 180/3.141692*acos((p[1]*mu[1]+p[2]*mu[2]+p[3]*mu[3])/p[0]/mu[0]);
            
            if(nup[0]*numu[0]!=0) coplanarity = 180/3.141692*acos((nup[1]*numu[1]+nup[2]*numu[2]+nup[3]*numu[3])/nup[0]/numu[0]);
            
            //angle true and resolution
            pang_diff=-1e5;
            muang_diff=-1e5;
            if(muang_tarray[2]!=-1&&mu[3]!=-1){
                muang_diff=
                acos(
                     (
                      muang_tarray[0]*mu[1]+
                      muang_tarray[1]*mu[2]+
                      muang_tarray[2]*mu[3]
                      )
                     /sqrt(muang_tarray[0]*muang_tarray[0]+muang_tarray[1]*muang_tarray[1]+muang_tarray[2]*muang_tarray[2])
                     /mu[0]
                     )*180/3.141692;
            }
            
            if(pang_tarray[2]!=-1&&p[3]!=-1){
                pang_diff=
                acos(
                     (
                      pang_tarray[0]*p[1]+
                      pang_tarray[1]*p[2]+
                      pang_tarray[2]*p[3]
                      )
                     /sqrt(pang_tarray[0]*pang_tarray[0]+pang_tarray[1]*pang_tarray[1]+pang_tarray[2]*pang_tarray[2])
                     /p[0]
                     )*180/3.141692;
            }
            
            //true information
            /*if(evt->NIngridSimVertexes()>0){
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
            }*/
            //cout<<"matching energy "<<nue_cp<<" other "<<nuE<<endl;
            
        }//end for pmana
        
        wtree -> Fill();
    }//Event Loop
    
    //######## Write and Close ####################
    wtree  -> Write();
    wfile  -> Write();
    wfile  -> Close();
    
}
