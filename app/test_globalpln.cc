#include<iostream>
#include<sstream>
#include<fstream>
#include<unistd.h>
using namespace std;

#include"INGRID_Ch_config.cxx"

int main(int argc,char *argv[]){

  INGRID_Ch_config *fCh_config = new INGRID_Ch_config();
  
  int mod;
  int plane;
  int ch;
  int global_ch;
  bool tpl=false;
  bool veto=false;
  mod = atoi(argv[1]);
  plane = atoi(argv[2]);
  ch = atoi(argv[3]);
  if(atoi(argv[4])==0)tpl=true; 
  if(atoi(argv[4])==1)veto=true; 
  bool flag = fCh_config->get_global_ch(&mod,&plane,&ch,&tpl,&veto,&global_ch);
  if(flag){
    cout<<"mod:"<<mod;
    if(tpl)cout<<" tpl:";
    if(veto)cout<<" veto:";
    cout<<plane<<" ch:"<<ch<<" global:"<<global_ch<<endl;
  }
  else{
    cout<<"mod:"<<mod;
    if(tpl)cout<<" tpl:";
    if(veto)cout<<" veto:";
    cout<<plane<<" ch:"<<ch<<" not global"<<endl;
  }

}
