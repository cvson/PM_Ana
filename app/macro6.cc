void macro6(){
  double a;
  ifstream f("test.txt");
  TH1F *h = new TH1F("h","h",19,0,19);
  int i=0;
  f>>a;
  double b;
  i++;
  while(f>>b){
    cout<<a<<endl;
    //b=b-a;
    if(b>6)h->Fill(i,b);
    i++;
  }
  h->SetXTitle("Run #");
  h->SetYTitle("gain[ADC counts]");
  h->SetName("Mod07,Ch00");
  h->SetTitle("gain");
  h->Draw("p");
  c1->Update();
  cin.get();
  c1->Print("091125_gaindiff3_ch0.pdf");
}
