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

  mod = atoi(argv[1]);
  plane = atoi(argv[2]);

  
  bool flag = fCh_config->active_veto(&mod,&plane);
  if(flag){
    cout<<"mod:"<<mod;
    cout<<plane<<" active"<<endl;
  }
  else{
    cout<<"mod:"<<mod;
    cout<<plane<<" not active"<<endl;
  }

}
