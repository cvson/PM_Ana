void fit_result(){
  float ax, axs;
  float ay, ays;
  float a, as;
  ifstream fx("slx.txt");
  TFile*    f = new TFile("t.root","recreate");
  TTree* tree = new TTree("tree","tree");
  tree -> Branch("ax",&ax, "ax/F");
  tree -> Branch("ay",&ay, "ay/F");
  tree -> Branch("axs",&axs, "axs/F");
  tree -> Branch("ays",&ays, "ays/F");
  tree -> Branch("as",&as, "as/F");
  tree -> Branch("a",&a, "a/F");
  while(fx>>axs>>ax){
    fx>>ays>>ay;


    a  = sqrt(ax**2+ay**2)/TMath::Pi()*180;
    if( ax != 0 ) a = a * fabs(ax)/ax;
    if( ay != 0 ) a = a * fabs(ay)/ay;
    as = sqrt(axs**2+ays**2)/TMath::Pi()*180;
    if( axs != 0 ) as = as * fabs(axs)/axs;
    if( ays != 0 ) as = as * fabs(ays)/ays;
 
    axs = TMath::ATan(axs)/TMath::Pi()*180;
    ax = TMath::ATan(ax)/TMath::Pi()*180;
    ays = TMath::ATan(ays)/TMath::Pi()*180;
    ay = TMath::ATan(ay)/TMath::Pi()*180;


    tree->Fill();
  }
  tree->Write();
  f->Write();
  f->Close();
}
