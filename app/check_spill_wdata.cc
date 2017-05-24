#include<iostream>
#include<sstream>
#include<fstream>
#include<vector>
const static int number_of_normal = 87517;
const static int number_of_good   = 86069;
using namespace std;
int main(){

  ifstream finggood  ("/home/daq/data/dst/ingrid_spill_trgsec.txt");
  ifstream fbsdgood  ("/home/daq/data/BeamSummary/check_txt/check_bsd_goodspill_100304.txt");

  vector<int> ingspillnum, bsdspillnum; 
  vector<int> ingtrg_sec, bsdtrg_sec;

  int t1,t2;
  while(finggood>>t1>>t2){
    ingspillnum.push_back(t1);
    ingtrg_sec. push_back(t2);
  }
  while(fbsdgood>>t1){
    bsdspillnum.push_back(t1);
  }
  //### check good
  for(int i=0; i<number_of_good; i++){
    bool flag=false;
    for(int j=0; j<ingspillnum.size(); j++){
      if( ingspillnum[j] == bsdspillnum[i])
	flag=true;
    }
    if(!flag){
      cout << bsdspillnum[i] << endl;
    }
  }



}
