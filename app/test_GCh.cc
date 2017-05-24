#include<iostream>
#include<sstream>
#include<fstream>
#include<unistd.h>
using namespace std;

#include"INGRID_Ch_config.cxx"

int main(int argc,char *argv[]){

  INGRID_Ch_config *func;
  
  int gch;
  for(int mod=0; mod<14; mod++){
    for(int view=0; view<2; view++){
      for(int pln=0; pln<15; pln++){
	for(int ch=0; ch<24; ch++){
	  if( func->get_global_ch(mod, view, pln, ch, &gch) ){
	    cout<<"Mod :"    <<mod
		<<"\tView:"  <<view
		<<"\tPlane:" <<pln
		<<"\tCh.:"   <<ch
		<<"\t\tGch:" <<gch
		<<endl;

	  }

	}//ch
      }//pln
    }//view
  }//module

}
