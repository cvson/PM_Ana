void macro_draw(){
  const int    nbin = 20;
  const double bins[nbin+1]={
    0., 0.2, 0.4, 0.6, 0.8, 1.0, 1.2, 1.4, 1.6, 1.8, 2.0, 2.4, 2.8, 3.2, 3.6, 4.0, 5, 6, 7, 8, 10};

  TH1F* fHgen = new TH1F("fHgen","fHgen",nbin, bins);
  TH1F* fHsel = new TH1F("fHsel","fHsel",nbin, bins);
  TH1F* fHeff = new TH1F("fHeff","fHeff",nbin, bins);
  tree->Project("fHgen","Enu", "inttype<30&&fid");
  tree->Project("fHsel","Enu", "inttype<30&&rec");
  fHgen -> Sumw2();
  fHsel -> Sumw2();
  fHeff -> Add(fHsel);
  fHeff -> Divide(fHgen);

  TH1F* fHgen_ = new TH1F("fHgen_","fHgen_",nbin, bins);
  TH1F* fHsel_ = new TH1F("fHsel_","fHsel_",nbin, bins);
  TH1F* fHeff_ = new TH1F("fHeff_","fHeff_",nbin, bins);
  tree->Project("fHgen_","Enu", "abs(inttype)==1&&fid&&thetamu<10&&Emu>0.3");
  tree->Project("fHsel_","Enu", "abs(inttype)==1&&rec&&thetamu<10&&Emu>0.3");
  fHgen_ -> Sumw2();
  fHsel_ -> Sumw2();
  fHeff_ -> Add(fHsel_);
  fHeff_ -> Divide(fHgen_);


  fHeff -> GetYaxis() -> SetRangeUser(0,1.05);


  fHeff  -> Draw("");
  fHeff_ -> Draw("same");
}
