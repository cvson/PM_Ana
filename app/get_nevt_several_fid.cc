#include"get_nevt_several_fid.hxx"


int main(int argc,char *argv[]){
  TROOT        root ("GUI","GUI");
  TApplication app  ("App",0,0);

  Int_t c=-1;  char FileName[300];
  string Output;
  Int_t run_number=0;
  bool  mc = false;
  while ((c = getopt(argc, argv, "f:o:m")) != -1) {
    switch(c){
    case 'f':
      sprintf(FileName,"%s",optarg);
      break;
    case 'o':
      Output = optarg;
      //sprintf(Output,"%s",optarg);
      break;
    case 'm':
      mc = true;
      break;

    }
  }
  FileStat_t fs;
  cout<<"reading "<<FileName<<"....."<<endl;
  if(gSystem->GetPathInfo(FileName,fs)){
    cout<<"Cannot open file "<<FileName<<endl;
    exit(1);
  }
      
  //#### Read ROOT File after basic reconstruction ######
  //#####################################################
  TFile*            rfile = new TFile(FileName,"read");
  TTree*             tree = (TTree*)rfile -> Get("tree");
  TBranch*          EvtBr = tree->GetBranch("fDefaultReco.");
  IngridEventSummary* evt = new IngridEventSummary();
  EvtBr                   ->SetAddress(&evt);
  tree                    ->SetBranchAddress("fDefaultReco.", &evt);
  int                nevt = (int)tree -> GetEntries();
  cout << "Total # of events = " << nevt <<endl;



  Book(Output);
  IngridBasicReconSummary* basicrecon;
  for(int ievt = 0; ievt < nevt; ++ievt){
    if(ievt%1000==0) cout << ievt << endl;
    evt  -> Clear();
    tree -> GetEntry(ievt);
    for( int ibas=0; ibas < evt->NIngridBasicRecons(); ibas++ ){
e      basicrecon = (IngridBasicReconSummary*) ( evt -> GetBasicRecon( ibas ) );

      Bool_t  nuregwofid  = 
	(  basicrecon -> layerpe > 6.5 ) &&
	(  basicrecon -> nactpln > 2   ) &&
	(  basicrecon -> hastrk        ) &&
	(  basicrecon -> matchtrk      ) &&
	(  basicrecon -> ontime || mc  ) &&
	(!(basicrecon -> vetowtracking) );

      if( nuregwofid ){
	Fill(basicrecon);
      }

    }//basic recon
  }//evt
  Write();
	
}
 
