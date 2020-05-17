

#include "pch.h"
#include"xml_tree.h"
#include <string>
#include <iostream>
#include <fstream>
#include <stack>
#include <sstream> 
#include <chrono> 
#include<vector>
#include "xml_cutter.h"
using namespace std::chrono;


using namespace std;

void pp(Node* k, xml_tree tree, int m);

void print_all_children(Node* n, xml_tree tree, ofstream & final, int i);
string tabs(int i);

int main()
{
	string input;
	ifstream inFile;
	vector<Node*> nodes;
	xml_tree tree;
	stack<int>tags;
	ofstream final;
	final.open("FinalOutput.txt");
	auto start = high_resolution_clock::now();
	xml_cutter("tests.txt");//function to make each xml tag in seprate line (the function take xml file as an inpput)
	// start of checking and correcting errors in the xml file
	ifstream infile;
	infile.open("output.txt");
	ofstream outfile;
	outfile.open("output1.txt");


	stack <string> s1;


	//for (int i=0 ; i<3 ; i++) {std::string line1; getline( infile, line1 ); outfile<<line1<<endl;}
	//std::string line1; getline( infile, line1 ); outfile<<line1;  //awel kam satr abl el tags


	for (std::string line; std::getline(infile, line);)  //line bye line
	{
		int data = 0;
		if (line[0] == '<' && line[line.length() - 2] == '/')  //case (self closing tag)
		{
			outfile << line << endl;
			continue;
		}
		if (line.length() >= 2) {
			if (line[line.length() - 2] == '-' && line[line.length() - 1] == '>') continue;
		}

		if (line[0] == '<' && line[1] != '/') //opening tag

		{
			if (line[1] == '!' || line[1] == '?')  continue;  //comment tags (msh batba3hom khales)
			s1.push(line);  //by7ot el line kolo
			outfile << s1.top() << endl; //batba3 el opening tag
		}


		else if (line[0] != '<') //data
		{

			data = 1;
			outfile << line << endl; //etba3 el data

			//w etba3 b3daha 3ala tool el closing tag
			stringstream str(s1.top());
			s1.pop();
			str >> line;  //3lshan akhod el tag bs mn gher el attributes
			if (line[line.length() - 1] == '>')  //bt-check akher char howa ">" wla la
				outfile << "<" << "/" << line.substr(1) << endl;
			else
				outfile << "<" << "/" << line.substr(1) << ">" << endl;

			std::getline(infile, line);

		}


		else if (line[0] == '<' && line[1] == '/')  //closing tag
		{
			if (s1.empty()) continue;  //law el stack kan fadi
			if (data) { data = 0; continue; } //law kan el satr eli abli data

			stringstream str(s1.top());
			s1.pop();
			str >> line;  //3lshan akhod el tag bs mn gher el attributes
			if (line[line.length() - 1] == '>')  //bt-check akher char howa ">" wla la
				outfile << "<" << "/" << line.substr(1) << endl;
			else
				outfile << "<" << "/" << line.substr(1) << ">" << endl;

		}


	}


	//law el closing tags khelset w lsa feh opening tags
	string line;
	while (!s1.empty())
	{
		stringstream str(s1.top());
		s1.pop();
		str >> line;  //3lshan akhod el tag bs mn gher el attributes
		if (line[line.length() - 1] == '>')  //bt-check akher char howa ">" wla la
			outfile << "<" << "/" << line.substr(1) << endl;
		else
			outfile << "<" << "/" << line.substr(1) << ">" << endl;
	}



	//end of checking and correcting errors
	//start of implementing the xml tree from the xml file
	inFile.open("output1.txt");
	if (!inFile) {
		std::cout << "Unable to open file";
		exit(1); // terminate with error
	}

	while (std::getline(inFile, input))
	{


		//Opening Tag			// We can take Tag name and Attribute from this line
		if (input[0] == '<' && (input[1] == '!' || input[1] == '?')) {
			continue;
		}
		if (input[0] == '<' && input[1] != '/')
		{

			int index;
			// Searching for the space to get the tag name only from the line
			for (int i = 1; i < input.length(); i++)
			{
				index = i;
				if (input[i] == ' ')
				{
					//First white space in the tag
					break;
				}
			}
			string tag = input.substr(1, index - 1);
			string att = input.substr(index + 1);
			nodes.push_back(tree.add_node(tag, att));//decalre a new tag 
			if (tags.size() == 0) {
				tree.add_root(nodes[nodes.size() - 1]);//add thre root to the tree
			}
			else {
				tree.add_child(nodes[tags.top()], nodes[nodes.size() - 1]);//add a child to the last opened tag
			}
			tags.push(nodes.size() - 1);//add the last opened tag to deal with it to add children or data to it 
			if (input[input.length() - 2] == '/')
				tags.pop();//self closing tag 

		}
		//Closing tag
		else if (input[0] == '<' && input[1] == '/')
		{
			tags.pop();//remove the last opend tag (closed) to deal with the next tag to it 
		}
		//Data
		else
		{
			tree.add_data(nodes[tags.top()], input);//add data to the last opened tag

		}
	}

	//end of impementation the xml tree from the xml file



	inFile.close();
	//printing the execution time for the code
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);

	std::cout << "Time taken by function: "
		<< duration.count() << " microseconds" << endl;
	//-----------------------
	//testing for the tree
	Node* root = tree.get_root();
	//pp(root, tree,0);
	//---------------
	//-----SETTING UP THE FORMAT FOR THE FINAL OUTPUT FILE--------------------


	print_all_children(root, tree, final, 0);




	//-----------QUERIES FROM THE USER----------------------//


	vector<Node*>tags_children = tree.get_children(root);
	vector<Node*>NoOFSynsets;
	vector<Node*>synsets(0);
	//looping to get to synsets tag
	if (tags_children.size() == 1 && tree.get_tag(tags_children[0]) == "synsets") {
		NoOFSynsets = tree.get_children(tags_children[0]);
		std::cout << "Number Of Synsets is :" << NoOFSynsets.size() << endl;
	}
	else if (tree.get_tag(root) == "synsets") {

		NoOFSynsets = tree.get_children(root);
		std::cout << "Number Of Synsets is :" << NoOFSynsets.size() << endl;
	}
	else {
		for (int i = 0; i < tags_children.size(); i++) {

			if (tree.get_tag(tags_children[i]) == "synsets") {
				synsets.push_back(tags_children[i]);

			}
		}


		for (int i = 0; i < synsets.size(); i++) {
			vector<Node*>b = tree.get_children(synsets[i]);

			NoOFSynsets.insert(NoOFSynsets.end(), b.begin(), b.end());



		}
		std::cout << "Number Of Synsets is :" << NoOFSynsets.size() << endl;
	}

	//getting word and output its definition 
	string word;
	std::getline(cin, word);
	string id;
	cin>>id;
	
	string def;
	int flag = 0;
	for (int i = 0; i < NoOFSynsets.size(); i++) {
		vector<Node*>child = tree.get_children(NoOFSynsets[i]);
		Node* words;
		for (int j = 0; j < child.size(); j++) {
			if (tree.get_tag(child[j]) == "word") {
				if (tree.get_data(child[j]) == word && tree.get_attributes(child[j]).find(id)!=std::string::npos) {
					flag = 1;
					break;

				}
			}
		}
		if (flag) {
			for (int j = 0; j < child.size(); j++) {
				if (tree.get_tag(child[j]) == "def") {
					def=tree.get_data(child[j]);
					break;
				}
			}

			break;

		}

	}
	std::cout << "Defintion is : " << def << endl;
	return 0;
}
string tabs(int i) {
	string s = "";
	for (int j = 1; j <= i; j++) {
		s += "\t";
	}
	if (i == 0) {
		return "";
	}
	return s;
}
void print_all_children(Node* n, xml_tree tree, ofstream & final, int i) {
	//function to print all tags with format 
	final << tabs(i) << "<" << tree.get_tag(n) << (tree.get_attributes(n) == "" ? ">" : " " + tree.get_attributes(n)) << endl;//prints the opening tag

	if (tree.get_data(n) != "") //prints data if exists
	{
		final << tabs(i) << tree.get_data(n) << endl;
	}

	vector<Node*>child = tree.get_children(n);//check for children
	if (child.empty()) {
		if (tree.get_attributes(n) == "") //if no children exists and there is no attributes ,print closing tag
		{
			//closing tags
			final << tabs(i) << "<" << tree.get_tag(n) << "/>" << endl;
			return;
		}
		else {
			string att = tree.get_attributes(n);
			if (att[att.length() - 2] != '/') {
				final << tabs(i) << "<" << tree.get_tag(n) << "/>" << endl;
				return;
			}
		}

	}
	else {
		i++;
		for (int j = 0; j < child.size(); j++) {

			print_all_children(child[j], tree, final, i);
		}
		if (tree.get_attributes(n) == "") {
			//closing tags for parent nodes
			final << tabs(i - 1) << "<" << tree.get_tag(n) << "/>" << endl;

		}
		else {
			string att = tree.get_attributes(n);
			if (att.length() > 3 && att[att.length() - 2] != '/') {
				final << tabs(i - 1) << "<" << tree.get_tag(n) << "/>" << endl;
				return;
			}
		}

	}
}
void pp(Node* k, xml_tree tree, int m) {
	//function to print all the tags in the file 
	for (int i = 0; i < m; i++) {
		std::cout << " ";
	}
	tree.print(k);
	std::cout << tree.get_attributes(k) << endl;
	m++;
	vector<Node*> ss = tree.get_children(k);
	if (tree.get_children(k).size() > 0) {
		for (int i = 0; i < ss.size(); i++) {

			pp(ss[i], tree, m);
			m++;

		}
	}
}
