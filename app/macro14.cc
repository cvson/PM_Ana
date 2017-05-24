void macro14(){
  ifstream file("testfile.txt");
  TFile* f=new TFile("testfile.root","recreate");
  int view, set;
  float pe;
  TTree* tree=new TTree("tree","tree");
  tree->Branch("set", &set, "set/I");
  tree->Branch("view", &view, "view/I");
  tree->Branch("pe", &pe, "pe/F");
  while(file>>view>>pe>>set)
    tree->Fill();
  tree->Write();
  f->Write();
  f->Close();
}
