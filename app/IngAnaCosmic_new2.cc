#include "IngAnaCosmic_new2.hxx"


IngEvtDisp* fingevtdisp;
IngEvtDisp* fingevtdisp_u;





#define DEBUG

int main(int argc,char *argv[]){
  int nCosmic = 0;
  int nMiss   = 0;

  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);

  frecontrack = new ReconTrackBasic();
  fingevtdisp = new IngEvtDisp();
 

  int sub_run_number=0;
  int n;
  Int_t c=-1;  char FileName[300];

  while ((c = getopt(argc, argv, "r:s:")) != -1) {
    switch(c){
    case 'r':
      run=atoi(optarg);
      break;
    case 's':
      sub_run_number=atoi(optarg);
      break;
    }
  }
  sprintf(FileName,"/home/daq/data/cosmic/ingrid_%08d_%04d_tclster.root",run,sub_run_number);
 //### Read Root File #####
  int nevt  = ReadFile( FileName );
  if(nevt==-1)exit(1);
  cout << "Total # of events = " << nevt <<endl;
 //### Make Root File #####
  //Book( Form("/home/daq/data/cosmic/result/ingrid_%08d_%04d_vetoresult_new2.root",
  Book( "t.root" );
	     
  


  basicrecon    = new IngridBasicReconSummary();//### test  layer's info.
  testbas       = new IngridBasicReconSummary();//### test  layer's info.
  trackingbas   = new IngridBasicReconSummary();//### other layer's info
  for(ievt=0; ievt<nevt; ievt++){           //### Event Loop
    if(ievt%1000==0) cout << "analysis " << ievt << endl;
    evt                 -> Clear();
    rtree               -> GetEntry(ievt);

    for(int ibrecon=0; ibrecon < evt -> NIngridBasicRecons(); ibrecon++){//### BasicRecon Loop
      basicrecon = (IngridBasicReconSummary*)evt->GetBasicRecon( ibrecon );

      //for(int itest=0; itest<ntest; itest++){       //### test plane's loop
      for(int itest=0; itest<4; itest++){       //### test plane's loop
	mod  = basicrecon -> hitmod;
	cyc  = basicrecon -> hitcyc;
	if     ( itest==0 ){
	  pln  = RVETO;
	  view = FromY;
	}
	else if( itest==1 ){
	  pln  = LVETO;
	  view = FromY;
	}
	else if( itest==2 ){
	  pln  = BVETO;
	  view = FromX;
	}
	else if( itest==3 ){
	  pln  = UVETO;
	  view = FromX;
	}

	//### Set hit to testbas and trackingbas
	testbas     -> Clear();
	trackingbas -> Clear();
	for(int ipln=0; ipln < nTPL+nVETO; ipln++){
	  for(int iview=0; iview < nView; iview++){
	    if( ipln == pln && iview == view )
	      SetHit2Basic( basicrecon, testbas,     mod, cyc, iview, ipln );
	    else
	      SetHit2Basic( basicrecon, trackingbas, mod, cyc, iview, ipln );
	  }
	}
	//### Copy trackingbas to fIngridHit
	nhits = 0;
	for(int ihit=0; ihit < trackingbas->nhits; ihit++){
	  fIngridHit[nhits] = 0;
	  fIngridHit[nhits] = TRef( (IngridHitSummary*)trackingbas -> GetIngridHit(ihit) );
	  nhits++;
	}
  	for(int ini=nhits; ini<INGRIDHIT_MAXHITS; ini++){
	  fIngridHit[ini]=0;
	}

	if( nhits < 6 )continue;

	//### Set fIngridHit to frecontrack and tracking
	frecontrack -> Clear();
	frecontrack -> SetHit(fIngridHit, nhits); //### Set fIngridHit to ReconTrack
	if( frecontrack -> ReconTrack(1) ){ //#### ReconTrack(1=cosmic mode)
	  GetTrackInfo();
	  svpreVETOxy = fSVPreVETOXY();
	  svexpz      = fSVExpZ();
	  ovexpxy     = fOVExpXY();

	  GetHitChInfo();

	  if(  6  < svexpz      && svexpz      < 101 )  fsvexpz      =true;
	  else                                          fsvexpz      =false;
	  if(itest==2){
	    if(  10 < ovexpxy     && ovexpxy     < 100 )  fovexpxy     =true;
	    else                                          fovexpxy     =false;
	  }
	  else{
	    if(  10 < ovexpxy     && ovexpxy     < 110 )  fovexpxy     =true;
	    else                                          fovexpxy     =false;
	  }
	  if( itest == 0 || itest == 2 ){
	    if( svpreVETOxy <  -5 )  fsvprevetoxy =true;
	    else                     fsvprevetoxy =false;
	  }
	  else if( itest == 1 || itest == 3 ){
	    if( svpreVETOxy > 120 )  fsvprevetoxy =true;
	    else                     fsvprevetoxy =false;
	  }
	  if( trklenx > 5 )  ftrklenx = true;
	  else               ftrklenx = false;
	  if( trkleny > 5 )  ftrkleny = true;
	  else               ftrkleny = false;
	  if( abs(diffstartz) < 2 )  ftrkstart = true;
	  else                       ftrkstart = false;
	  if( abs(diffendz)   < 2 )  ftrkend   = true;
	  else                       ftrkend = false;

	  fana = fsvexpz && fovexpxy && fsvprevetoxy && ftrklenx && ftrkleny && ftrkstart && ftrkend;
	  //fana = fsvexpz && fovexpxy && ftrklenx && ftrkleny && ftrkstart && ftrkend;


	  Fill();
#ifdef DEBUG //########## Display Updating #######
//#################################################

	  if( fana && diff[0]>20 ){

		
	    if(fana&&diff[0]>20)cout << "miss" << endl;
	    if(fana&&diff[0]<=20)cout << "hit" << endl;


	    TCanvas* evtdisp = new TCanvas("c1","c1",10,10,700,500);
	    evtdisp     -> Clear();
	    fingevtdisp -> Draw_Module    ( *evtdisp , basicrecon -> hitmod);
	    fingevtdisp -> Draw_Hit_A     ( *trackingbas,0.1,2,1 );
	    TF1* f0 = new TF1("f0", "pol1",0, 130);
	    f0     -> SetParameter(0, retrk0->etx);
	    f0     -> SetParameter(1, retrk0->tx);
	    fingevtdisp -> Draw_Line(*f0, 0, retrk0->vtxi[2], retrk0->vtxf[2],1,2);
	    TF1* f1 = new TF1("f1", "pol1",0, 130);
	    f1     -> SetParameter(0, retrk1->etx);
	    f1     -> SetParameter(1, retrk1->tx);
	    fingevtdisp -> Draw_Line(*f1, 1, retrk1->vtxi[2], retrk1->vtxf[2],1,2);


	    fingevtdisp -> Draw_Hit_A     ( *testbas,0.1, 4 ,0);


	    evtdisp     -> Update();
	   

	    Print();
	    cin.get();
	  }
#endif

	    
	} //### tracking success


      }
    }
  

  }//Event Loop

  Write();

  cout << "nMiss  :" << nMiss                 << endl
       << "nCosmic:" << nCosmic               << endl
       << "Ineiff :" << 1.0 * nMiss / nCosmic << endl;

    

}
 
