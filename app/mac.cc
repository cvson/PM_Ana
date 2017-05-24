void mac(){
  ifstream oldfile("old6278.txt");
  ifstream newfile("new6278.txt");

  int oldspill;
  int newspill;

  while( oldfile >> oldspill ){
    newfile >> newspill;
    if( newspill != oldspill ){
      cout << "old: " << oldspill <<endl
	   << "new: " << newspill <<endl;
      cin.get();
    }
  }
}
