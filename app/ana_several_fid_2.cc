void ana_several_fid_2(){
  gStyle -> SetOptStat(0);
  const int      np          = 5;
  const int      nd          = 5;
  char*          name[np]    = {"29_30", "31", "32", "33_1", "33_2"};
  char*          fid [nd]    = 
    {"nominal", "upstrm", "downstrm", "internal 1", "internal 2"};

  TH1F*          fHcenter[np];
  TH1F*          fHsigma [np];
  TF1*           fHcenter_mean[np];
  ifstream rf;

  TH1F*          fHcenterdiff [np];
  TH1F*          fHcenterdiffr[np];
  TH1F*          fHsigmadiff [np];
  TH1F*          fHsigmadiffr[np];


  float center[np][nd], center_error[np][nd], 
    sigma[np][nd], sigma_error[np][nd];
  for(int ip=0; ip<np; ip++){
    fHcenter[ip] = new TH1F( Form("center%d",ip),
			     Form("center%d",ip),
			     500,0,5
			     );
    fHcenter[ip] ->SetLineColor(ip+1);
    fHcenter[ip] ->SetLineWidth(3);
    fHcenter[ip] ->SetMaximum(50);
    fHcenter[ip] ->SetMinimum(-50);
    fHcenterdiff[ip] = new TH1F( Form("centerdiff%d",ip),
				 Form("centerdiff%d",ip),
				 500,0,5
				 );
    fHcenterdiff[ip] ->SetLineColor(ip+1);
    fHcenterdiff[ip] ->SetLineWidth(3);


    fHsigma[ip] = new TH1F( Form("sigma%d",ip),
			    Form("sigma%d",ip),
			    500,0,5
			    );
    fHsigma[ip] ->SetLineColor(ip+1);
    fHsigma[ip] ->SetLineWidth(3);
    fHsigma[ip] ->SetMaximum(50);
    fHsigma[ip] ->SetMinimum(-50);
    fHsigmadiff[ip] = new TH1F( Form("sigmadiff%d",ip),
				Form("sigmadiff%d",ip),
				500,0,5
				);
    fHsigmadiff[ip] ->SetLineColor(ip+1);
    fHsigmadiff[ip] ->SetLineWidth(3);

    fHcenter_mean[ip] = new TF1(Form("fcenter%d",ip),"pol0");
    fHcenter_mean[ip] ->SetLineStyle(2);
    fHcenter_mean[ip] ->SetLineWidth(2);
    fHcenter_mean[ip] ->SetLineColor(ip+1);

    rf.open( Form("%s_vfitres.txt", name[ip]) );
    for(int id=0; id<nd; id++){
      rf >> center[ip][id]      
	 >> center_error[ip][id]
	 >> sigma[ip][id]       
	 >> sigma_error[ip][id];

      fHcenter[ip] -> SetBinContent(2+id*100+12*ip, center[ip][id]);
      fHcenter[ip] -> SetBinError  (2+id*100+12*ip, center_error[ip][id]);
      fHcenterdiff[ip] -> SetBinContent(2+id*100+12*ip, center[ip][id]-center[ip][0]);
      float e = sqrt( center_error[ip][id]**2 + center_error[ip][0]**2 );
      fHcenterdiff[ip] -> SetBinError  (2+id*100+12*ip, e);
      if(id==0){
	fHcenterdiff[ip] -> SetBinContent(2+id*100+12*ip,0);
	fHcenterdiff[ip] -> SetBinError  (2+id*100+12*ip,0);
      }

      fHsigma[ip] -> SetBinContent(2+id*100+12*ip, sigma[ip][id]);
      fHsigma[ip] -> SetBinError  (2+id*100+12*ip, sigma_error[ip][id]);
      if(id==0){
	fHsigmadiff[ip] -> SetBinContent(2+id*100+12*ip,0);
	fHsigmadiff[ip] -> SetBinError  (2+id*100+12*ip,0);
      }
      else{
	fHsigmadiff[ip] -> SetBinContent(2+id*100+12*ip, sigma[ip][id]-sigma[ip][0]);
	e = sqrt( sigma_error[ip][id]**2 + sigma_error[ip][0]**2 );
	fHsigmadiff[ip] -> SetBinError  (2+id*100+12*ip, e);
 
      }

    }
    fHcenter[ip] -> Fit(Form("fcenter%d",ip),"qn","",0,nd);
    rf.close();
  }


  TH1F*    frame = new TH1F("frame","frame",5,0,5);
  TLegend* leg   = new TLegend(0.75, 0.75, 0.95, 0.95);
  for(int i=0; i<5; i++)frame->Fill(fid[i],-500);

  frame -> SetMaximum(30);
  frame -> SetMinimum(-30);
  frame -> SetYTitle ("Y center[cm]");
  frame -> SetXTitle ("fiducial");
  frame -> Draw      ();
  leg   -> SetFillColor(0);
  for(int id=0; id < nd; id++){
    fHcenter[id] -> Draw("same");
    fHcenter_mean[id] -> Draw("same");
    leg -> AddEntry(fHcenter[id],Form("Run%s",name[id]),"l");
  }
  leg -> Draw();
  c1  -> Update();
  c1  -> Print("100609_YCenter_Several_fid.pdf");
  cin.get();

  c1 -> Clear();
  leg-> Clear();
  frame -> SetYTitle ("difference to nominal[cm]");
  frame -> Draw();
  for(int id=0; id < nd; id++){
    fHcenterdiff[id] -> Draw("same");
    leg -> AddEntry(fHcenterdiff[id],Form("Run%s",name[id]),"l");
  }
  leg -> Draw();
  c1  -> Update();
  c1  -> Print("100609_YCenterDiff_Several_fid.pdf");
  cin.get();


  c1    -> Clear();
  leg   -> Clear();
  frame -> SetMaximum(300);
  frame -> SetMinimum(600);
  frame -> SetYTitle ("Y width(sigma)[cm]");
  frame -> SetXTitle ("fiducial");
  frame -> Draw      ();
  for(int id=0; id < nd; id++){
    fHsigma[id] -> Draw("same");
    leg -> AddEntry(fHsigma[id],Form("Run%s",name[id]),"l");
  }
  leg -> Draw();
  c1  -> Update();
  c1  -> Print("100609_YSigma_Several_fid.pdf");
  cin.get();

  frame -> SetMaximum(50);
  frame -> SetMinimum(-50);
  c1 -> Clear();
  leg-> Clear();
  frame -> SetYTitle ("difference to nominal[cm]");
  frame -> Draw();
  for(int id=0; id < nd; id++){
    fHsigmadiff[id] -> Draw("same");
    leg -> AddEntry(fHsigmadiff[id],Form("Run%s",name[id]),"l");
  }
  leg -> Draw();
  c1  -> Update();
  c1  -> Print("100609_YSigmaDiff_Several_fid.pdf");
  cin.get();



}
