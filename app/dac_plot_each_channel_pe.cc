#include<iostream>
#include<sstream>
#include<fstream>
using namespace std;
#include <iomanip.h>
#include <sys/stat.h>

#include <TROOT.h>
#include <TStyle.h>
#include <TApplication.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TEventList.h>
#include <TBranch.h>
#include <TH1.h>
#include <TGraph.h>
#include <TGaxis.h>
#include <TMarker.h>
#include <TText.h>
#include <TSpectrum.h>
#include <TF1.h>
#include <TMath.h>
#include <TSpectrum.h>

#include "TApplication.h"

//experimental setup
#include "setup.hxx"

int main(int argc,char *argv[]){
  TROOT root("GUI","GUI");
  TApplication theApp("App",&argc,argv);
  struct stat sta;
  gStyle->SetOptFit(1);
  gStyle->SetNdivisions(011,"X");
  gStyle->SetNdivisions(011,"Y");
  gStyle->SetTitleOffset(1.0,"X");
  gStyle->SetTitleOffset(1.0,"Y");
  gStyle->SetLabelSize(0.06,"X");
  gStyle->SetLabelSize(0.06,"Y");
  gStyle->SetTitleSize(0.06,"X");
  gStyle->SetTitleSize(0.06,"Y");
  gStyle->SetTitleYOffset(1.2);
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetPadGridX(1);
  gStyle->SetPadGridY(1);
  gStyle->SetCanvasColor(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadColor(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetStatColor(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetTitleFillColor(0);
  Int_t temp;
  char buff1[300];

  //read setup  
  sprintf(buff1,"%s/log.txt",dac_calib_folder);
  ifstream calibdata_(buff1);
  Int_t total_number=0;;
  while(!calibdata_.eof()){
    calibdata_>>temp>>temp;
    total_number++;
  }
  total_number--;
  calibdata_.close();
  Double_t dac[total_number];
  Double_t pe_thre[NumTPL][NumCh][total_number];

  ifstream calibdata(buff1);
  for(Int_t i=0;i<total_number;i++){
    calibdata>>temp>>dac[i];
    sprintf(buff1,"%s/ingrid_%08d_result.txt",dac_calib_folder,temp);
    ifstream result_file(buff1);
    for(Int_t j=0;j<NumTPL;j++){
      for(Int_t k=0;k<NumCh;k++){
	result_file>>pe_thre[j][k][i];
      }//k
    }//j
    result_file.close();
  } 
  calibdata.close();

  //create graph
  TCanvas *c1 = new TCanvas("c1","c1",10,10,600,600);
  sprintf(buff1,"%s/result.root",dac_calib_folder);
  TFile *f = new TFile(buff1,"recreate");
  TGraph *g[NumTPL][NumCh];
  char graph_name[NumTPL][NumCh][200];

  Double_t a[NumTPL][NumCh],b[NumTPL][NumCh];
  TH1F *a_hist= new TH1F("a_hist","a_hist",40,-0.05,-0);
  TH1F *b_hist= new TH1F("a_hist","a_hist",40,2,8);
  TH1F *a_hist1[NumTPL];
  TH1F *b_hist1[NumTPL];
  TH1F *a_hist2[NumTPL*3];
  TH1F *b_hist2[NumTPL*3];

  TH1F *a_ver = new TH1F("a_ver","a_ver",30,-0.05,0);
  TH1F *b_ver = new TH1F("b_ver","b_ver",30,5,10);
  for(Int_t i=0;i<NumTPL;i++){
    sprintf(buff1,"a_hist%d",i+1);
    a_hist1[i]= new TH1F(buff1,buff1,40,-0.05,-0);
    a_hist1[i]->SetLineColor(i+1);
    sprintf(buff1,"b_hist%d",i+1);
    b_hist1[i]= new TH1F(buff1,buff1,40,2,8);
    b_hist1[i]->SetLineColor(i+1);

    sprintf(buff1,"a_%d",i*3);
    a_hist2[i*3]= new TH1F(buff1,buff1,40,-0.05,-0);
    sprintf(buff1,"a_%d",i*3+1);
    a_hist2[i*3+1]= new TH1F(buff1,buff1,40,-0.05,-0);
    sprintf(buff1,"a_%d",i*3+2);
    a_hist2[i*3+2]= new TH1F(buff1,buff1,40,-0.05,-0);

    sprintf(buff1,"b_hist2%d",i*3);
    b_hist2[i*3]= new TH1F(buff1,buff1,40,2,8);
    sprintf(buff1,"b_hist2%d",i*3+1);
    b_hist2[i*3+1]= new TH1F(buff1,buff1,40,2,8);
    sprintf(buff1,"b_hist2%d",i*3+2);
    b_hist2[i*3+2]= new TH1F(buff1,buff1,40,2,8);

    Double_t a_all=0;
    Double_t b_all=0;
    for(Int_t j=0;j<NumCh;j++){

      TF1 *pol1 = new TF1("pol1","pol1",100,400);
      sprintf(graph_name[i][j],"adc_%02d_%02d",i+1,j+1);
      g[i][j]=new TGraph(total_number,dac,pe_thre[i][j]);
      g[i][j]->SetName(graph_name[i][j]);
      g[i][j]->SetMarkerStyle(8);
      g[i][j]->SetMarkerSize(1.2);
      g[i][j]->Write();
      g[i][j]->Fit("pol1","q","",0,300);


      c1->Clear();
      g[i][j]->Draw("AP");
      c1->Update();
      cin>>temp;

      a[i][j]=pol1->GetParameter(1);
      b[i][j]=pol1->GetParameter(0);
      a_hist1[i]->Fill(a[i][j]);
      b_hist1[i]->Fill(b[i][j]);
      a_hist->Fill(a[i][j]);
      b_hist->Fill(b[i][j]);
      a_all=a_all+a[i][j];
      b_all=b_all+b[i][j];
      if(j==15||j==31||j==47){
	a_all=1.0*a_all/16;
	b_all=1.0*b_all/16;
	a_ver->Fill(a_all);
	b_ver->Fill(b_all);
	a_all=0;
	b_all=0;
      }
      if(j<16){
	a_hist2[i*3]->Fill(a[i][j]);
	b_hist2[i*3]->Fill(b[i][j]);
      }
      if(16<=j&&j<32){
	a_hist2[i*3+1]->Fill(a[i][j]);
	b_hist2[i*3+1]->Fill(b[i][j]);
      }
      if(32<=j&&j<48){
	a_hist2[i*3+2]->Fill(a[i][j]);
	b_hist2[i*3+2]->Fill(b[i][j]);
      }
      //cout<<"ADC thre=\t"<<a[i][j]<<"*DAC + "<<b[i][j]<<"\t"<<1.0*(250-b[i][j])/a[i][j]<<endl; 
    }
  }


  /*
  for(Int_t i=0;i<NumTPL*3;i++){
    cout<<"ok"<<endl;
    //TH1F *frame2 = gPad->DrawFrame(-0.05,0,0,40);
    a_hist2[i]->Draw();
    cout<<"TPL "<<i+1<<"\t"<<a_hist1[i]->GetMean()<<endl;;
    c1->Update();
    a_ver->Fill(a_hist2[i]->GetMean());
    cin>>temp;
  }
  */
  a_ver->Draw();
  c1->Update();
  cin>>temp;
  b_ver->Draw();
  c1->Update();
  cin>>temp;


  for(Int_t i=0;i<NumTPL;i++){
    TH1F *frame3 = gPad->DrawFrame(-0.05,0,0,40);
    a_hist1[i]->Draw();
    cout<<"TPL "<<i+1<<"\t"<<a_hist1[i]->GetMean()<<endl;;
    c1->Update();
    cin>>temp;
   
  }
  c1->Update();
  cin>>temp;
  TH1F *frame4 = gPad->DrawFrame(2,0,8,40);
  for(Int_t i=0;i<NumTPL;i++){
    b_hist1[i]->Draw();
    c1->Update();
    cin>>temp;
  
   
  }
  c1->Update();
  cin>>temp;


  a_hist->SetXTitle("slope");
  a_hist->Write();
  c1->Clear();
  a_hist->Draw();
  c1->Update();
  cin>>temp;
  b_hist->SetXTitle("constant");
  b_hist->Write();
  c1->Clear();
  b_hist->Draw();
  c1->Update();
  cin>>temp;
 
 

  /*
  for(Int_t i=0;i<NumTPL;i++){
    for(Int_t j=0;j<NumCh;j++){
      TH1F *frame = gPad->DrawFrame(0,0,300,10);
      frame->SetXTitle("DAC");
      frame->SetYTitle("threshold[p.e.]");
      frame->Draw();

      //g[i]->SetMarkerColor(i+1);
      g[i][j]->Draw("P");
      sprintf(buff1,"thredshold_each_tfb_%02d_%02d.pdf",i+1,j+1);
      c1->Update();
      c1->SaveAs(buff1);
      cin>>temp;
      c1->Clear();
      //frame->Delete();
    }//j
  }//i
  */


  for(Int_t num=0;num<30;num++){
    Double_t target_DAC=250-5*num;
    TH1F *h = new TH1F("h","h",120,-1,6);
    TH1F *h1[NumTPL];
    TH1F *frame1 = gPad->DrawFrame(-1,0,5,40);
    for(Int_t i=0;i<NumTPL;i++){
      sprintf(buff1,"h%d",i+1);
      h1[i] = new TH1F(buff1,buff1,120,-1,6);
      for(Int_t j=0;j<NumCh;j++){
	Double_t target_pe=a[i][j]*target_DAC+b[i][j];
	h->Fill(target_pe);


	h1[i]->Fill(target_pe);
      }
      h1[i]->SetLineWidth(2);
      h1[i]->SetLineColor(i+1);
      h1[i]->Draw("same");
    }
    c1->Update();
    cin>>temp;

    sprintf(buff1,"thre.[p.e.](DAC=%d)",250-5*num);
    h->SetXTitle(buff1);
    TH1F *frame = gPad->DrawFrame(-1,0,5,200);
    frame->SetXTitle(buff1);
    h->Draw("sames");
    //h->Draw();
    c1->Update();
    c1->Print("Each_threshold.pdf");
    cin>>temp;
    delete h;
    delete frame;
  }

  f->Write();
  f->Close();


}
