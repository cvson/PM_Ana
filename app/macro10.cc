void macro10(){
  ifstream f("test.dat");
  int mod,tfb,ch;
  double gain,noise,caa;
  TTree *tree = new TTree("tree","tree");
  tree->Branch("gain",&gain,"gain/D");
  tree->Branch("noise",&noise,"noise/D");
  tree->Branch("caa",&caa,"caa/D");
  //while(f>>mod>>tfb>>ch>>gain>>noise>>caa){
  int count=0;
  for(int i=0;i<8338;i++){
    f>>mod>>tfb>>ch>>gain>>noise>>caa;

    if(gain!=-444){
      tree->Fill();
      //if(gain>14)    cout<<mod<<" "<<tfb<<" "<<ch<<endl;
      //if(noise>0.89)    cout<<mod<<" "<<tfb<<" "<<ch<<endl;
      if(caa<0.0)cout<<mod<<" "<<tfb<<" "<<ch<<endl;
      if(fabs(0.58-noise)<0.01)count++;
    }
  }
  cout<<count<<endl;
  tree->Draw("noise>>h(100,0,2)");
  h->SetXTitle("noise rate[MHz]");
  h->SetYTitle("# of channel");
  h->SetName("all modules");
  h->Draw();
  c1->Print("090929_noise_2.pdf");
}
