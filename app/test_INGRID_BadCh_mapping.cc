#include<iostream>
#include<sstream>
#include<fstream>
#include<unistd.h>
using namespace std;

#include"INGRID_BadCh_mapping.cxx"

int main(int argc,char *argv[]){

  INGRID_BadCh_mapping *fBadCh_mapping = new INGRID_BadCh_mapping();
  
  int mod;
  int plane;
  int ch; 
  bool tpl=false;
  bool veto=false;


  mod = atoi(argv[1]);
  if(atoi(argv[2])==0)tpl=true;
  if(atoi(argv[2])==1)veto=true;
  plane = atoi(argv[3]);
  ch = atoi(argv[4]);
  
  bool flag = fBadCh_mapping->badchannel(&mod,&plane,&ch,&tpl,&veto);
  if(flag){
    cout<<"mod:"<<mod;
    if(tpl)cout<<" tpl:";
    if(veto)cout<<" veto:";
    cout<<plane<<" ch:"<<ch<<" bad channel"<<endl;
  }
  else{
    cout<<"mod:"<<mod;
    if(tpl)cout<<" tpl:";
    if(veto)cout<<" veto:";
    cout<<plane<<" ch:"<<ch<<" good channel"<<endl;

  }

}
