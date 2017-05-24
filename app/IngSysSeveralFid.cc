#include "IngSysSeveralFid.hxx"
int main(int argc,char *argv[]){

  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);

  char buff1[300], Output[300];
  int run_number;
  int sub_run_number=0;
  int n;
  Int_t c=-1;
  bool  mc    = false;
  bool  jnu10c= false;

  while ((c = getopt(argc, argv, "mjo:")) != -1) {
    switch(c){

    case 'm':
      mc = true;
      break;
    case 'j':
      jnu10c = true;
      break;
    case 'o':
      sprintf(Output,"%s",optarg);
      break;
    }
  }
  ofstream wfile(Output);




  int ifile=0;
  cout << (FileName[0]) << endl;
  //TCanvas* c1 = new TCanvas("c1","c1");

  while(ifile<nfile){
    cout<<"reading "<<(FileName[ifile])<<"....."<<endl;
    if(gSystem->GetPathInfo(FileName[ifile],fs)){
      cout<<"Cannot open file "<<FileName[ifile]<<endl;
      exit(1);
    }

    TFile*            rfile = new TFile(FileName[ifile],"read");
    TTree*             tree = (TTree*)rfile -> Get("tree");
    TBranch*          EvtBr = tree->GetBranch("fDefaultReco.");
    EvtBr                   ->SetAddress(&evt);
    tree                    ->SetBranchAddress("fDefaultReco.", &evt);  
    int                nevt = (int)tree -> GetEntries();
    cout << "Total # of events = " << nevt <<endl;
    TH1F* fH = new TH1F("fH","fH",14,0,14);
    for(int ifid=0; ifid < nfid; ifid++){

      if(!mc&&!jnu10c)
	sprintf( EvtSelect, "hastrk&&matchtrk&&ontime&&!vetowtracking&&!edgewtracking");
      else if(mc)
	sprintf( EvtSelect, "hastrk&&matchtrk&&!vetowtracking&&!edgewtracking" );
      else if(jnu10c)
	sprintf( EvtSelect, "(hastrk&&matchtrk&&!vetowtracking&&!edgewtracking)*norm*totcrsne*weight" );
      
     
      sprintf( FidCut, "%d>=vertexx&&vertexx>=%d&&%d>=vertexy&&vertexy>=%d&&%d>=vertexxz&&vertexxz>=%d&&%d>=vertexyz&&vertexyz>=%d", 
	       fidverxmax[ifid],
	       fidverxmin[ifid],
	       fidverymax[ifid],
	       fidverymin[ifid],
	       fidverzmax[ifid],
	       fidverzmin[ifid],
	       fidverzmax[ifid],
	       fidverzmin[ifid]);
      tree -> Draw("hitmod>>fH", Form("%s&&%s", EvtSelect, FidCut) );    
      //c1   -> Update();
      //cin.get();
      for(int imod=0; imod<14; imod++){
	wfile << fH->GetBinContent(imod+1) << "\t";
      }
      wfile << endl;

    }//ifid
    ifile++;
  }//while
  wfile.close();
}
