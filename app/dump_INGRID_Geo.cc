#include<iostream>
#include<sstream>
#include<fstream>
#include<unistd.h>
using namespace std;

#include"INGRID_Geometry.cxx"

int main(int argc,char *argv[]){

  INGRID_Geometry *fGeometry = new INGRID_Geometry();
  

  int mod;
  int plane;
  int ch; 
  bool tpl;
  bool veto;

  double posx;
  double posy;
  double posz;

  mod = atoi(argv[1]);
  plane = atoi(argv[2]);
  ch = atoi(argv[3]);
  tpl = atoi(argv[4]);
  veto = atoi(argv[5]);
  
  bool flag = fGeometry->fGet_Geometry(&mod,&plane,&ch,&tpl,&veto,&posx,&posy,&posz);

  if(flag){
    cout<<" Mod:"<<mod;
    cout<<" Plane:"<<plane;
    cout<<" Ch:"<<ch;
    if(tpl)cout<<" tpl:"<<plane;
    if(veto)cout<<" veto:"<<plane;
    cout<<" posx:"<<posx;
    cout<<" posy:"<<posy;
    cout<<" posz:"<<posz<<endl;

  }


}
