void ana_several_fid(string str, string run){
  gStyle -> SetOptStat(0);
  const int     np       = 5;
  char*         name [np]= {"vpro","vproup","vprodown","vproin", "vproinin"};
  char*         title[np]= 
    {Form("nominal fiducial(Run%s)", run.c_str()),
     "upstream(ver. z=1~4)",
     "downstream(ver. z=5~8)",
     "internal(ver. xy = 3~20)", 
     "internal(ver. xy = 4~19)"};
  TFile*        f        = new TFile(str.c_str(),"read");
  TF1*          gaus     = new TF1("gaus","gaus");
  TCanvas*      c1       = new TCanvas("c1","c1",1200,800 );
  TGraphErrors* fG   [np];
  TH1F*         frame[np];
  gaus -> SetLineStyle(2); 
  gaus -> SetLineWidth(1); 
  c1   -> Divide(3,2);
  ofstream wf(Form("%s_vfitres.txt", run.c_str()));
  for(int ip=0; ip<np; ip++){
    if(ip<=2)c1 -> cd(ip+1);
    if(ip> 2)c1 -> cd(ip+2);
    fG[ip] = (TGraphErrors*)f->Get(name[ip]);
    fG[ip]->SetLineColor(2);
    fG[ip]->SetLineWidth(2);
    fG[ip]->Fit ("gaus","q","",-500,500);
    frame[ip] = new TH1F(title[ip],title[ip],1000,-500,500);
    frame[ip] ->SetMaximum( gaus->GetParameter(0)*1.2 );
    frame[ip] ->SetMinimum( 0 );
    frame[ip] ->SetXTitle ("Y[cm] from INGRID center");
    frame[ip] ->Draw();
    fG[ip]->Draw("P");
    wf << gaus->GetParameter(1) << "\t"
       << gaus->GetParError (1) << "\t"
       << gaus->GetParameter(2) << "\t"
       << gaus->GetParError (2) << "\t";

  }
  c1 -> Update();
  c1 -> Print ( Form("100609_%s_vresult.pdf",run.c_str()));
  wf << endl;
  wf.close();


}
