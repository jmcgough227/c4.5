/**************************************/
// Jason McGough
// AI Project 2: Decision Trees
// wrapper.cpp
/**************************************/

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <set>

using namespace std;

vector<string> load_file(string name) {
	ifstream file(name.c_str());
	string line;
	vector<string> lines;
	
	if(!file.is_open()) {
		cout << "Failed to load file!" << endl;
		throw -1;
	}
	
	while(getline(file, line)) {
		lines.push_back(line);
	}
	
	return lines;
}

struct Attribute {
	string name;
	set<string> values;
	bool continuous;
};

struct NameFile {
	vector<Attribute> att;
};

NameFile parse_name_file(string name) {
	vector<string> lines = load_file(name);
	NameFile file;
	
	for(auto &line : lines) {
		string new_line;
		bool line_has_at = false;
		
		for(int i = 0;i < line.length();i++) {
			if(line[i] == '|') {
				break;
			}
			else if(line[i] == ':') {
				line_has_at = true;
			}
			
			new_line += line[i];
		}
		
		line = new_line;
		string at_name;
		
		if(line_has_at) {
			int pos = 0;
			
			while(line[pos] == ' ')
				pos++;
			
			while(line[pos] != ':')
				at_name += line[pos++];
			
			pos++;
			
			while(line[pos] == ' ' || line[pos] == '\t')
				pos++;
				
			string val_name;
			Attribute a;
			a.name = at_name;
			a.continuous = false;
			
			while(line[pos] != '.') {
				while(line[pos] != ',' && line[pos] != '.') {
					val_name += line[pos++];
				}
				
				if(line[pos] == ',')
					pos++;
				
				while(line[pos] == ' ' || line[pos] == '\t')
					pos++;
			
				
				if(val_name == "continuous") {
					a.continuous = true;
					break;
				}
				else {
					a.values.insert(val_name);
				}
				
				val_name = "";
			}
				
			file.att.push_back(a);
		}
	}
	
	return file;
}

struct TreeData {
	NameFile name;
};



//===================================================
// Stores a decision tree node
struct Node{
	Node *child;
	Node *next;

	string attribute;
	string value;
	string result;
	
	bool less_than;
	bool greater_than;
	bool equal;

	Node(){
		child = NULL;
		next = NULL;
	}

	~Node(){
		if(child) delete child;
		if(next) delete next;
	}
};

// A list of relevant lines from c4.5's output
vector<string> lines;
int lines_pos = 0;

Node *tree;


// Gets various pieces of information about a decision tree line
void get_line_info(string &line,int &count,string &attribute,string &value,bool &has_child,
				   string &res, bool &less_than, bool &greater_than, bool &equal_to) {
	
	cout << "Line: " << line << endl;
	count = 0;
	int pos = 0;

	for(int i = 0;i < line.length();i++){
		if(line[i] == '|') count++;

		if(line[i] != '|' && line[i] != ' '){
			pos = i;
			break;
		}
	}

	attribute = "";
	value = "";
	
	equal_to = false;
	less_than = false;
	greater_than = false;

	for(int i = pos;i < line.length();i++){
		if(line[i] == ' '){
			if(line[i+1] != '=' && line[i + 1] != '<' && line[i + 1] != '>'){
				attribute+=line[i];
			}
			else{
				pos = i + 1;
				
				do {
					if(line[pos] == '=')
						equal_to = true;
					else if(line[pos] == '<')
						less_than = true;
					else if(line[pos] == '>')
						greater_than = true;
					else if(line[pos] != ' ')
						break;
					
					pos++;
				} while(pos < line.length());
				
				//pos = i+3;
				break;
			}
		}
		else{
			attribute+=line[i];
		}
	}

	for(;pos < line.length();pos++){
		if(line[pos] != ':'){
			value+=line[pos];
		}
		else{
			break;
		}
	}

	if(pos+1 >= line.length()) has_child = true;
	else has_child = false;

	pos+=2;
	res = "";

	for(;pos < line.length();pos++){
		if(line[pos] == ' '){
			if(line[pos+1] != '('){
				res+=line[pos];
			}
			else{
				break;
			}
		}
		else{
			res+=line[pos];
		}
	}

}

// Adds a node to the decision tree (recursively!)
Node *add_node(int indent){
	Node *start = NULL;
	Node *node;

	do{
		string &line = lines[lines_pos];
		int new_indent;
		string attribute,value;
		string result;
		bool has_child;
		bool less, greater, equal;

		get_line_info(line,new_indent,attribute,value,has_child,result,less,
					  greater, equal);

		if(new_indent < indent) return start;

		Node *new_node = new Node;

		new_node->attribute = attribute;
		new_node->value = value;
		new_node->result = result;
		new_node->equal = equal;
		new_node->less_than = less;
		new_node->greater_than = greater;

		if(start == NULL){
			node = new_node;
			start = new_node;
		}
		else{
			node->next = new_node;
			node = new_node;
		}

		if(has_child){
			lines_pos++;
			node->child = add_node(indent+1);
		}
		else{
			lines_pos++;
		}

	} while(lines_pos < lines.size());

	return start;
}
//================================================

TreeData generate_tree(string file_name) {
	string name_file = file_name + ".names";
	string data_file = file_name + ".data";
	TreeData t;
	
	t.name = parse_name_file(name_file);
	
	// Run c4.5
	string command = "../R8/Src/c4.5 -f " + file_name + " > temp.txt";
	
	system(command.c_str());
	
	vector<string> lines_before = load_file("temp.txt");
	
	int start = -1, end = -1;
	
	lines.clear();
	
	for(int i = 0;i < lines_before.size();i++) {
		if(lines_before[i] == "Decision Tree:") {
			start = i + 1;
		}
	}
	
	for(int i = start;i < lines_before.size();i++) {
		if(lines_before[i] == "Simplified Decision Tree:" || lines_before[i] == "Tree saved") {
			end = i;
			break;
		}
	}
	
	if(start == -1 || end == -1) {
		cout << "ERROR!" << endl;
		throw -1;
	}
	
	cout << "Start: " << start << endl;
	cout << "End: " << end << endl;
	
	for(int i = start;i < end;i++) {
		if(lines_before[i] != "")
			lines.push_back(lines_before[i]);
	}
	
	tree = add_node(0);
	
	return t;
}

void print_tree(Node *n, int indent) {
	if(n == NULL) return;
	
	while(n) {
		for(int i = 0;i < indent;i++)
			cout << "\t";
		
		cout << n->attribute;
		
		if(n->less_than)
			cout << "<";
		else if(n->greater_than)
			cout << ">";
		
		if(n->equal)
			cout << "=";
		
		cout << n->value << " (" << n->result << ")" << endl;
		
		print_tree(n->child, indent + 1);
		
		n = n->next;
	}
}
	
	
#if 0
struct Node{
	Node *child;
	Node *next;

	string attribute;
	string value;
	string result;
	
	bool less_than;
	bool greater_than;
	bool equal;

	Node(){
		child = NULL;
		next = NULL;
	}

	~Node(){
		if(child) delete child;
		if(next) delete next;
	}
#endif

int main(int argc, char *argv[]) {
	TreeData data = generate_tree((string)argv[1]);
	
	for(auto i : data.name.att) {
		cout << "'" << i.name << "'" << endl;
		
		if(!i.continuous) {
			for(auto d : i.values) {
				cout << "\t'" << d << "'" << endl;
			}
		}
		else {
			cout << "\tcontinuous" << endl;
		}
	}
	
	cout << "=========Tree========" << endl;
	
	print_tree(tree, 0);
	
	
	
	return 0;
}

















#if 0
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

#endif
