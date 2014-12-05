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
#include <map>

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
	set<string> result_values;
};

NameFile parse_name_file(string name) {
	vector<string> lines = load_file(name);
	NameFile file;
	bool done_result = false;
	
	
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
			
			while(line[pos] != ':') {
				if(pos + 1 < line.length() && line[pos + 1] == ':' && line[pos] == ' ') {
					pos++;
					break;
				}
				else {
					at_name += line[pos++];
				}
			}
			
			pos++;
			
			while(line[pos] == ' ' || line[pos] == '\t')
				pos++;
				
			string val_name;
			Attribute a;
			a.name = at_name;
			a.continuous = false;
			
			while(pos < line.length() && line[pos] != '.') {
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
		else if(line != "") {
			if(!done_result) {
				bool non_white = false;
				
				for(int i = 0;i < line.size();i++) {
					if(line[i] != ' ' && line[i] != '\t') {
						non_white = true;
						break;
					}
				}
				
				if(!non_white)
					continue;
				
				int pos = 0;
				string name = "";
				
				while(pos < line.length() && line[pos] == ' ')
					pos++;
				
				while(pos < line.length()) {
					if(line[pos] == ',') {
						file.result_values.insert(name);
						name = "";
						
						++pos;
						
						while(pos < line.length() && line[pos] == ' ')
							pos++;
					}
					else if(line[pos] == '.') {
						done_result = true;
						++pos;
					}
					else {
						name += line[pos++];
					}
				}
				
				if(name != "") {
					file.result_values.insert(name);
				}
			}
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
	
	cout << line << endl;
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
	
	for(int i = start;i < end;i++) {
		if(lines_before[i] != "")
			lines.push_back(lines_before[i]);
	}
	
	tree = add_node(0);
	
	return t;
}


// A training example
struct Example {
	map<string,string> data;
	string result;
	
	bool setValue(string &name, string &value, set<string> &attribute_list);
};

// Sets the value of an attribute in an examples
bool Example::setValue(string &name, string &value, set<string> &attribute_list){
//	if(attribute_list.count(name) == 0){
//		cout << "Error: attempting to set an undeclared attribute" << endl;
//		return false;
//	}

	//if(attribute_list[name].values.count(value) == 0){
	//	return false;
	//}

	data[name] = value;
	return true;
}

vector<Example> load_training_set(string file_name, TreeData &t) {
	vector<string> lines = load_file(file_name);
	vector<Example> examples;
	
	
	for(int i = 0;i < lines.size();i++) {
		if(lines[i] != "") {
			int count = 0;
			Example e;
			int pos = 0;
			string &line = lines[i];
			string at_value;
			
			while(pos < line.length()) {
				if(line[pos] == ',') {
					e.data[t.name.att[count++].name] = at_value;
					at_value = "";
					
					while(pos < line.length() && line[pos] != ',')
						pos++;
					
					++pos;
					
					while(pos < line.length() && (line[pos] == ' ' || line[pos] == '\t'))
						pos++;
					
					at_value = "";
				}
				else {
					at_value += line[pos++];
				}
			}
			
			if(at_value != "") {
				e.result = at_value;
			}
			
			examples.push_back(e);
		}
	}
	
	return examples;
}

// Classifies a sample against the decision tree
string classify_example(Example &e, Node *node){
	if(node == NULL) return "";

	string &attribute = node->attribute;
	string &node_value = node->value;
	string &result = node->result;

	string &value = e.data[attribute];

	
	if(node->equal && value == node->value ||
		node->less_than && atof(value.c_str()) < atof(node->value.c_str()) ||
		node->greater_than && atof(value.c_str()) > atof(node->value.c_str())) {
	
		if(node->child == NULL) return result;
		else return classify_example(e, node->child);
		
	}

	return classify_example(e,node->next);
}

// Computes and prints the confusion matrix
void print_confusion_matrix(TreeData &t, vector<Example> &example_list) {
	int count[50][50];
	
	cout << "? = unclassified by tree" << endl << endl;

	for(int i = 0;i < 50;i++){
		for(int d = 0;d < 50;d++){
			count[i][d] = 0;
		}
	}

	map<string,int> m;

	int max_length = 0;
	set<string> &s = t.name.result_values;
	
	//attribute_list[format.decision].values;
	int pos = 0;

	vector<string> a_list;

	for(set<string>::iterator i = s.begin();i != s.end();i++){
		m[*i] = pos++;

		if(i->length() > max_length){
			max_length = i->length();
		}
		a_list.push_back(*i);
	}
	
	m["?"] = pos++;
	
	a_list.push_back("?");

	cout << "List of attributes:" << endl;
	for(int i = 0;i < a_list.size();i++) {
		cout << (a_list[i])[0] << "[" << i << "] -> " << a_list[i] << endl;
	}
	
	int right = 0;
	int total = 0;
	
	for(int i = 0;i < example_list.size();i++){
		string res = classify_example(example_list[i], tree);
		
		if(res == "")
			res = "?";
		
		int p = m[res];
		int a = m[example_list[i].result];

		count[p][a]++;
		
		if(p == a)
			right++;
		
		total++;
	}

	cout << "\tPredicted" << endl << endl << "\t\t ";

	for(int d = 0;d < a_list.size();d++){
		cout << (a_list[d])[0] << "[" << d << "]" << "\t";
	}

	cout << endl << "\t\t";

	for(int d = 0;d < a_list.size();d++){
		cout << "--------";
	}

	cout << endl;

	pos = 0;

	string actual = "Actual";

	for(int i = 0;i < a_list.size() - 1;i++){
		if(pos < actual.length())
			cout << actual[pos++] << "\t";
		else
			cout << "\t";
		
		
		string length = to_string(a_list.size() - 1);
		string width = to_string(length.length());
		
		string format = "%c[%" + width + "d]|\t";
		
		printf(format.c_str(), (a_list[i])[0], i);

		for(int d = 0;d < a_list.size();d++){
			cout << count[d][i] << "\t";
		}
		cout << endl;
	}

	while(pos < actual.length()) cout << actual[pos++] << endl;
	
	cout << endl << "Accuracy: " << (right * 100) / total << "%" << endl;
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

// Displays a menu that allows the user to select an option
int show_menu(string title, string data, string prompt){
    cout << title << endl << endl;
    int pos = 1;
    
    cout << "1. ";
    
    for(int i = 0;i<data.length();i++){
        if(data[i] != '|'){
            cout << data[i];
        }
        else{
            cout << endl << ++pos << ". ";
        }
    }
    
    cout << endl << endl;
	
    int value;
    
    do{
		cout << prompt << ": ";
		string line;
		getline(cin, line);
		value = atoi(line.c_str());
    } while(value <= 0 || value >= pos+1);
    
    cout << endl;
    
    return value - 1;
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

#if 0
int main(int argc, char *argv[]) {
	TreeData data = generate_tree((string)argv[1]);
	string training_name = (string)argv[1] + ".data";
	vector<Example> examples = load_training_set(training_name, data);
	
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
	
	cout << "=========Training data=========" << endl;
	
	int count = 0;
	
	for(Example e : examples) {
		cout << "Example " << count++ << " -> " << e.result << endl;
		
		for(auto i : e.data) {
			cout << "\t" << i.first << " -> " << i.second << endl;
		}
	}
	
	cout << "=========Possible Result Values=========" << endl;
	
	for(string i : data.name.result_values) {
		cout << i << endl;
	}
	
	cout << "=========Confusion Matrix=========" << endl;

	print_confusion_matrix(data, examples);
	
	
	return 0;
}
#endif

// Has the user eneter attributes to classify an object
Example get_example(TreeData &t) {
	Example e;

	for(auto i : t.name.att) {
		
		do{
			cout << i.name << ": ";
			string s;

			getline(cin, s);
			
			if(i.values.count(s) != 0 || i.continuous) {
				e.setValue(i.name, s, i.values);
				break;
			}
			else{
				cout << "Invalid value for attribute" << endl << endl;
				
				if(i.continuous) {
					cout << "This attribute is continuous" << endl;
				}
				else {
					cout << "Valid values:" << endl;
					
					for(auto d : i.values) {
						cout << "\t" << d << endl;
					}
					
					cout << endl;
				}
			}
		} while(1);
	}

	return e;
}

int main(int argc, char *argv[]) {
	bool tree_built = false;
	TreeData data;
	vector<Example> examples;
	string file;
	
	tree = NULL;
	
	do{
		int choice = show_menu("Select an option",
			"Build tree|"
			"Calculate confusion matrix|"
			"Interactively test cases|"
			"Quit","Option");


		if(choice == 0){
			lines.clear();
			
			if(tree) delete tree;
			tree = NULL;

			string attribute_file;
			string training_file;

			bool success = false;
			
			cout << "Enter the dataset file (without extension): ";
			getline(cin, file);

			try {
				
				data = generate_tree(file);
				string training_name = file + ".data";
				examples = load_training_set(training_name, data);
				tree_built = true;
			}
			catch(int val) { }
		}
		else if(choice == 1){
			if(tree_built){
				try {
					cout << "Enter the dataset file (without extension): ";
					getline(cin, file);
					string training_name = file + ".data";
					examples = load_training_set(training_name, data);
					print_confusion_matrix(data, examples);
				}
				catch(int val) { }
			}
			else{
				cout << "Please build a tree first" << endl;
			}
		}
		else if(choice == 2){
			if(tree_built){
				cout << "Please enter the values for the following attributes:" << endl << endl;

				do{
					Example e = get_example(data);
					string c = classify_example(e,tree);

					if(c == ""){
						cout << "No objects match this description" << endl;
					}
					else{
						cout << endl << "Identified object: " << c << endl << endl;
					}

					int new_choice = show_menu("Would you like to enter another description?",
						"Yes|"
						"No; quit","Choice");

					if(new_choice == 1) break;
				} while(1);
			}
			else{
				cout << "Please build a tree first" << endl;
			}
		}
		else if(choice == 3){
			break;
		}

		cout << endl;

	} while(1);

	if(tree) delete tree;

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
