#include "IngAnaCosmicTracking.hxx"


IngEvtDisp* fingevtdisp;


const static float CUT_PATH_LENZ = PlnIronThick * 6;

//#define DEBUG
//#define DEBUG2
#define TolOfHit 10

int main(int argc,char *argv[]){
  //ofstream ofile("cosmic_test.txt");
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  frecontrack = new ReconTrackBasic();
  fingevtdisp = new IngEvtDisp();

  char buff1[300];
  int run_number;
  int sub_run_number=0;
  int n;
  Int_t c=-1;  
  char FileName[300];
  char Output[300]; 
  bool rename = false;
  bool renamef= false;

  while ((c = getopt(argc, argv, "f:o:r:s:")) != -1) {
    switch(c){

    case 'f':
      sprintf(FileName,"%s",optarg);
      run_number=0;
      renamef = true;
      break;

    case 'o':
      sprintf(Output,"%s",optarg);
      rename  = true;
      break;

    case 'r':
      run_number=atoi(optarg);
      break;
    case 's':
      sub_run_number=atoi(optarg);
      break;
    }
  }
  FileStat_t fs;
  ifstream timing;

  if(!renamef){
    sprintf(FileName,"/home/kikawa/scraid1/data/pm_ingrid_track/ingrid_%08d_%04d_done.root",
	    run_number,
	    sub_run_number);
  }

  if( !rename ){
    sprintf(Output,"/home/kikawa/scraid1/data/pm_ingrid_track_cosmicana/ingrid_%08d_%04d_cosmicana.root",
	    run_number,
	    sub_run_number);
  }

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

  //#####
  /*
  Book( Form("/home/daq/data/cosmic/result/ingrid_%08d_%04d_result.root",
	     run_number, sub_run_number) );
  */
  IngridEventSummary* wevt= new IngridEventSummary();
  TFile*            wfile = new TFile( Output ,"recreate");
  TTree*            wtree = new TTree("tree","tree");
  MakeHist();
  wtree -> Branch( "fDefaultReco.", "IngridEventSummary", 
		   &wevt, 64000, 99);



  IngridHitSummary*          inghitsum;
  IngridHitSummary*        inghitsum_t; 
  IngridBasicReconSummary*  basicrecon;
  BeamInfoSummary*            beaminfo;

#ifdef DEBUG
  TCanvas* evtdisp = new TCanvas("c1","c1",10,10,700,500);
#endif
  TRef  fIngridHit[INGRIDHIT_MAXHITS];
  int ntrkok    = 0;
  int nretrkok0 = 0;
  int nretrkok1 = 0;
  IngridBasicReconSummary* testbas     = new IngridBasicReconSummary();//### test  plane's info.
  IngridBasicReconSummary* trackingbas = new IngridBasicReconSummary();//### other plane's info
  for(int ievt=0; ievt<nevt; ievt++){
  //for(int ievt=0; ievt<5000; ievt++){
    if(ievt%100==0) cout << "analysis " << ievt << endl;
    evt                -> Clear();
    tree               -> GetEntry(ievt);

    int nbrecon  = evt -> NIngridBasicRecons();
    for(int ibrecon=0; ibrecon<nbrecon; ibrecon++){
      basicrecon = (IngridBasicReconSummary*)evt->GetBasicRecon( ibrecon );


      for(int ipln=0; ipln<nTPL; ipln++){//### ipln = Test plane
	if( ipln == 0 || ipln == 10 )continue;
	testbas     -> Clear();
	trackingbas -> Clear();
	//#### Add IngridHit to TRef which is not test plane(ipln) ######
	//###############################################################
	int refhits    = basicrecon -> nhits;
	int nhits      = 0;
	int nhits0     = 0;
	int nhits1     = 0;
	for( int ihit=0; ihit < refhits; ihit++ ){
	  IngridHitSummary* b = (IngridHitSummary*)(basicrecon -> GetIngridHit(ihit));
	  if( b->pln == ipln ){
	    testbas   -> AddIngridHit( b );
	    continue;
	  }
	  fIngridHit[nhits] = 0;
	  fIngridHit[nhits] = TRef( (IngridHitSummary*)basicrecon -> GetIngridHit(ihit) );
	  nhits++;
	  trackingbas -> AddIngridHit( b );
	  if(b->view==0)
	    nhits0++;

	  if(b->view==1)
	    nhits1++;

	}

	for(int ini=nhits; ini<INGRIDHIT_MAXHITS; ini++){
	  fIngridHit[ini]=0;
	}

	//#### elminate small hit(<4) event because trigger is fired with 4 TPL coincidence #####
	if( nhits0 < 4 || nhits1 < 4 )
	  continue;


	  

#ifdef DEBUG  //###### Hit Drawing #######
	evtdisp     -> Clear();
	fingevtdisp -> Draw_Module    ( *evtdisp );
	fingevtdisp -> Draw_Hit_A     ( *trackingbas,0.1 );
#endif


	frecontrack -> Clear();
	frecontrack -> SetHit(fIngridHit, nhits);
	if( frecontrack -> ReconTrack(0) ){ 
	  IngridTrackSummary* retrk0 = frecontrack -> GetTrack(0, 0);
	  IngridTrackSummary* retrk1 = frecontrack -> GetTrack(1, 0);


#ifdef DEBUG  //###### Track Drawing #######
	  TF1* f0 = new TF1("f0", "pol1",0, 130);
	  f0     -> SetParameter(0, retrk0->etx);
	  f0     -> SetParameter(1, retrk0->tx);
	  fingevtdisp -> Draw_Line(*f0, 0, retrk0->vtxi[2], retrk0->vtxf[2],1,2);
	  TF1* f1 = new TF1("f1", "pol1",0, 130);
	  f1     -> SetParameter(0, retrk1->etx);
	  f1     -> SetParameter(1, retrk1->tx);
	  fingevtdisp -> Draw_Line(*f1, 1, retrk1->vtxi[2], retrk1->vtxf[2],1,2);
#endif


	  float startxz    = retrk0 -> vtxi[2];
	  float endxz      = retrk0 -> vtxf[2];
	  float startyz    = retrk1 -> vtxi[2];
	  float endyz      = retrk1 -> vtxf[2];
	  bool startisveto = frecontrack -> startisveto[0] || frecontrack -> startisveto[1];
	  bool endisveto   = frecontrack -> endisveto[0]   || frecontrack -> endisveto[1];
	  diffverz   = frecontrack -> GetInitialZ( 0 ) - frecontrack -> GetInitialZ( 1 );
	  if( startxz < PlnZ[ipln]                 && 
	      !startisveto                         && 
	      PlnZ[ipln] < endxz                   && 
	      !(endisveto)                         && 
	      abs(diffverz) < 2                    &&
	      ( endxz - startxz ) >= CUT_PATH_LENZ &&
	      ( endyz - startyz ) >= CUT_PATH_LENZ 
	      ){
	    

	    chi20      = retrk0->chi2x;
	    chi21      = retrk1->chi2x;


	    ch_info0.clear();
	    ch_info1.clear();
	    mod        = basicrecon -> hitmod;
	    cyc        = basicrecon -> hitcyc;
	    pln        = ipln;
	    a0         = retrk0 -> tx;  
	    b0         = retrk0 -> etx;
	    a1         = retrk1 -> tx;
	    b1         = retrk1 -> etx;
	    expxy0     = a0 * PlnZ[ipln] + b0; //expected position at view 0
	    expxy1     = a1 * PlnZ[ipln] + b1; //expected position at view 1
	    expch0     = (int)( (expxy0+0.5*ScintiWidth)/5 );
	    expch1     = (int)( (expxy1+0.5*ScintiWidth)/5 );
	    angle      = TMath::ATan( sqrt(a0*a0+a1*a1) ) / TMath::Pi() * 180;  	    

	    //### check hit and whether it is expected position or not ####
	    //### if there is no hit, set only gocosmic true           ####
	    bool nohit0 = true;
	    bool nohit1 = true;
	    for(int itestbrec = 0; itestbrec < testbas -> nhits; itestbrec++ ){
	      IngridHitSummary* thit = (IngridHitSummary*)testbas->GetIngridHit(itestbrec);
	      int tch   = thit->ch;
	      int tview = thit->view;
	      int tpln  = thit->pln;
	      float pecorr   = thit->Pe()*cos(angle*TMath::Pi()/180);
	      fHly[mod][tview][tpln][tch]->Fill(pecorr);
	      if( tview == 0 ){
		if( fabs( thit->xy - expxy0 ) < TolOfHit ){
		  thit -> hitcosmic = true;
		  thit -> gocosmic  = true;
		  thit -> pecorr = pecorr;
		  nohit0    = false;
		}
	      }
	      if( tview == 1 ){
		if( fabs( thit->xy - expxy1 ) < TolOfHit ){
		  thit -> hitcosmic = true;
		  thit -> gocosmic  = true;
		  nohit1    = false;
		  thit -> pecorr = pecorr;
		}
	      }
	    }

	    if( nohit0 ){
	      IngridHitSummary* thit = new IngridHitSummary();
	      thit -> mod  = mod;
	      thit -> pln  = ipln;
	      thit -> view = 0;
	      thit -> ch   = expch0;
	      thit -> gocosmic   = true;
	      evt  -> AddIngridModHit(thit, mod, cyc);
	    }
	    if( nohit1 ){
	      IngridHitSummary* thit = new IngridHitSummary();
	      thit -> mod  = mod;
	      thit -> pln  = ipln;
	      thit -> view = 1;
	      thit -> ch   = expch1;
	      thit -> gocosmic   = true;
	      evt  -> AddIngridModHit(thit, mod, cyc);
	    }




#ifdef DEBUG  //###### Display Updating #######
//#############################################
	    if( nohit0 )
	      cout << "View 0 : No" << endl;
	    if( nohit1 )
	      cout << "View 1 : No" << endl;
	    
	    fingevtdisp -> Draw_Hit_A     ( *testbas,0.1, 4 ,0);
	    evtdisp     -> Update();
	    cin.get();
#endif

#ifdef DEBUG2 //########## Display Updating #######
//#################################################
	    if( ( chi20 < 5 && diff0[0] >= 10 )||
		( chi21 < 5 && diff1[0] >= 10 )
		){
	      cout <<"module "<< basicrecon -> hitmod << endl; 

	      if( diff0[0] >= 10 ) cout<< "ineff. Side " << ch0[0] << endl; 
	      if( diff1[0] >= 10 ) cout<< "ineff. Top  " << ch1[0] << endl; 
	      if(!sflag) cout << "not special" << endl;
	      if(!clearcosmic1) cout << "not clear cosmic" << endl;
	    // frecontrack -> Print();


	    TCanvas* evtdisp = new TCanvas("c1","c1",10,10,700,500);
	    evtdisp     -> Clear();
	    fingevtdisp -> Draw_Module    ( *evtdisp );
	    fingevtdisp -> Draw_Hit_A     ( *trackingbas,0.1,2,1 );
	    //fingevtdisp -> Draw_Hit_A     ( *trackingbas,1,2,0 );

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

	    int tmod = basicrecon -> hitmod;
	    int tcyc = basicrecon -> hitcyc;

	    cout << "base time " << basicrecon -> clstime << endl;
	    //##### all hit information #####
	    //###############################
	    /*
	    cout << "=== all hit ====" << endl;
	    int nallhit = evt -> NIngridModHits( tmod, tcyc );
	    IngridHitSummary* allinghit;
	    for(int iallhit=0; iallhit < nallhit; iallhit++ ){
	      allinghit = (IngridHitSummary*) (evt -> GetIngridModHit(iallhit, tmod, tcyc    ));
	      cout << "view:"<< allinghit -> view<< "\t"
		   << "pln:" << allinghit -> pln << "\t"
		   << "ch:"  << allinghit -> ch  << "\t"
		   << "pe:"  << allinghit -> pe  << "\t"
		   << "time:"<< allinghit -> time<< "\t"
		   << endl;
	    }

	    //##### clster hit information #####
	    //##################################
	    cout << "=== clster hit ====" << endl;
	    int nclshit = basicrecon -> nhits;
	    IngridHitSummary* clsinghit;
	    for(int iclshit=0; iclshit < nclshit; iclshit++ ){
	      clsinghit = (IngridHitSummary*)( basicrecon -> GetIngridHit(iclshit) );
	      cout << "view:"<< clsinghit -> view<< "\t"
		   << "pln:" << clsinghit -> pln << "\t"
		   << "ch:"  << clsinghit -> ch  << "\t"
		   << "pe:"  << clsinghit -> pe  << "\t"
		   << "time:"<< clsinghit -> time<< "\t"
		   << endl;
	    }


	    //##### other cyc hit information #####
	    //#####################################
	    int ocyc;
	    if( tcyc == 14 )ocyc=15;
	    if( tcyc == 15 )ocyc=14;
	    cout << "=== other hit ====" << ocyc << endl;
	    int notherhit = evt -> NIngridModHits( tmod, ocyc );
	    IngridHitSummary* otheringhit;
	    for(int iotherhit=0; iotherhit < notherhit; iotherhit++ ){
	      otheringhit = (IngridHitSummary*) (evt -> GetIngridModHit(iotherhit, tmod, ocyc    ));
	      cout << "view:"<< otheringhit -> view<< "\t"
		   << "pln:" << otheringhit -> pln << "\t"
		   << "ch:"  << otheringhit -> ch  << "\t"
		   << "pe:"  << otheringhit -> pe  << "\t"
		   << "time:"<< otheringhit -> time<< "\t"
		   << endl;
	    }

	    if( tcyc == 14 )ocyc=13;
	    if( tcyc == 15 )ocyc=16;
	    cout << "=== other hit ====" << ocyc << endl;
	    notherhit = evt -> NIngridModHits( tmod, ocyc );
	    for(int iotherhit=0; iotherhit < notherhit; iotherhit++ ){
	      otheringhit = (IngridHitSummary*) (evt -> GetIngridModHit(iotherhit, tmod, ocyc    ));
	      cout << "view:"<< otheringhit -> view<< "\t"
		   << "pln:" << otheringhit -> pln << "\t"
		   << "ch:"  << otheringhit -> ch  << "\t"
		   << "pe:"  << otheringhit -> pe  << "\t"
		   << "time:"<< otheringhit -> time<< "\t"
		   << endl;
	    }
	    */

	    cin.get();
	    }
#endif

	  }//### track matching
	    
	} //### tracking success


      }
    }
    wevt = evt;
    wtree -> Fill();


  }//Event Loop

  wtree->Write();
  wfile->Write();
  wfile->Close();
  //Write();



}
 
