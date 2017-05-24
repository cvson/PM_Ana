#include "IngAnaCosmicTracking.hxx"


IngEvtDisp* fingevtdisp;


const static int idtest[8][2] =
  { // {plane# ,  view#}
    { 0, 0},
    { 0, 1},
    {10, 0},
    {10, 1},
    {11, 1},
    {12, 1},
    {13, 0},
    {14, 0}
  };
const static int  ntest = 8;

const static float CUT_PATH_LENZ = PlnIronThick * 6;

//#define DEBUG
#define DEBUG2

int main(int argc,char *argv[]){
  int nCosmic = 0;
  int nHit    = 0;
  int nMiss   = 0;
  TROOT root("GUI","GUI");
  TApplication theApp("App",0,0);
  frecontrack = new ReconTrackBasic();
  fingevtdisp = new IngEvtDisp();

  char buff1[300];
  int run_number;
  int sub_run_number=0;
  int n;
  Int_t c=-1;  char FileName[300];
  char Output[300]; bool rename=false;

  while ((c = getopt(argc, argv, "r:s:b:o:")) != -1) {
    switch(c){
    case 'r':
      run_number=atoi(optarg);
      break;
    case 's':
      sub_run_number=atoi(optarg);
      break;
    case 'o':
      sprintf(Output,"%s",optarg);
      rename = true;
      break;

    }
  }
  FileStat_t fs;
  ifstream timing;

  sprintf(FileName,"/home/daq/data/cosmic/ingrid_%08d_%04d_tclster.root",
	  run_number,
	  sub_run_number);



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
  Book( Form("/home/daq/data/cosmic/result/ingrid_%08d_%04d_vetoresult.root",
	     run_number, sub_run_number) );
  //Book("cosmic-test.root");

  IngridHitSummary*          inghitsum;
  IngridHitSummary*        inghitsum_t; 
  BeamInfoSummary*            beaminfo;
  int testcount  = 0;
  int testcount2 = 0;
#ifdef DEBUG
  TCanvas* evtdisp = new TCanvas("c1","c1",10,10,700,500);
#endif

  testbas       = new IngridBasicReconSummary();//### test  plane's info.
  trackingbas   = new IngridBasicReconSummary();//### other plane's info
  //for(int ievt=0; ievt<nevt; ievt++){
  for(int ievt=0; ievt<10000; ievt++){
    if(ievt%100==0) cout << "analysis " << ievt << endl;
    evt                -> Clear();
    tree               -> GetEntry(ievt);

    int nbrecon  = evt -> NIngridBasicRecons();
    for(int ibrecon=0; ibrecon<nbrecon; ibrecon++){
      basicrecon = (IngridBasicReconSummary*)evt->GetBasicRecon( ibrecon );

      clearcosmic1 = fClearCosmic_1( *basicrecon ); //### clear cosmic selection
      for(int itest=0; itest<ntest; itest++){//### test plane's loop
	int ipln  = idtest[itest][0];
	iview = idtest[itest][1];
	AllClear(); //### initialize
	CopyHit(basicrecon, testbas, trackingbas, ipln, iview);


	//#### elminate small hit(<4) event because     #####
	//#### trigger is fired with 4 TPL coincidence  #####
	//#### elminate big hit  (>16) event            #####
	//#### in orger to eliminate shower event       #####
	if( !SpecialCosmicSelection_3() )
	  continue;
	//#### other seletion ###############################
	sflag = SpecialCosmicSelection_4();


//########################################
#ifdef DEBUG  //###### Hit Drawing #######
	evtdisp     -> Clear();
	fingevtdisp -> Draw_Module    ( *evtdisp );
	fingevtdisp -> Draw_Hit_A     ( *trackingbas,0.1 );

	//evtdisp     -> Update();
	//cin.get();
#endif
//########################################
//########################################

	frecontrack -> SetHit(fIngridHit, nhits);
	if( frecontrack -> ReconTrack(1) ){ //#### ReconTrack(1=cosmic mode)

	  newfidcosmic = (frecontrack->Newfidcosmic(0)&&frecontrack->Newfidcosmic(1));
//##########################################
#ifdef DEBUG  //###### Track Drawing #######

	  TF1* f0 = new TF1("f0", "pol1",0, 130);
	  f0     -> SetParameter(0, retrk0->etx);
	  f0     -> SetParameter(1, retrk0->tx);
	  fingevtdisp -> Draw_Line(*f0, 0, retrk0->vtxi[2], retrk0->vtxf[2],1,2);

	  TF1* f1 = new TF1("f1", "pol1",0, 130);
	  f1     -> SetParameter(0, retrk1->etx);
	  f1     -> SetParameter(1, retrk1->tx);
	  fingevtdisp -> Draw_Line(*f1, 1, retrk1->vtxi[2], retrk1->vtxf[2],1,2);

	  //evtdisp     -> Update();
	  //cin.get();
#endif
//##########################################
//##########################################




	  //#########################################
	  //### Special Flag for VETO analysis ######
	  //#########################################
	  GetTrackInfo();
	  bool special_for_VETO = FlagForVETOAnalysis(basicrecon->hitmod, ipln, iview);

	  if( //startxz < PlnZ[ipln]                 && 
	      //!startisveto                         && 
	      //PlnZ[ipln] < endxz                   && 
	      //!(endisveto)                         && 
	      abs(diffverz) < 2                    &&
	      ( endxz - startxz ) >= CUT_PATH_LENZ &&
	      ( endyz - startyz ) >= CUT_PATH_LENZ && 
	      special_for_VETO ){

	    /*
	  if(ipln < nTPL){
	    if(iview == 1 && ipln ==  0)cout << startypln << endl;
	    if(iview == 0 && ipln ==  0)cout << startxpln << endl;
	    if(iview == 1 && ipln == 10)cout << endypln << endl;
	    if(iview == 0 && ipln == 10)cout << endxpln << endl;
	  }
	  cout << "module :"    << basicrecon -> hitmod << endl;
	  cout << "test plane:" << idtest[itest][0] << endl;
	      cout << "ax   :" << ax 
		   << "\tbx :" << bx
		   << "ay   :" << ay 
		   << "\tby :" << by
		   << endl;

	  if(idtest[itest][0]>=nTPL)
	    cout << "expected Z :"   << expposz_thisview 
		 << "\texpected XY:" << expposxy_anotherview << endl;
	  if( special_for_VETO )
	    cout << "analysis VETO flag is O.K." << endl;
	  if( !special_for_VETO )
	    cout << "analysis VETO flag is NO" << endl;

	    */
	    Clear();
	    Get();

	    pln        = ipln;
	    expxy0     = a0 * PlnZ[ipln] + b0;
	    expxy1     = a1 * PlnZ[ipln] + b1;
	    
	    if(ipln >= nTPL){
	      GetVetoPosZ ( a0, b0, mod, ipln , expxy0 );
	      GetVetoPosZ ( a1, b1, mod, ipln , expxy1 );
	    }

	    for(int itestbrec = 0; itestbrec < testbas -> nhits; itestbrec++ ){
	      IngridHitSummary* t = (IngridHitSummary*)testbas->GetIngridHit(itestbrec);
	      
	      if( t -> view == 0 ){
		ch_info tc;
		tc.ch   =  t->ch ;
		tc.pe   =  t->pe ;
		tc.xy   =  t->xy ;
		tc.diff =  fabs( t->xy - expxy0 ) ;
		if( ipln >= nTPL ) 
		  tc.diff =  fabs( t->z - expxy0 ) ;
		ch_info0.push_back(tc);
	      }
	      if( t -> view == 1 ){
		ch_info tc;
		tc.ch   =  t->ch ;

		tc.pe   =  t->pe ;
		tc.xy   =  t->xy ;
		tc.diff =  fabs( t->xy - expxy1 ) ;
		if( ipln >= nTPL )
		  tc.diff =  fabs( t->z - expxy1 ) ;

		ch_info1.push_back(tc);
	      }
	    }

	    //### if no hit ####
	    //##################

	    if( ch_info0.size() == 0 ){
	      ch_info tc;
	      tc.ch   =  (int)( (expxy0+0.5*ScintiWidth) / ScintiWidth) ;
	      if( ipln>= nTPL )
		tc.ch   =  (int)( (expxy0-1) / ScintiWidth) ;
	      tc.pe   =  -1e-5 ;
	      tc.xy   =  130;
	      tc.diff =  130;
	      ch_info0.push_back(tc);
	    }
	    if( ch_info1.size() == 0 ){
	      ch_info tc;
	      tc.ch   =  (int)( (expxy1+0.5*ScintiWidth) / ScintiWidth) ;

	      if( ipln>= nTPL )
		tc.ch   =  (int)( (expxy1-1) / ScintiWidth) ;
	      
	      tc.pe   =  -1e-5 ;
	      tc.xy   =  130 ;
	      tc.diff =  130;
	      ch_info1.push_back(tc);
	    }
	    Fill();
	    int tempch = ch_info1[0].ch;
	    bool draw = false;
	    if(clearcosmic1){
	      if( chi20 < 5 && chi21 < 5 ){
		if(ipln==11&&tempch==0)
		  nCosmic++;
		if( (iview==0 && ch_info0[0].diff < 20) ||
		    (iview==1 && ch_info1[0].diff < 20) 
		    )
		  if(ipln==11&&tempch==0)
		    nHit++;
	      }
		  

	    }

#ifdef DEBUG  //###### Display Updating #######
//#############################################
	    //if(!newfidcosmic)
	    //continue;
	    fingevtdisp -> Draw_Hit_A     ( *testbas,0.1, 4 ,0);
	    evtdisp     -> Update();
	    cin.get();
#endif

#ifdef DEBUG2 //########## Display Updating #######
//#################################################



	    if( draw ){
	      //if( ( chi20 < 5 && chi21 < 5 )||
	      //( chi21 < 5 && chi20 < 5 )
	      //){


	      if(ipln!=11||tempch!=0||iview!=1)continue;

	      if(draw)cout <<"IT IS INEFF" << endl;
	      cout << "tempch" << tempch << endl;
	      cout << "---------------------------------------" << endl;
	      cout << "exp z     :" << expposz_thisview    << endl; 
	      cout << "exp z high:" << expposz_thisview_high    << endl; 
	      cout << "exp z low :" << expposz_thisview_low     << endl; 
	      cout <<"module   :" << basicrecon -> hitmod         << endl; 
	      cout <<"pln      :" << ipln                         << endl; 
	      cout <<"view     :" << iview                        << endl; 
	      cout <<"diff verz:" << diffverz                     << endl; 
	      cout <<"vertex z side:" << frecontrack->GetInitialZ(1)                     << endl; 
	      cout <<"vertex z top:" << frecontrack->GetInitialZ(0)                     << endl; 

	      if(iview==0)
		cout << "exp. ch." << expxy0    
		     << "\tdiff. " << diff0[0] <<endl; 
	      if(iview==1)
		cout << "exp. ch." << expxy1
		     << "\tdiff. " << diff1[0] <<endl; 
	      if( diff0[0] >= 10 && iview==0) 
		cout << "*this view "  
		     << " expected xyz " << expxy0 
		     << endl
		     << "*another view "  
		     << " expected xyz " << expxy1 
		     << endl;
	      //cout<< "ineff. Side  diff is " << diff0[0] << "\texp. ch " << ch0[0] << endl; 
	      if( diff1[0] >= 10 && iview==1) 
		cout << "*this view "  
		     << " expected xyz " << expxy1 
		     << endl
		     << "*another view "  
		     << " expected xyz " << expxy0 
		     << endl;


	      //cout<< "ineff. Top   diff is " << diff1[0] << "\texp. ch " << ch1[0] << endl; 
	      cout << "exp posz  :" << expposz_thisview << endl;
	      cout << "exp posxy :" << expposxy_anotherview << endl;
	      if(!sflag) cout << "not special" << endl;
	      if(!clearcosmic1) cout << "not clear cosmic" << endl;
	    // frecontrack -> Print();


	    TCanvas* evtdisp = new TCanvas("c1","c1",10,10,700,500);
	    evtdisp     -> Clear();
	    fingevtdisp -> Draw_Module    ( *evtdisp , basicrecon -> hitmod);
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
	    /*
	    else{//E
	      cout <<"module "<< basicrecon -> hitmod << endl; 
	      cout <<"pln    "<< ipln << endl; 
	      cout <<"view    "<< iview << endl; 
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
	      cin.get();
	    }
	    */
#endif

	  }//### track matching
	    
	} //### tracking success


      }
    }


  }//Event Loop

  Write();

  cout << "Miss   :" << nMiss    << endl
       << "hit   :"  << nHit    << endl
       << "Cosmic :" << nCosmic  << endl
       << "ineff  :" << 1.0 * nMiss / nCosmic << endl;
    

}
 
