#include<iostream>
#include<sstream>
#include<fstream>
#include<vector>
const static int number_of_normal = 87517;
const static int number_of_good   = 86980;//86069;
using namespace std;
int main(){
  ifstream fingnormal("/home/daq/INGRID_analysis/app/check_ingrid_normal.txt");
  //ifstream finggood  ("/home/daq/INGRID_analysis/app/check_ingrid_good.txt");
  ifstream finggood  ("/home/daq/data/dst/ingrid_spill_trgsec.txt");
  ifstream fbsdnormal;
  //ifstream fbsdgood  ("/home/daq/data/BeamSummary/check_txt/check_bsd_goodspill_100304.txt");
  ifstream fbsdgood  ("/home/daq/INGRID_analysis/app/check_ingrid_good.txt");
 

  int ingspillnum, bsdspillnum; 
  int ingtrg_sec, bsdtrg_sec;

  //### check normal
  /*
  for(int i=0; i<number_of_normal; i++){
    fingnormal >> ingspillnum >> ingtrg_sec;
   
    fbsdnormal.open("/home/daq/data/BeamSummary/check_txt/check_bsd_normal.txt");
    bool flag=false;
    for(int i=0; i<number_of_normal; i++){
      fbsdnormal >> bsdspillnum >> bsdtrg_sec;
      if( ingspillnum == bsdspillnum &&
	  ingtrg_sec  == bsdtrg_sec   )
	{
	flag = true;
	}
    }
    fbsdnormal.close();
    if(!flag){
      cout << ingspillnum << "\tcaution" << endl;
    }
  }
  */

  //### check good #########################
  //########################################
  //### read ingrid data
  vector< pair<int, int> >  ing_data; // < spill#, time >
  vector<double>                ctnp;
  int a, b;
  double e;
  while( finggood >> a >> b >> e ){
    pair<int, int> t;
    t.first  = a;
    t.second = b;
    ing_data.push_back(t);
    ctnp    .push_back(e);
  }
  finggood.close();
  //### read beam summary data
  vector< pair<int, int> >  bsd_data; // < spill#, time >
  while( fbsdgood >> a >> b ){
    pair<int, int> t;
    t.first  = a;
    t.second = b;
    bsd_data.push_back(t);

  }
  fbsdgood.close();
  int count_bad=0;
  for(int i=0; i<bsd_data.size(); i++){
    //if(i%100==0)cout << i << "check." << endl;
    bool flag=false;
    for(int j=0; j<ing_data.size(); j++){
      if( bsd_data[i].first  == ing_data[j].first 
	  // &&bsd_data[i].second == ing_data[j].second 
	  )
	flag = true;
    }
    if(!flag){
      cout << "cauction " 
	   << bsd_data[i].first  <<"\t" 
	   << bsd_data[i].second <<"\t" 
	   << endl;
      count_bad++;
    }
  }
  cout <<"number of missing = " << count_bad << endl;
  ofstream wf("ingrid_spill_ctnp.txt");
  for(int i=0; i<ing_data.size(); i++){
    wf << ing_data[i].first <<"\t"<< ctnp[i] << endl;
  }

  /*
  int count_bad = 0;
  double temp;
  for(int i=0; i<number_of_good; i++){
    if(i%1000==0)cout << "check " << i << "  end" << endl;
    finggood >> ingspillnum >> ingtrg_sec;
   
    //fbsdgood.open("/home/daq/data/BeamSummary/check_txt/check_bsd_good.txt");
    fbsdgood.open("/home/daq/INGRID_analysis/app/check_ingrid_good.txt");
    //fbsdgood.open("/home/daq/data/BeamSummary/check_txt/check_bsd_good.txt");
    bool flag=false;
    for(int i=0; i<number_of_good; i++){
      fbsdgood >> bsdspillnum >> bsdtrg_sec;
      if( ingspillnum == bsdspillnum &&
	  ingtrg_sec  == bsdtrg_sec   )
	{
	flag = true;
	}
    }
    fbsdgood.close();
    if(!flag){
      cout << "\tcaution\t" 
	   << ingspillnum << "\t"
	   << ingtrg_sec  << "\t"
	   << endl;
      count_bad++;
    }
  }
  cout <<"number of missing = " << count_bad << endl;
  */

}
