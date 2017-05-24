#include<iostream>
#include<sstream>
#include<fstream>
using namespace std;
#include "/home/daq/INGRID_analysis/inc/setup.hxx"

int main(int argc,char *argv[]){

  int runnumber = atoi(argv[1]);
  char file_name[300];
  sprintf(file_name,"%s/ingrid_%08d_0000.daq.mid.gap.txt",gain_and_pedestal_folder,runnumber);
  ifstream rf(file_name);

  sprintf(file_name,"%s/cpedtest_IC00_otani.txt",gain_and_pedestal_folder,runnumber);
  ofstream wf(file_name);

  int initial=128;
  int c=0;
  int pedestal,gain,temp;
  double Pedestal,Gain,Temp;


  for(int numtrip=0;numtrip<48;numtrip++){
      for(int numsub=0;numsub<4;numsub++){
	wf<<numtrip<<" "<<numsub<<" trip"<<endl;
	for(int numch=0;numch<16;numch++){
	  rf>>temp>>temp>>temp>>Pedestal>>Temp>>Gain;//read
	  pedestal=Pedestal;
	  gain=Gain;
	  if(numtrip<3)cout<<pedestal<<"\t"<<Pedestal<<endl;
	  wf<<initial<<" ";
	  wf<<c<<" ";
	  for(int i=0;i<23;i++){
	    wf<<pedestal<<" ";
	  }
	  wf<<endl;//write
	}//numch
	for(int numch=0;numch<2;numch++){
	  wf<<initial<<" ";
	  wf<<c<<" ";
	  for(int i=0;i<23;i++){
	    wf<<pedestal<<" ";
	  }
	  wf<<endl;//write
	}//numch
      }//numsub
  }//numtrip

  rf.close();


  sprintf(file_name,"%s/ingrid_%08d_0000.daq.mid.gap.txt",gain_and_pedestal_folder,runnumber);
  ifstream rf2(file_name);


  for(int numtrip=0;numtrip<48;numtrip++){
      for(int numsub=0;numsub<4;numsub++){
	wf<<numtrip<<" "<<numsub<<" trip"<<endl;
	for(int numch=0;numch<16;numch++){
	  rf2>>temp>>temp>>temp>>Pedestal>>Temp>>Gain;//read
	  pedestal=Pedestal;
	  gain=Gain;
	  if(numtrip<3)cout<<pedestal<<"\t"<<Pedestal<<endl;
	  wf<<initial<<" ";
	  wf<<c<<" ";
	  for(int i=0;i<23;i++){
	    wf<<pedestal<<" ";
	  }
	  wf<<endl;//write
	}//numch
	for(int numch=0;numch<2;numch++){
	  wf<<initial<<" ";
	  wf<<c<<" ";
	  for(int i=0;i<23;i++){
	    wf<<pedestal<<" ";
	  }
	  wf<<endl;//write
	}//numch
      }//numsub
  }//numtrip
  wf.close();
  rf2.close();
}
