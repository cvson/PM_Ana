#include"get_nevt_several_fid.hxx"
#include "math.h"
FileStat_t fs;
const int NRunStart = 6272;
const int NRunEnd =   12493;
//const int NRunEnd =   10000;
int main(int argc,char *argv[]){
  TROOT        root ("GUI","GUI");
  TApplication app  ("App",0,0);

  Int_t c=-1;  char FileName[300];
  char Output[300];
  Int_t run_number=0;
  bool  mc = false;
  while ((c = getopt(argc, argv, "mo:")) != -1) {
    switch(c){
    case 'm':
      mc = true;
      break;
    case 'o':
      sprintf(Output,"%s",optarg);
      break;
    }
  }
  cout << Output << endl;
  TFile*              rfile;
  TTree*              tree;
  TBranch*            EvtBr;
  IngridEventSummary* evt = new IngridEventSummary();
  const int nregion= 5;

  int Nevt[nMod][2][nregion];
  for(int imod=0; imod<nMod; imod++){
    for(int i=0; i<2; i++){
      for(int j=0; j<nregion; j++){
	Nevt[imod][i][j]=0;
      }
    }
  }
  for(int irun = NRunStart; irun < NRunEnd; irun++){
    for(int isrun = 0; isrun < 100; isrun++){
    
      sprintf(FileName, "/home/daq/data/dst/ingrid_%08d_%04d_track.root",irun, isrun );
  //for(int irun = 29; irun <= 33; irun++){
  //for(int irun = 0; irun <= 100; irun++){
 
  //for(int irun = 0; irun <= 100; irun++){
  //for(int isrun = 3; isrun < 5; isrun++){
    
	//sprintf(FileName, "/home/daq/data/dst/merged_%d_track.root",irun );
	//sprintf(FileName, "/home/daq/data/mc_data/ana/ingmc_numu_fe_nd%0d_horn250ka_%d_track.root",isrun,irun );
    
      if(gSystem->GetPathInfo(FileName,fs))
	continue;
      
      cout<<"reading "<<FileName<<"....."<<endl;  
      //#### Read ROOT File after basic reconstruction ######
      //#####################################################
      rfile = new TFile(FileName,"read");
      tree  = (TTree*)rfile -> Get("tree");
      EvtBr = tree->GetBranch("fDefaultReco.");

      EvtBr ->SetAddress(&evt);
      tree  ->SetBranchAddress("fDefaultReco.", &evt);
      int                nevt = (int)tree -> GetEntries();
      cout << "Total # of events = " << nevt <<endl;




      IngridBasicReconSummary* basicrecon;
      for(int ievt = 0; ievt < nevt; ++ievt){
	if(ievt%1000==0) cout << ievt << endl;
	evt  -> Clear();
	tree -> GetEntry(ievt);
	for( int ibas=0; ibas < evt->NIngridBasicRecons(); ibas++ ){
	  basicrecon = (IngridBasicReconSummary*) ( evt -> GetBasicRecon( ibas ) );

	  Bool_t  nuregwofid  = 
	    (  basicrecon -> layerpe > 6.5 ) &&
	    (  basicrecon -> nactpln > 2   ) &&
	    (  basicrecon -> hastrk        ) &&
	    (  basicrecon -> matchtrk      ) &&
	    (  basicrecon -> ontime || mc  ) &&
	    (!(basicrecon -> vetowtracking) );

	  if( nuregwofid ){
	    if( basicrecon -> vertexxz <  4 ){
	      for(int icutch=0; icutch < nregion; icutch++){
		if( FidX(basicrecon, icutch) && FidY(basicrecon, icutch) )
		  Nevt[basicrecon->hitmod][0][icutch]++;
	      }
	    }
	    if( basicrecon -> vertexxz >= 4 ){
	      for(int icutch=0; icutch < nregion; icutch++){
		if( FidX(basicrecon, icutch) && FidY(basicrecon, icutch) )
		  Nevt[basicrecon->hitmod][1][icutch]++;
	      }

	    }


	  }
	  
	}//basic recon
      }//evt
      rfile -> Close();
      }//isrun
  }//irun

  ofstream f(Output);
  
  for(int imod=0; imod<nMod; imod++){
    for(int j=0; j<nregion; j++){
      for(int i=0; i<2; i++){
	f << Nevt[imod][i][j] <<"\t"
	  << 1.0 * Nevt[imod][i][j] / Nevt[imod][i][2] <<"\t"
	  << sqrt( abs(Nevt[imod][i][j]-Nevt[imod][i][2]) ) / Nevt[imod][i][2] <<"\t";
      }
      f<<endl;
    }

  }
  f.close();
	
}
 
