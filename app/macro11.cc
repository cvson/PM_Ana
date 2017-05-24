void macro11(){
  ifstream f("study_background.txt");
  TFile *rf = new TFile("study_background.root","recreate");
  int mod, view, pln, ch;
  double pe;
  TTree *tree = new TTree("tree","tree");
  tree->Branch("mod",  &mod,  "mod/I");
  tree->Branch("vidw", &view, "view/I");
  tree->Branch("pln",  &pln , "pln/I");
  tree->Branch("ch",   &ch  , "ch/I");
  tree->Branch("pe",   &pe,   "pe/D");
  while(f>>mod>>pln>>view>>ch>>pe)tree->Fill();
  tree->Write();
  rf->Write();
  rf->Close();
}
