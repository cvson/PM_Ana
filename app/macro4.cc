void macro4(){
  ifstream f("test.dat");
  TH1F *fHmean = new TH1F("fHmean","fHmean",50,-25,25);
  fHmean  -> SetXTitle("mean of TDC difference");
  fHmean  -> SetYTitle("# of planes");
  TH1F *fGmean = new TH1F("fGmean","fGmean",294,0,294);
  fGmean  -> SetXTitle("plane ID");
  fGmean  -> SetYTitle("mean of TDC difference");
  TH1F *fHrms  = new TH1F("fHrms" ,"fHrms" ,50,2,10);
  fHrms   -> SetXTitle("RMS of TDC difference");
  fHrms   -> SetYTitle("# of planes");
  TH1F *fGrms  = new TH1F("fGrms" ,"fGrms" ,294,0,294);
  fGrms   -> SetXTitle("plane ID");
  fGrms   -> SetYTitle("RMS of TDC difference");

  double temp, mean, rms;

  int i=0;
  //while(f>>temp>>temp>>mean>>rms){
  while(f>>temp>>temp>>temp>>mean>>rms){
    if(mean!=0){
    fHmean -> Fill(mean);
    fGmean -> Fill(i,mean);
    fGrms  -> Fill(i,rms );
    fHrms  -> Fill(rms);
    i++;
    }
  }
  fHmean->Draw();
  c1    ->Update();
  c1    ->Print("091116_TDCCalib_Hmean.pdf");
  cin.get();
  fGmean->Draw();
  c1    ->Update();
  c1    ->Print("091116_TDCCalib_Gmean.pdf");
  cin.get();
  fHrms ->Draw();
  c1    ->Update();
  c1    ->Print("091116_TDCCalib_Hrms.pdf");
  cin.get();
  fGrms ->Draw();
  c1    ->Update();
  c1    ->Print("091116_TDCCalib_Grms.pdf");
  cin.get();
}
