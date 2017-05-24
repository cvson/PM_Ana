#include "IngAnaCosmic_new.hxx"


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
  fingevtdisp_u = new IngEvtDisp();

  int run_number;
  int sub_run_number=0;
  int n;
  Int_t c=-1;  char FileName[300];
  char Output[300];

  while ((c = getopt(argc, argv, "r:s:")) != -1) {
    switch(c){
    case 'r':
      run_number=atoi(optarg);
      break;
    case 's':
      sub_run_number=atoi(optarg);
      break;
    }
  }
  run  = run_number;
  sprintf(FileName,"/home/daq/data/cosmic/ingrid_%08d_%04d_tclster.root",run_number,sub_run_number);

 //### Read Root File #####
  int nevt  = ReadFile( FileName );
  if(nevt==-1)exit(1);
  cout << "Total # of events = " << nevt <<endl;

 //### Make Root File #####
  Book( Form("/home/daq/data/cosmic/result/ingrid_%08d_%04d_vetoresult.root",
	     run_number, sub_run_number) );
  



  testbas       = new IngridBasicReconSummary();//### test  plane's info.
  trackingbas   = new IngridBasicReconSummary();//### other plane's info
  for(int ievt=0; ievt<nevt; ievt++){           //### Event Loop


    if( ievt != 689  &&
	ievt != 1069 &&
	ievt != 1241 &&
	ievt != 2257 &&
	ievt != 2424 &&
	ievt != 2688 &&
	ievt != 2729 &&
	ievt != 2939 &&
	ievt != 3412 &&
	ievt != 3434 &&
	ievt != 3892 &&
	ievt != 4140 &&
	ievt != 4819 &&
	ievt != 5293 &&
	ievt != 5565 &&
	ievt != 5666 &&
	ievt != 5754 &&
	ievt != 6315 &&
	ievt != 7166 &&
	ievt != 7465 &&
	ievt != 7705 &&
	ievt != 7840 &&
	ievt != 8114 
	)continue;



    numevt = ievt;
    evt                 -> Clear();
    rtree               -> GetEntry(ievt);
    if(ievt%1000==0) cout << "analysis " << ievt << endl;

    int nbrecon  = evt -> NIngridBasicRecons();
    for(int ibrecon=0; ibrecon<nbrecon; ibrecon++){//### BasicRecon Loop
      basicrecon = (IngridBasicReconSummary*)evt->GetBasicRecon( ibrecon );

      if( ! fClearCosmic_1( *basicrecon ) ) //### clear cosmic selection
	continue;
      for(int itest=0; itest<ntest; itest++){       //### test plane's loop
	mod  = basicrecon -> hitmod;
	pln  = idtest[itest][0];
	view = idtest[itest][1];
	if( pln>=1 && pln <=9 )continue;
	//if( pln>=nTPL )continue;
	//if( basicrecon ->  hitmod!=13 )continue;
	if( pln != BVETO )continue;


	AllClear(); //### initialize
	CopyHit(basicrecon, testbas, trackingbas, pln, view);//### Copy to IngridHit to testbas and trackingbas and fIngridHit

	if( nhits < 6 )continue;

	frecontrack -> SetHit(fIngridHit, nhits); //### Set fIngridHit to ReconTrack
	if( frecontrack -> ReconTrack(1) ){ //#### ReconTrack(1=cosmic mode)

	  GetTrackInfo();

	  trkmatching = false;
	  if( abs(diffverz) < 2 ) trkmatching = true; //### Track matching
	  

	  trkendmatching = fTrkEndMatching(mod, pln, view);
	  trklen         = fTrkLen    ();
	  goodchi2       = fGoodChi2  ();
	  goVETO         = fGoVETO    (mod, pln, view);
	  notescape      = fNotEscape (mod, pln, view);
	  trgsamepln     = fTrgSamePln(mod, pln, view);
	  notstop        = fNotStop   (mod, pln, view);
	  anaflag = 
	    trkmatching    && trkendmatching && trklen &&
	    goodchi2  && goVETO && notescape && trgsamepln &&
	    notstop;

	  if( 1 ){


	    nCosmic++;	      

	    Clear();
	    Get  ();

	    Fill();
	    bool draw = false;

	    if( fch_info[0].diff>20 ){
	      nMiss++;
	      draw = true;
	    }



#ifdef DEBUG //########## Display Updating #######
//#################################################

	    bool fff = 
	      trkmatching    &&
	      trkendmatching &&
	      trklen         &&
	      goodchi2       &&
	      goVETO         &&
	      notescape      &&
	      notstop        &&
	      trgsamepln;

	    if( fff && draw ) {
		

	      if(draw)cout << "miss" << endl;
	      cout << "event :" << ievt << endl;
	      Print();
	      //PrintHit(trackingbas);
	      TCanvas* evtdisp = new TCanvas("c1","c1",10,10,700,500);
	      evtdisp     -> Clear();
	      fingevtdisp -> Draw_Module    ( *evtdisp , basicrecon -> hitmod);
	      fingevtdisp -> Draw_Hit_A     ( *trackingbas,0.1,2,1 );

	      /*
	      IngridBasicReconSummary* under_module = new IngridBasicReconSummary();
	      under_module -> Clear();
	      SetHit2Basic( evt, under_module, mod-1, basicrecon -> hitcyc );
	      TCanvas* evtdisp_u = new TCanvas("c2","c2",10,510,700,500);
	      evtdisp_u     -> Clear();
	      fingevtdisp_u -> Draw_Module    ( *evtdisp_u , mod-1);
	      fingevtdisp_u -> Draw_Hit_A     ( *under_module,0.1,2,1 );
	      */


	      TF1* f0 = new TF1("f0", "pol1",0, 130);
	      f0     -> SetParameter(0, retrk0->etx);
	      f0     -> SetParameter(1, retrk0->tx);
	      fingevtdisp -> Draw_Line(*f0, 0, retrk0->vtxi[2], retrk0->vtxf[2],1,2);
	      TF1* f1 = new TF1("f1", "pol1",0, 130);
	      f1     -> SetParameter(0, retrk1->etx);
	      f1     -> SetParameter(1, retrk1->tx);
	      fingevtdisp -> Draw_Line(*f1, 1, retrk1->vtxi[2], retrk1->vtxf[2],1,2);

	      fingevtdisp -> Draw_Hit_A     ( *testbas,0.1, 4 ,0);

	      //##### all hit information #####
	      //###############################
	      /*	      
	      cout << "=== all hit ====" << endl;
	      int nallhit = evt -> NIngridModHits( mod, basicrecon->hitcyc );
	      IngridHitSummary* allinghit;
	      IngridBasicReconSummary* bbb = new IngridBasicReconSummary();
	      for(int iallhit=0; iallhit < nallhit; iallhit++ ){
		allinghit = (IngridHitSummary*) (evt -> GetIngridModHit(iallhit, mod, basicrecon->hitcyc    ));
		//if( allinghit->view == 1 )
		cout << "view:"<< allinghit -> view<< "\t"
		     << "pln:" << allinghit -> pln << "\t"
		     << "ch:"  << allinghit -> ch  << "\t"
		     << "pe:"  << allinghit -> pe  << "\t"
		     << "time:"<< allinghit -> time<< "\t"
		     << endl;
		bbb -> AddIngridHit(allinghit);
	      }
	      fingevtdisp -> Draw_Hit_A     ( *bbb,0.1,1,1 );
	      */
	      /*
	      cout << "analysis mod        : " << mod    << endl
		   << "analysis plane      : " << pln    << endl
		   << "analysis view       : " << view   << endl
		   << "analysis expected xy: " << expxy  << endl
		   << "analysis expected z : " << expz   << endl
		   << "diff vertex z       : " << diffverz   << endl;
	      */
	      evtdisp     -> Update();
	      //evtdisp_u   -> Update();

	      cin.get();
	    }
#endif

	  }//### Good Track
	    
	} //### tracking success


      }
    }


  }//Event Loop

  Write();

  cout << "nMiss  :" << nMiss                 << endl
       << "nCosmic:" << nCosmic               << endl
       << "Ineiff :" << 1.0 * nMiss / nCosmic << endl;

    

}
 
