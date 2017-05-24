void macro_draw_111202(string file){
  gStyle -> SetOptStat(0000);
  gStyle -> SetTitleX(0.2);
  gStyle -> SetPadLeftMargin(0.12);
  gStyle -> SetPadRightMargin(0.1);
  TGaxis::SetMaxDigits(3);
  TFile* frF         = new TFile(file.c_str(), "read");
  TTree* tree        = (TTree*)frF->Get("tree");


  TFile* fF          = new TFile("ingrid_mdpln_for_pileup_111202.root","recreate");
  TH1F*  fH_nu_mdpln = new TH1F ("fH_nu_mdpln","most downstream active plane of neutrino events (MR RUN 29 and 30)",11,0,11);
  TH1F*  fH_nofv_mdpln = new TH1F ("fH_nofv_mdpln","most downstream active plane of out-of-fv events (MR RUN 29 and 30)",11,0,11);
  TH1F*  fH_veto_mdpln = new TH1F ("fH_veto_mdpln","most downstream active plane of veto events (MR RUN 29 and 30)",11,0,11);

  tree -> Project("fH_nu_mdpln","mdpln","veto==0&&fv==0");
  tree -> Project("fH_nofv_mdpln","mdpln","veto==0&&fv==1");
  tree -> Project("fH_veto_mdpln","mdpln","veto==1");


  fH_nu_mdpln -> SetTitleOffset(0.9,"Y");
  fH_veto_mdpln -> SetTitleOffset(0.9,"Y");
  fH_nofv_mdpln -> SetTitleOffset(0.9,"Y");
  fH_nu_mdpln -> SetXTitle("plane number");
  fH_nofv_mdpln -> SetXTitle("plane number");
  fH_veto_mdpln -> SetXTitle("plane number");
  fH_nu_mdpln -> SetYTitle("number of events");
  fH_nofv_mdpln -> SetYTitle("number of events");
  fH_veto_mdpln -> SetYTitle("number of events");

  fH_nu_mdpln -> Sumw2();
  fH_nofv_mdpln -> Sumw2();
  fH_veto_mdpln -> Sumw2();

  fF   -> Write();
  fF   -> Close();
}
