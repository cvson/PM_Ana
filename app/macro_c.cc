void macro_c(){
  TTree* tree = new TTree("tree","tree");
  float a,b;
  tree -> Branch("a",&a,"a/F");
  tree -> Branch("b",&b,"b/F");
  ifstream f("cosmic_test.txt");
  while(f>>a>>b)tree->Fill();
}
