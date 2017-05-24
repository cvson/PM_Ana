void macro13(){
  double x[12], xe[12];
  for(int i=0; i<12; i++){
    x[i]  = 5 + 10 * i;
    xe[i] = 2.8;
  }

  double y [12]={25.7, 26.1, 24.5, 22.9, 21.9, 20.4,
		19.9, 18.9, 18.6, 17.8, 17.4, 17.0 };
  double ye[12]={0.2,0.2,0.2,0.2,0.2,0.2,
		 0.2,0.2,0.2,0.3,0.4,0.4};

  TGraphErrors* g = new TGraphErrors(12,x,y,xe,ye);
  TH1F* fH = new TH1F("fH","fH",120,0,120);
  fH -> SetMaximum(30);
  fH -> SetMinimum(10);
  fH -> SetXTitle("Distance from MPPC[cm]");
  fH -> SetYTitle("Peak light yield[PE/1cm]");
  fH -> SetTitle ("Attenuation of peak light yield");
  TF1* expo = new TF1("expo", "[0]*exp(0-1.0*x/[1])");
  expo->SetParameter(0, 30);
  expo->SetParameter(1, 200);
  expo->SetLineColor(2);
  expo->SetLineWidth(2);
  expo->SetLineStyle(2);
  g->SetLineWidth(3);

  gStyle->SetOptStat(0000);
  gStyle->SetOptFit(110);
  fH -> Draw("");
  g->Draw("P");
  g->Fit("expo","","",20, 100);
}
