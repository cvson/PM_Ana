#include "IngAddWeight.hxx"




int main(int argc,char *argv[]){

  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  Set();


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

  ifstream timing;
  //######## read root file #############################
  //#####################################################
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

  TFile*            wfile = new TFile(Output,"recreate");
  TTree*            wtree = new TTree("tree","tree");
  IngridEventSummary* wsummary = new IngridEventSummary(); 
  wtree              -> Branch   ("fDefaultReco.","IngridEventSummary", 
				 &wsummary,  64000,   99);



  IngridSimVertexSummary* simver;
  for( int ievt = 0; ievt < nevt; ievt++  ){
    //for( int ievt = 0; ievt < 1000; ievt++  ){
    if(ievt%100==0)cout << "analyze event# " << ievt<<endl;
    evt      -> Clear();
    wsummary -> Clear();
    tree -> GetEntry(ievt);

    simver = (IngridSimVertexSummary*)(evt->GetSimVertex(0));
    int flavor = (int)( (simver->promode)/10 )-1;
    
    simver -> weight  = Get_Weight( 0, flavor, simver->mod, simver->nuE );
    simver -> errorhp = Get_Weight( 1, flavor, simver->mod, simver->nuE );
    //cout << simver->mod << "\t" << simver->nuE*1000 << "\t" << simver->weight << endl;
    

    wsummary  = evt;
    wtree    -> Fill();
  }//Event Loop


  //######## Write and Close ####################
  wtree  -> Write();
  wfile  -> Write();
  wfile  -> Close();
  rfile  -> Close();
}
 

