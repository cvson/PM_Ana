void macro16(){
  ifstream file("testfilemc1.txt");
  TFile* f=new TFile("testfilemc1.root","recreate");
  float angle;
  int   hit;
  TTree* tree=new TTree("tree","tree");
  tree->Branch("angle",  &angle, "angle/F");
  tree->Branch("hit",    &hit,   "hit/I");
  while(file>>angle>>hit)
    tree->Fill();
  tree->Write();
  f->Write();
  f->Close();
}
