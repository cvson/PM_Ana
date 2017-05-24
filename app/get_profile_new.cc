#include"get_profile_new.hxx"

int main(int argc,char *argv[]){
  TROOT        root ("GUI","GUI");
  TApplication app  ("App",0,0);

  Int_t c=-1;  char FileName[300]; char Output[300]; char WF[300];

  sprintf(Output,"temp.root");
  while ((c = getopt(argc, argv, "f:o:w:")) != -1) {
    switch(c){
    case 'f':
      sprintf(FileName,"%s",optarg);
      break;
    case 'o':
      sprintf(Output,"%s",optarg);
      break;
    case 'w':
      sprintf(WF,"%s",optarg);
      break;

    }
  }

  int nevt = Read(FileName);
  cout << "total nevt " << nevt << endl;
  Reset();
  for(int ievt = 0; ievt < nevt; ++ievt){
    if( ievt%1000 == 0 )cout<<ievt<<endl;
    evt  -> Clear();
    tree -> GetEntry(ievt);


    for( int ibasic=0; ibasic < evt -> NIngridBasicRecons(); ibasic++ ){

      basicrecon = (IngridBasicReconSummary*) ( evt -> GetBasicRecon( ibasic ) );
      Count( basicrecon ); 
    }//ibasic
  }//ievt
  Print( WF );
  Book ( Output );
}
