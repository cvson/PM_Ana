#include<TApplication.h>
#include<TGFrame.h>
#include<TGClient.h>
#include<TGButton.h>
#include<TGButtonGroup.h>
#include<TCanvas.h>
//#include<TRootEmbeddedCanvas.h>
#include<TF1.h>
#include<TH1F.h>
#include<TThread.h>
#include<iostream>
using namespace std; 


class test{
private:
  int v;
public:
  test(){v=0;};
  ~test(){};
  static void set_v(){v=1;}
  static void Print(void * arg){
    int *a = (int*) arg;
    cout<<(*a)<<endl;
    set_v();
  }

};

void Canvas(void * arg){
  TH1F* h1 = (TH1F*) arg;
  while(1){
    h1->Fill(5);
    sleep(1);
    //TThread::Wait(1);
  }
}

int main(int argc, char **argv){
  TApplication theApp("app", 0, 0);
  TH1F *h1 = new TH1F("h1","h1",10,0,10);

  test t;
  int a=1;
  t.Print(&a);
  TThread *th2 = new TThread("mythread2", &(test::Print), &a);

  /*
  TThread *th = new TThread("mythread", &Canvas, h1);
  th->Run();
  TCanvas *c1 = new TCanvas("c1","c1",10,10,400,400);
  while(1){
    c1->cd();
    h1->Draw();
    c1->Update();
    cin.get();
  }
  */
  theApp.Run();
  return 0;
}

