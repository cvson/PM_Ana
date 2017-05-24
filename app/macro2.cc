void macro2(){
  int cyc;
  long tdc;
  ifstream f("test.dat");
  TH2F *h = new TH2F("h","h",25,0,25,100,-10000,50000);
  TH1F *h2 = new TH1F("h2","h2",50,0,40);
  while(f>>cyc>>tdc){
    h->Fill(cyc,tdc);
    h2->Fill(tdc*2.5/1000);
  }
  h2->Draw();
}
