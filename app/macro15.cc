void macro15(){
  ifstream file("testfile.txt");
  TFile* f=new TFile("testfile.root","recreate");
  float pe;
  int   set;
  TTree* tree=new TTree("tree","tree");
  tree->Branch("pe",  &pe, "pe/F");
  tree->Branch("set", &set, "set/I");
  while(file>>pe>>set)
    tree->Fill();
  tree->Write();
  f->Write();
  f->Close();
}
