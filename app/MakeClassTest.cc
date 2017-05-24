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
#include "NeutInfoSummary.h"


int main(int argc,char *argv[]){
    char ipFile[200];//,buff2[200];
    TROOT root("GUI","GUI");
    TApplication theApp("App",0,0);
    Int_t c=-1;
    char ipTree[200];
    char opName[200];
    while ((c = getopt(argc, argv, "t:f:a:")) != -1) {
        switch(c){
            case 'f':
                sprintf(ipFile,"%s",optarg);
                break;
            case 't':
                sprintf(ipTree,"%s",optarg);
                break;
            case 'a':
                sprintf(opName,"%s",optarg);
                break;
        }
    }
    
    TFile *pfile = new TFile(ipFile);
    TTree *ptree = (TTree*)pfile->Get(ipTree);
    ptree->MakeClass(opName);

    
}
