void macro12(){
  ifstream f("run290.txt");
  TFile *rf = new TFile("temp.root","recreate");
  TTree* tree = new TTree("tree","tree");
  double pe;
  int mod, view, pln, ch, verz;
  tree->Branch("pe",&pe,"pe/D");
  tree->Branch("mod",&mod,"mod/I");
  tree->Branch("view",&view,"view/I");
  tree->Branch("pln",&pln,"pln/I");
  tree->Branch("ch",&ch,"ch/I");
  tree->Branch("verz",&verz,"verz/I");

  while(f>>pe>>mod>>view>>pln>>ch>>verz)tree->Fill();
  tree->Write();
  rf->Write();
  rf->Close();
}
