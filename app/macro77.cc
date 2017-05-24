void macro77(){
  TFile* data  = new TFile("data_trkeff.root","read");
  TTree* dtree = (TTree*)data->Get("tree");
  //TFile* mc    = new TFile("mc_trkeff.root","read");
  TFile* mc    = new TFile("/home/ingrid/data/MC/10c/v3/numu/fe/onlybrec/temp.root","read");
  TTree* mtree = (TTree*)mc->Get("tree");
  TCanvas* c1 = new TCanvas("c1","c1");


  /////////////////////////////////

  TH1F*  dgen  = new TH1F("dgen","dgen",6,2,8);
  dtree       -> Draw("niron>>dgen");
  TH1F*  dsel  = new TH1F("dsel","dsel",6,2,8);
  dtree       -> Draw("niron>>dsel","!retrk");

  TH1F*  mgen  = new TH1F("mgen","mgen",6,2,8);
  mtree       -> Draw("niron>>mgen","norm*weight*totcrsne*2.91e-12");
  TH1F*  msel  = new TH1F("msel","msel",6,2,8);
  mtree       -> Draw("niron>>msel","(!retrk)*norm*weight*totcrsne*2.91e-12");


  TH1F* dr    = new TH1F("dr","dr",6,2,8);
  dr   -> Add(dsel); 
  dr   -> Divide(dgen); 
  TH1F* mr    = new TH1F("mr","mr",6,2,8);
  mr   -> Add(msel); 
  mr   -> Divide(mgen); 
 
  dr   -> SetMaximum(0.2);
  dr   -> SetMinimum(-0.1);
  dr   -> SetLineColor(2);
  mr   -> SetLineColor(4);
  dr   -> SetLineWidth(3);
  mr   -> SetLineWidth(3);
  dr   -> SetXTitle("# of penetrated irons");
  dr   -> SetYTitle("tracking inefficiency");
  dr   -> SetTitle("tracking inefficiency");
  TLegend* leg = new TLegend(0.8,0.8,0.95,0.95);
  leg  -> AddEntry(dr, "DATA");
  leg  -> AddEntry(mr, "MC");
  leg  -> SetFillColor(0);


  gStyle->SetTitleOffset(1.1,"Y");
  gStyle->SetOptStat(0);
  dr -> Draw();
  mr -> Draw("same");
  leg-> Draw();
  c1 -> Update();
  c1 -> Print("~/temppdf/100919_TrkEff.pdf");
  cin.get(); 


  TH1F* diff= new TH1F("diff","DATA-MC",6,2,8);
  diff->Add(dr);
  diff->Add(mr,-1);
  diff->SetMinimum(-0.015);
  diff->SetMaximum(0.015);
  diff->SetLineWidth(3);
  diff-> SetXTitle("# of penetrated irons");
  diff->Draw();
  for(int ibin=1; ibin<=6; ibin++)
    cout << ibin << "\t" << diff->GetBinContent(ibin)<<endl;
  c1  ->Update();
  c1 -> Print("~/temppdf/100919_TrkEff_Diff.pdf");
  cin.get();


  //////////////////////////////////

  TH1F*  dgen  = new TH1F("dgen","dgen",6,0,60);
  dtree       -> Draw("angle>>dgen","niron<8");
  TH1F*  dsel  = new TH1F("dsel","dsel",6,0,60);
  dtree       -> Draw("angle>>dsel","niron<8&&!retrk");

  TH1F*  mgen  = new TH1F("mgen","mgen",6,0,60);
  mtree       -> Draw("angle>>mgen","(niron<8)*norm*weight*totcrsne*2.91e-12");
  TH1F*  msel  = new TH1F("msel","msel",6,0,60);
  mtree       -> Draw("angle>>msel","(niron<8&&!retrk)*norm*weight*totcrsne*2.91e-12");


  TH1F* dr    = new TH1F("dr","dr",6,0,60);
  dr   -> Add(dsel); 
  dr   -> Divide(dgen); 
  TH1F* mr    = new TH1F("mr","mr",6,0,60);
  mr   -> Add(msel); 
  mr   -> Divide(mgen); 

  dr   -> SetLineColor(2);
  mr   -> SetLineColor(4);
  dr   -> SetLineWidth(3);
  mr   -> SetLineWidth(3);
  dr   -> SetXTitle("track angle[deg.]");
  dr   -> SetTitle("tracking inefficiency");
  dr   -> SetMaximum(0.2);
  dr   -> SetMinimum(-0.1);
  TLegend* leg = new TLegend(0.8,0.8,0.95,0.95);
  leg  -> AddEntry(dr, "DATA");
  leg  -> AddEntry(mr, "MC");
  leg  -> SetFillColor(0);
 
  dr   -> Draw();
  mr   -> Draw("same");
  leg  -> Draw();
  c1 -> Print("~/temppdf/100919_TrkEff_Angle.pdf");
  c1 -> Update();
  cin.get();

  TH1F* diff= new TH1F("diff","DATA-MC",6,0,60);
  diff->Add(dr);
  diff->Add(mr,-1);
  diff->SetMinimum(-0.04);
  diff->SetMaximum(0.04);
  diff->SetLineWidth(3);
  diff-> SetXTitle("track angle[deg.]");
  diff->Draw();
  c1  ->Update();
  c1 -> Print("~/temppdf/100919_TrkEff_Angle_Diff.pdf");
  cin.get();



}

