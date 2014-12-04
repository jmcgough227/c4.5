/**************************************/
// Jason McGough
// AI Project 2: Decision Trees
// wrapper.cpp
/**************************************/

#include <iostream>
#include <string>

using namespace std;

//"Build a tree|Do something|Load a tree"

int menu_select(string title, string selections)
{
  int count =1;
  int selection;
  
  cout << "=*=*=*=*=*=*=" << title << "=*=*=*=*=*=*=" << endl << endl;
  cout << "Please make a selection..." << endl;
  cout << "[1] ";

  for(char c : items)
  {
    if(c == '|') cout << "\n[" << count << "] ";
    else cout << c;
  }

  cout << endl;

  do
  {
    cin >> selection;
  } while(selection < 1 || selection > count);

  return selection;
}

int main(int argc, int* argv[])
{
  
  
  return 0;
}
