char jnubeam[300];
char IntCut [300];

void efficiency_curve(string str){
  char fidHmod[300], fidVmod[300], NeutCut[300];
  sprintf( fidHmod, 
	   "fSimVertex.mod<7&&( fSimVertex.x+525-150*fSimVertex.mod )>25&&( fSimVertex.x+525-150*fSimVertex.mod )<125&&fSimVertex.y>-50&&fSimVertex.y<50&&fSimVertex.z<33.5" );
  sprintf( fidVmod, 
	   "fSimVertex.mod>=7&&( fSimVertex.y+525-150*(fSimVertex.mod-7) )>25&&( fSimVertex.y+525-150*(fSimVertex.mod-7) )<125&&fSimVertex.x>-50&&fSimVertex.x<50&&fSimVertex.z<-366.5" );
  sprintf( NeutCut, "(hastrk&&matchtrk&&!vetowtracking&&!edgewtracking)");

  TFile*   f       = new TFile(str.c_str(), "read");
  TTree*   tree    = (TTree*)f->Get("tree");

  TFile*   wf      = new TFile("eff_curve_ingrid.root","recreate");
  TH1D*    gen     = new TH1D ("gen",
			       "generated events in FV",
			       200, 0, 10);
  TH1D*    obs     = new TH1D ("obs",
			       "selected events",
			       200, 0, 10);
  TH1D*    eff     = new TH1D ("eff",
			       "efficiency",
			       200, 0, 10);

  TH1D*    gencc   = new TH1D ("gencc",
			       "generated CC events in FV",
			       200, 0, 10);
  TH1D*    obscc   = new TH1D ("obscc",
			       "selected CC events",
			       200, 0, 10);
  TH1D*    effcc   = new TH1D ("effcc",
			       "efficiency for CC",
			       200, 0, 10);
  /*
  TH1D*    genccqe = new TH1D ("genccqe",
			       "generated CCQE events in FV",
			       200, 0, 10);
  TH1D*    obsccqe = new TH1D ("obsccqe",
			       "selected CCQE events",
			       200, 0, 10);
  TH1D*    effccqe = new TH1D ("effccqe",
			       "efficiency for CCQE",
			       200, 0, 10);

  TH1D*    genccother = new TH1D ("genccother",
				  "generated CC other events in FV",
				  200, 0, 10);
  TH1D*    obsccother = new TH1D ("obsccother",
				  "selected CC other events",
				  200, 0, 10);
  TH1D*    effccother = new TH1D ("effccother",
				  "efficiency for CC Other",
				  200, 0, 10);
  */
  TH1D*    gennc   = new TH1D ("gennc",
			       "generated NC events in FV",
			       200, 0, 10);
  TH1D*    obsnc   = new TH1D ("obsnc",
			       "selected NC events",
			       200, 0, 10);
  TH1D*    effnc   = new TH1D ("effnc",
			       "efficiency for NC",
			       200, 0, 10);


  tree   -> Draw("nuE>>gen",
		 Form("( (%s||%s) )*norm*totcrsne*weight*2.91e-12",fidHmod, fidVmod));
		 //Form("( (%s||%s) )*norm*totcrsne*weight*2.57e-12",fidHmod, fidVmod));
  tree   -> Draw("nuE>>obs",
		 Form("( %s )*norm*totcrsne*weight*2.91e-12", NeutCut));

  tree   -> Draw("nuE>>gencc",
		 Form("( (%s||%s)&&inttype < 30 )*norm*totcrsne*weight*2.91e-12",fidHmod, fidVmod));
  tree   -> Draw("nuE>>obscc",
		 Form("( %s&&inttype < 30 )*norm*totcrsne*weight*2.91e-12", NeutCut));
  /*
  tree   -> Draw("nuE>>genccqe",
		 Form("( (%s||%s)&&inttype==1 )*norm*totcrsne*2.91e-12",fidHmod, fidVmod));
  tree   -> Draw("nuE>>obsccqe",
		 Form("( %s&&inttype==1 )*norm*totcrsne*2.91e-12", NeutCut));
  
  tree   -> Draw("nuE>>genccother",
		 Form("( (%s||%s)&&(inttype!=1&&inttype<30) )*norm*totcrsne*2.91e-12",fidHmod, fidVmod));
  tree   -> Draw("nuE>>obsccother",
		 Form("( %s&&(inttype!=1&&inttype<30) )*norm*totcrsne*2.91e-12", NeutCut));
  */

  tree   -> Draw("nuE>>gennc",
		 Form("( (%s||%s)&&inttype>=30 )*norm*totcrsne*weight*2.91e-12",fidHmod, fidVmod));
  tree   -> Draw("nuE>>obsnc",
	       	 Form("( %s&&inttype>=30 )*norm*totcrsne*weight*2.91e-12", NeutCut));
  
  gen             -> SetXTitle("Enu[GeV]");
  obs             -> SetXTitle("Enu[GeV]");
  eff             -> SetXTitle("Enu[GeV]");
  gencc           -> SetXTitle("Enu[GeV]");
  obscc           -> SetXTitle("Enu[GeV]");
  effcc           -> SetXTitle("Enu[GeV]");
  /*
  genccqe         -> SetXTitle("Enu[GeV]");
  obsccqe         -> SetXTitle("Enu[GeV]");
  effccqe         -> SetXTitle("Enu[GeV]");
  genccother      -> SetXTitle("Enu[GeV]");
  obsccother      -> SetXTitle("Enu[GeV]");
  effccother      -> SetXTitle("Enu[GeV]");
  */
  gennc           -> SetXTitle("Enu[GeV]");
  obsnc           -> SetXTitle("Enu[GeV]");
  effnc           -> SetXTitle("Enu[GeV]");

  /*
  genccqe         -> Sumw2();
  obsccqe         -> Sumw2();
  genccother      -> Sumw2();
  obsccother      -> Sumw2();
  gennc           -> Sumw2();
  obsnc           -> Sumw2();
  */
  eff             -> Add   (obs);
  eff             -> Divide(gen);
  effcc           -> Add   (obscc);
  effcc           -> Divide(gencc);
  //effccqe         -> Add   (obsccqe);
  //effccqe         -> Divide(genccqe);
  //effccother      -> Add   (obsccother);
  //effccother      -> Divide(genccother);
  effnc           -> Add   (obsnc);
  effnc           -> Divide(gennc);

  ofstream wfile("eff_curve_ingrid.txt");
  for(int i=0; i<200; i++){
    wfile << i+1       << "\t"
	  << 50*i      << "\t"
	  << 50*(i+1)  << "\t"
	  << 50*i+25   << "\t"
	  << gen        -> GetBinContent(i) << "\t"
	  << gencc      -> GetBinContent(i) << "\t"
      //<< genccqe    -> GetBinContent(i) << "\t"
      //<< genccother -> GetBinContent(i) << "\t"
	  << gennc      -> GetBinContent(i) << "\t"
	  << obs        -> GetBinContent(i) << "\t"
	  << obscc      -> GetBinContent(i) << "\t"
      //<< obsccqe    -> GetBinContent(i) << "\t"
      //<< obsccother -> GetBinContent(i) << "\t"
	  << obsnc      -> GetBinContent(i) << "\t"
	  << eff        -> GetBinContent(i) << "\t"
	  << effcc      -> GetBinContent(i) << "\t"
      //<< effccqe    -> GetBinContent(i) << "\t"
      //<< effccother -> GetBinContent(i) << "\t"
	  << effnc      -> GetBinContent(i) << "\t"
	  << endl;
  }
  wfile.close();

  wf -> Write();
  wf -> Close();


}

