void temp(){
  ifstream f2("new4.txt");
  ifstream f1("old4.txt");
  int s1,s2;
  while(f1>>s1){
    cout<<s1<<endl;
    f2>>s2;
    if(s1!=s2){
      cout<<s1<<"\t"<<s2<<endl;
      cin.get();
    }
  }
}
