#include<TApplication.h>
#include<TGFrame.h>
#include<TGClient.h>
#include<TGButton.h>
#include<TGButtonGroup.h>
#include<TCanvas.h>
#include<TRootEmbeddedCanvas.h>
#include<TGTextEntry.h>
#include<TGComboBox.h>
#include<TGLabel.h>
#include<TF1.h>
#include<TH1F.h>
#include<TThread.h>
#include<iostream>
using namespace std; 


class GroupBox : public TGGroupFrame{
private:
  TGComboBox*  fCombo;
  TGTextEntry* fEntry1;
  TGTextEntry* fEntry2;
  TGTextEntry* fEntry3;
public:
  GroupBox(const TGWindow* p, const char* name, const char* title);
  TGTextEntry* GetEntry() const{return fEntry1;}
  TGComboBox*  GetCombo() const{return fCombo;}
  TString       GetText1()  const{return fEntry1->GetDisplayText();}
  TString       GetText2()  const{return fEntry2->GetDisplayText();}
  TString       GetText3()  const{return fEntry3->GetDisplayText();}
};
GroupBox::GroupBox(const TGWindow* p, const char* name, const char* title):
  TGGroupFrame(p, name)
{
   TGHorizontalFrame *horz = new TGHorizontalFrame(this);
   AddFrame(horz, new TGLayoutHints(kLHintsExpandX | kLHintsCenterY));
   TGLabel *label = new TGLabel(horz, title);
   horz->AddFrame(label, new TGLayoutHints(kLHintsLeft | kLHintsCenterY));

   fCombo = new TGComboBox(horz);
   horz->AddFrame(fCombo, new TGLayoutHints(kLHintsRight | kLHintsExpandY,
                                            5, 0, 5, 5));
   fCombo->Resize(100, 20);

   fEntry1 = new TGTextEntry(this);
   AddFrame(fEntry1, new TGLayoutHints(kLHintsExpandX | kLHintsCenterY, 1,1,1,1));
   fEntry2 = new TGTextEntry(this);
   AddFrame(fEntry2, new TGLayoutHints(kLHintsExpandX | kLHintsCenterY));
   fEntry3 = new TGTextEntry(this);
   AddFrame(fEntry3, new TGLayoutHints(kLHintsExpandX | kLHintsCenterY));
}


//########### mybutton class ####################
//###############################################
class mybutton : public TGTextButton, public TThread {
private:
  void (*fFunc)(void *);
  void * fData;

public:
  mybutton(TGFrame* frame, 
	   Char_t*   text, 
	   Int_t       id, 
	   Int_t   txsize = 34) :
    TGTextButton(frame, text, id),
    fFunc(NULL),
    fData(NULL) 
  {
    TString font;
    font.Form("-adobe-courier-bold-r-*-*-%d-*-iso8859-1",txsize);
    SetFont(font);
  };

  void SetFunction(void (*func)(void *)) {
    fFunc = func;
  };



  void SetData(void * data) {
    fData = data;
  };

  virtual void Pressed() {
    TGTextButton::Pressed();
    cout << GetString() << " Pressed" << endl;
    if (fFunc)
      fFunc(fData);
  };
};

class Adc{
public:
  TH1F *highadc;
  Adc()  
  {
    highadc = new TH1F("highadc","highadc",10,0,10);
  }
  ~Adc(){
    highadc->Clear();
    delete highadc;
  }
  void Print(){
    cout<<"test printing adc..."<<endl;
  }
};
//############ Main Frame for Online plots #######################
//################################################################
class SampleMainFrame : public TTimer{
 private:
  TGMainFrame              mf;
  TGHorizontalFrame  fButtons;
  TRootEmbeddedCanvas      c1;
  TRootEmbeddedCanvas      c2;
  TGButtonGroup  fButtonGroup;
  mybutton                mb1;
  mybutton                mb2;
  mybutton                mb3;
  mybutton                mb4;
  TH1F                     h1;




public:
  GroupBox*             fEcho; 
  Adc                    fAdc;

  SampleMainFrame( const TGWindow *p = NULL,
		   UInt_t w          = 800,
		   UInt_t h          = 600 );
  virtual ~SampleMainFrame();
  void SetButtons(Bool_t);

  //### Draw Function ######
  void Drawhighadc(TCanvas *c){
    c->cd();
    fAdc.highadc->Draw();
    c->Update();
  }
  void Print(){
    cout<<"test printing..."<<endl;
  }
  void Wait(){
    Start(1000 ,1);
  };

};


//############# local function for my button ###################
//##############################################################
//void drawfunc1(void * arg){
void drawfunc1(void * arg){
  SampleMainFrame *smf      = (SampleMainFrame*) arg;
  TRootEmbeddedCanvas *emb = (TRootEmbeddedCanvas*)gClient->GetWindowByName("IngShiftCanvas1");
  TCanvas* c          = emb->GetCanvas();
  TGMainFrame*    mf = (TGMainFrame*)gClient->GetWindowByName("ingrid");

  c->cd();
  smf->fAdc.Print();
  smf->Drawhighadc(c);
  cout<<(smf->fEcho->GetText1())<<endl;  
  cout<<(smf->fEcho->GetText2())<<endl;  
  cout<<(smf->fEcho->GetText3())<<endl;  
  cout<<(smf->fEcho->GetCombo())<<endl;  

  TGComboBox* a  = smf->fEcho->GetCombo();
  TGLBEntry *ent = a->GetSelectedEntry();
  if(!ent) return;
  TString name = ent->GetTitle();
  cout<<name<<endl;
}
void drawfunc2(void *){
  TRootEmbeddedCanvas *emb =(TRootEmbeddedCanvas*)gClient->GetWindowByName("IngShiftCanvas1");
  TCanvas*c          = emb->GetCanvas();
  c->cd();
  TGMainFrame*    mf = (TGMainFrame*)gClient->GetWindowByName("ingrid");
  TF1*             f = new TF1("pol1","-x",0,10);
  f->Draw();
  c->Update();
  f->Clear();
  delete f;  
}
void drawfunc3(void *){
  TRootEmbeddedCanvas *emb =(TRootEmbeddedCanvas*)gClient->GetWindowByName("IngShiftCanvas2");
  TCanvas*c          = emb->GetCanvas();
  c->cd();
  TGMainFrame*    mf = (TGMainFrame*)gClient->GetWindowByName("ingrid");
  TF1*             f = new TF1("pol1","x",0,10);
  f->Draw();
  c->Update();
  f->Clear();
  delete f;  
}
void drawfunc4(void *){
  TRootEmbeddedCanvas *emb =(TRootEmbeddedCanvas*)gClient->GetWindowByName("IngShiftCanvas2");
  TCanvas*c          = emb->GetCanvas();
  c->cd();
  TGMainFrame*    mf = (TGMainFrame*)gClient->GetWindowByName("ingrid");
  TF1*             f = new TF1("pol1","-x",0,10);
  f->Draw();
  c->Update();
  f->Clear();
  delete f;  
}



SampleMainFrame::SampleMainFrame( const TGWindow *p, UInt_t w, UInt_t h )
  :mf(gClient->GetRoot(),200,210,kVerticalFrame),
   h1("h1","h1",100,0,100),
   fButtons(&mf, 100, 40),
   fButtonGroup(&fButtons,"Screen Shot",kHorizontalFrame),
   c1("ingrid1",&mf,400,400),

   c2("ingrid1",&mf,400,400),
   mb1(&fButtonGroup, "mb1",1,6),
   mb2(&fButtonGroup, "mb2",1,6),
   mb3(&fButtonGroup, "mb3",1,6),
   mb4(&fButtonGroup, "mb4",1,6),
   fAdc()
{
  mf. SetName("ingrid");
  c1. SetName("IngShiftCanvas1");
  c2. SetName("IngShiftCanvas2");


  TGComboBox  *combo;
  TGTextEntry *entry; 
  fEcho = new GroupBox(&mf, "Echo", "Mod, view, pln");
  mf.AddFrame(fEcho, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5));
  combo = fEcho->GetCombo();
  combo ->AddEntry("Adc", TGTextEntry::kNormal);
  combo ->AddEntry("Tdc", TGTextEntry::kNormal);
  entry = fEcho->GetEntry();
  cout<<entry<<endl;  
  //#### set function to mybutton #######
  //#####################################
  mb1      .SetFunction(drawfunc1);
  mb1      .SetData(this);
  mb2      .SetFunction(drawfunc2);
  mb3      .SetFunction(drawfunc3);
  mb4      .SetFunction(drawfunc4);
  fButtons .AddFrame(&fButtonGroup);
  mf.AddFrame(&fButtons);



  mf.AddFrame(&c1, new TGLayoutHints(kLHintsRight | kLHintsExpandX|
  			     kLHintsExpandY,2,10,2,2));  
  mf.AddFrame(&c2, new TGLayoutHints(kLHintsLeft  | kLHintsExpandX|
  				     kLHintsExpandY,2,10,2,2));  
  mf.MapSubwindows();
  mf.Resize( );
  mf.MapWindow();
}
   
SampleMainFrame::~SampleMainFrame()
{
  mf.Cleanup();
}

void ProcessData(void * arg){
  SampleMainFrame *mf = (SampleMainFrame*) arg;
  TF1 *gaus = new TF1("gaus","gaus",0,10);
  gaus->SetParameters(10,5,2);
  while(1){
    double a = gaus->GetRandom();
    sleep(1);
    mf->fAdc.highadc->Fill(a);
    mf->Wait();
  }
}

   

int main( int argc, char **argv )
{
  TApplication theApp( "app", &argc, argv );
  SampleMainFrame win( gClient->GetRoot() );
  TThread *th = new TThread("mythread", &ProcessData, &win);
  th->Run();

  theApp.Run();
  return 0;
}
