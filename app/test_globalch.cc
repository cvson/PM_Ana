#include<iostream>
#include<sstream>
#include<fstream>
#include<unistd.h>
using namespace std;

#include"INGRID_Ch_config.cxx"

int main(int argc,char *argv[]){

  INGRID_Ch_config *fCh_config = new INGRID_Ch_config();

  int ach[14];
  for(int i=0; i<14;i++)ach[i]=0;
  int temp;
  for(int imod=0; imod<14; imod++ ){
    for(int iview=0; iview<2; iview++ ){
      for(int ipln=0; ipln<15; ipln++ ){
	for(int ich=0; ich<24; ich++ ){
	  if( fCh_config -> get_global_ch(&imod, &iview, &ipln, &ich, &temp) )
	    ach[imod]++;
	  
	}
      }
    }
  }

  for(int i=0; i<14;i++)cout << "module #" << i << "\t"<<ach[i] << endl;
}
