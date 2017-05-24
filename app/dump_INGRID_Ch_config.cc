#include<iostream>
#include<sstream>
#include<fstream>
#include<unistd.h>
using namespace std;

#include"INGRID_Ch_config.cxx"

int main(int argc,char *argv[]){

  INGRID_Ch_config *fCh_config = new INGRID_Ch_config();
  
  int rmm;
  int tfb;
  int trip;
  int trip_ch;
  int mod;
  int plane;

  int ch; 
  bool tpl;
  bool veto;


  rmm = atoi(argv[1]);
  tfb = atoi(argv[2]);
  trip = atoi(argv[3]);
  trip_ch = atoi(argv[4]);
  //mod = atoi(argv[5]);
  //tpl = atoi(argv[6]);
  //veto = atoi(argv[7]);
  //ch = atoi(argv[6]);

  
  bool flag = fCh_config->channel_configuration(&rmm,&tfb,&trip,&trip_ch,&mod,&plane,&ch,&tpl,&veto);

  if(flag){
  cout<<" RMM:"<<rmm;
  cout<<" TFB:"<<tfb;
  cout<<" TRIP:"<<trip;
  cout<<" Ch:"<<trip_ch<<"\t";
  cout<<" Mod:"<<mod;
  if(tpl)cout<<" tpl:"<<plane;
  if(veto)cout<<" veto:"<<plane;
  cout<<" ch:"<<ch<<endl;
  }


}
