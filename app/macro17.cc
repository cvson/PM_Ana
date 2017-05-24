void macro17(){
  TFile* fdata = new TFile("testfiledata.root","read");
  TFile* fmc   = new TFile("testfilemc.root","read");
  TTree* fTdata= (TTree*)fdata->Get("tree");
  TTree* fTmc  = (TTree*)fmc->Get("tree");
  TH1F* fHdatamiss = new TH1F("fHdatamiss","fHdatamiss",18,0,92);
  TH1F* fHdatacos  = new TH1F("fHdatacos","fHdatacos",18,0,92);
  TH1F* fHdataeff  = new TH1F("fHdataeff","fHdataeff",18,0,92);

  TH1F* fHmcmiss = new TH1F("fHmcmiss","fHmcmiss",18,0,92);
  TH1F* fHmccos  = new TH1F("fHmccos","fHmccos",18,0,92);
  TH1F* fHmceff  = new TH1F("fHmceff","fHmceff",18,0,92);
  TH1F* fHeff  = new TH1F("fHeff","fHeff",18,0,92);
  
  fHdatamiss->Sumw2();
  fHdatacos->Sumw2();
  fHmcmiss->Sumw2();
  fHmccos->Sumw2();

  fTdata->Project("fHdatacos","angle");
  fTdata->Project("fHdatamiss","angle","hit");
  fTmc  ->Project("fHmccos","angle");
  fTmc  ->Project("fHmcmiss","angle","hit");

  for(int i=1;i<=18;i++){
    if(fHdatacos->GetBinContent(i)>0){
      fHdataeff->SetBinContent(i, 1-fHdatamiss->GetBinContent(i)/fHdatacos->GetBinContent(i));
      fHdataeff->SetBinError(i, sqrt(fHdatamiss->GetBinContent(i))/fHdatacos->GetBinContent(i));
     
    }
    else{
      fHdataeff->SetBinContent(i, 0);
      fHdataeff->SetBinError(i,   0);
    }
    if(fHmccos->GetBinContent(i)>0){
      fHmceff->SetBinContent(i, 1.005-fHmcmiss->GetBinContent(i)/fHmccos->GetBinContent(i));
      fHmceff->SetBinError(i, sqrt(fHmcmiss->GetBinContent(i))/fHmccos->GetBinContent(i));
    }
    else{
      fHmceff->SetBinContent(i, 0);
      fHmceff->SetBinError(i,   0);
    }

  }
  gStyle->SetOptStat(0000);
  gStyle->SetTitleOffset(1.2,"Y");
  fHdataeff->SetMaximum(1.01);
  fHdataeff->SetMinimum(0.950);
  fHdataeff->SetLineWidth(3);  
  fHdataeff->SetLineColor(2);  
  fHdataeff->SetYTitle("Hit efficiency");
  fHdataeff->SetXTitle("angle[deg.]");
  fHdataeff->SetTitle("Angular dependence of hit efficiency");
  TLegend* leg = new TLegend(0.7,0.8,0.94,0.98);
  leg->AddEntry(fHdataeff,"DATA");
  leg->AddEntry(fHmceff,"MC");
  leg->SetFillColor(0);
  fHmceff->SetLineWidth(3);  
  fHmceff->SetLineWidth(3);  


  fHeff->Add(fHdataeff);
  fHeff->Add(fHmceff,-1);
  fHdataeff->Draw();
  fHmceff->Draw("same");
  leg->Draw();
  //c1->Print("~/temppdf/101206_HitEff_DATA_MC.pdf");
  fHeff->SetMaximum(0.05);
  fHeff->SetMinimum(-0.05);
  fHeff->SetLineWidth(3);
  fHeff->SetXTitle("angle[deg.]");
  fHeff->SetYTitle("DATA-MC");
  fHeff->SetTitle("DATA-MC");

  //fHeff->Draw();
  //c1->Print("~/temppdf/101206_HitEff_DATA_MC.pdf");
 
}
