#include<iostream>
#include<sstream>
#include<fstream>
using namespace std;
#include <iomanip.h>
#include <sys/stat.h>
#include "plot.cxx"
#include "setup.hxx"



int main(int argc,char *argv[]){
  Int_t run_number_1,run_number_2;
  int c=-1;

  char buff1[400];


  while ((c = getopt(argc, argv, "r:s:")) != -1) {
    switch(c){
    case 'r':
      run_number_1=atoi(optarg);
      break;
    case 's':
      run_number_2=atoi(optarg);
      break;
    }
  }
  sprintf(buff1,"/home/daq/data/gain_problem/ingrid_%08d_problem_ch.txt",run_number_2);
  ofstream f(buff1);  
  plot fplot(run_number_1,run_number_2);
  if(!fplot.flag)exit(1);

  for(Int_t nummod=0;nummod<NumMod;nummod++){
    for(Int_t numtfb=0;numtfb<UseNumTFB+2;numtfb++){
      for(Int_t numch=0;numch<UseNumCh;numch++){
	if(fplot.flag_gain_error[nummod][numtfb][numch]){
	  f<<nummod<<"\t"<<numtfb<<"\t"<<numch<<endl;
	}
      }//numch
    }//numtfb
  }//nummod

  f.close();

}
