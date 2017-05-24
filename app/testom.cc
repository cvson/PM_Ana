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
#include <TGFrame.h>
#include <TGClient.h>

class disp : public TGMainFrame{
public:
  disp( const TGWindow *p=NULL , UInt_t w=800, UInt_t h=600) ;
  virtual ~disp(){
    Cleanup();
  }

};
disp::disp( const TGWindow *p , UInt_t w, UInt_t h) 
    : TGMainFrame(p, w, h)
{
  MapSubwindows();
  Resize( GetDefaultSize() );
  MapWindow();
}
int main(int argc, char **argv){
  //TROOT root("GUI","GUI");
  TApplication theApp("App",&argc,argv);
  char *buff = "localhost";
  //disp d(gClient->GetRoot(),200,400);
  theApp.Run();
  return 0;
}
