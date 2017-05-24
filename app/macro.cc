void macro(){
  TFile *fdata = new TFile("~/test.root", "read");
  TTree *tree = (TTree*)fdata->Get("tree");
  TFile *fexp  = new TFile("~/expect.root", "read");
  char buff[300];
  TH2F *fHdata[14][2];
  TH2F *fHexp [14][2];
  TH1F *fH1data[14][2]; 
  TH1F *fH1exp [14][2];
  TH1F *fH1div [14][2];
  TH1F *fHmap  [14];
  TH1F *fHly   [14];
  for(int i=0;i<14;i++){
    sprintf(buff,"Mod%02dActMap",i);
    fHmap[i]=(TH1F*)fdata->Get(buff);
    fHmap[i]->SetXTitle("Pln. ID");
    fHmap[i]->SetYTitle("# of activity");
    sprintf(buff,"Mod%02dPeMean",i);
    fHly[i]=(TH1F*)fdata->Get(buff);
    fHly[i]->SetXTitle("mean of light yield");
    fHly[i]->SetYTitle("# of clusters");
    for(int j=0;j<2;j++){
      if(j==0)sprintf(buff,"Mod%02dXActMap",i);
      if(j==1)sprintf(buff,"Mod%02dYActMap",i);
      fHdata[i][j]=(TH2F*)fdata->Get(buff);
      fHdata[i][j]->SetXTitle("plane ID");
      fHdata[i][j]->SetYTitle("channel ID");
      fHexp [i][j]=(TH2F*)fexp ->Get(buff);
      fHexp[i][j]->SetXTitle("plane ID");
      fHexp[i][j]->SetYTitle("channel ID");


      if(j==0)sprintf(buff,"Mod%02dXActMapdataProj",i);
      if(j==1)sprintf(buff,"Mod%02dYActMapdataPfoj",i);
      fH1data[i][j] = new TH1F(buff,buff,100,0,300);
      if(j==0)sprintf(buff,"Mod%02dXActMapexpProj",i);
      if(j==1)sprintf(buff,"Mod%02dYActMapexpPfoj",i);
      fH1exp [i][j] = new TH1F(buff,buff,100,0,300);
      if(j==0)sprintf(buff,"Mod%02dXActMapdivProj",i);
      if(j==1)sprintf(buff,"Mod%02dYActMapdivPfoj",i);
      fH1div [i][j] = new TH1F(buff,buff,100,0,300);
    }
  }


  for(Int_t nummod=0;nummod<14;nummod++){
    for(Int_t view=0;view<2;view++){
      for(Int_t i=1;i<=11;i++){
	for(Int_t j=1;j<=24;j++){
	  double data = fHdata[nummod][view]->GetBinContent(i,j);
	  double exp  = fHexp [nummod][view]->GetBinContent(i,j);
	  fH1data[nummod][view] -> Fill(data);
	  fH1exp [nummod][view] -> Fill(exp);
	  fH1div [nummod][view] -> Fill (1.0*data/exp);
	}
      }
    }
  }


  TCanvas *c1 = new TCanvas("c1","c1",10,10,500,600);
  for(Int_t nummod=0;nummod<14;nummod++){
    c1->SetLogy();
    fHly[nummod]->Draw();
    c1->Update();
    sprintf(buff,"091106_Mod%02dPeMean.pdf",nummod);
    c1->Print(buff);
    cin.get();

    /*
    c1->SetLogy();
    TH1F *h = new TH1F("h","h",11,0,11);
    sprintf(buff,"fActPln[%d]>>h",nummod);
    tree->Draw(buff);
    sprintf(buff,"Mod%d",nummod);
    h->SetName(buff);
    h->SetXTitle("# of active planes");
    h->SetYTitle("# of events");
    h->SetTitle("Activities");
    h->Draw();
    sprintf(buff,"091106_Mod%02dPlnHit.pdf",nummod);
    c1->Print(buff);
    cin.get();
    */
    /*
    fHmap[nummod]->Draw();
    c1->Update();
    sprintf(buff,"091106_Mod%02dPlnHitMap.pdf",nummod);
    c1->Print(buff);
    cin.get();
    */
    for(Int_t view=0;view<2;view++){
      /*
      c1->Divide(1,2);
      c1->cd(1);
      fHdata[nummod][view]->Draw("box");
      c1->cd(2);
      fHexp [nummod][view]->Draw("box");
      c1->Update();
      if(view==0)sprintf(buff,"091106_Mod%02dXChHitMap.pdf",nummod);
      if(view==1)sprintf(buff,"091106_Mod%02dYChHitMap.pdf",nummod);
      c1->Print(buff);
      cin.get();
      c1->Clear();
      */
      /*
      fH1data[nummod][view]->Draw();
      c1->Update();
      cin.get();
      fH1exp[nummod][view]->Draw();
      c1->Update();
      cin.get();
      fH1div[nummod][view]->Draw();
      c1->Update();
      cin.get();
      */
    }
  }
}
