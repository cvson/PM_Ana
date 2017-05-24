#include<iostream>
#include<sstream>
#include<fstream>
#include<unistd.h>
using namespace std;

#include"INGRID_Dimension.cxx"

int main(int argc,char *argv[]){

  INGRID_Dimension *fDimension = new INGRID_Dimension();
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
  
  double posxy,posz;
  bool flag = fDimension->get_pos(mod,plane,ch,tpl,veto,&posxy,&posz);
  if(flag){
    cout<<"mod:"<<mod;
    if(tpl)cout<<" tpl:";
    if(veto)cout<<" veto:";
    cout<<plane<<" ch:"<<ch<<" posxy:"<<posxy<<" posz:"<<posz<<endl;
  }

  //fDimension->get_expch(0,0,&mod,0,0,0,0);  
}
