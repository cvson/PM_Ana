void macro3(){
  ifstream f("run3968_calib.txt");
  int mod,tfb,ch;
  double gain,noise,caa;
  TTree *tree = new TTree("tree","tree");
  tree->Branch("gain",&gain,"gain/D");
  while(f>>mod>>tfb>>ch>>gain>>noise>>caa){
    if(gain!=-444){//good channel
      if(mod==1&&tfb==1&&ch==35){
	//if(gain>14){
	cout<<"mod: "<<mod;
	cout<<" tfb: "<<tfb;
	cout<<" ch: "<<ch;
	cout<<" gain: "<<gain;
	cout<<" noise: "<<noise;
	cout<<" caa: "<<caa<<endl;
      }
      tree->Fill();
    }
  }
  tree->Draw("gain");
}
