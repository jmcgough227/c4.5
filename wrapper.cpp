/**************************************/
// Jason McGough
// AI Project 2: Decision Trees
// wrapper.cpp
/**************************************/

#include <iostream>
#include <string>

using namespace std;


int menu_select(string menu_title, string selections)
{
  int count = 1;
  int select;
  
  cout << "=*=*=*=*=*=*=" << menu_title << "=*=*=*=*=*=*=" << endl << endl;
  cout << "Please make a selection..." << endl;
  cout << "[1] ";

  for(char c : selections)
  {
    if(c == '|') cout << "\n[" << ++count << "] ";
    else cout << c;
  }

  cout << endl;

  do
  {
    cin >> select;
  } while(select < 1 || select > count);

  return select;
}

int main(int argc, char* argv[])
{
  string menu_title = "AI Project 2: Decision Trees";
  string selections = "Learn/Load a decision tree|"
                      "Test Decision tree accuracy|"
		      "Apply decision tree to new cases|"
		      "Quit";

  string submenu_title = "Apply decision tree to new cases";
  string submenu_selections = "Enter a new case interactively|"
                              "Quit";
  
  bool program_loop = true;
  bool submenu_loop;
  int select;

  while(program_loop)
  {
    select = menu_select(menu_title, selections);

    switch(select)
    {
      case 1:
      {
        // Learn/load tree
        break;
      }

      case 2:
      {
        // Test tree accuracy
        break;
      }

      case 3:
      {
        submenu_loop = true;

        while(submenu_loop)
	{
          select =  menu_select(submenu_title, submenu_selections);
            // Enter cases interactively
            // Quit
          break;
	}
      }

      case 4: program_loop = false;
    }
  }

  return 0;
}
