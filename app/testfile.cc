void testfile(){
  ifstream f("testfile.txt");

  float pos;
  int   view;
  float pe;
  TFile* rf    = new TFile("testfile.root","recreate");
  TTree* tree  = new TTree("tree","tree");
 
  tree -> Branch("pos",  &pos,  "pos/F");
  tree -> Branch("view", &view, "view/I");
  tree -> Branch("pe",   &pe,   "pe/F");
  while( f >> view >> pos >> pe ){
    tree ->Fill();
  }
  tree->Write();
  rf  ->Write();
  rf  ->Close();
}
