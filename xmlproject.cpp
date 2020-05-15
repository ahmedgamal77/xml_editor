

#include "pch.h"
#include"xml_tree.h"
#include <string>
#include <iostream>
#include <fstream>
#include <stack>
#include <sstream> 
#include <chrono> 

#include "xml_cutter.h"
using namespace std::chrono;


using namespace std;

void pp(Node * k, xml_tree tree, int m);

int main()
{
	string input;
	ifstream inFile;
	vector<Node *> nodes;
	xml_tree tree;
	stack<int>tags;
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


	for (std::string line; getline(infile, line);)  //line bye line
	{
		int data = 0;
		if (line[0] == '<' &&  line[line.length() - 2] == '/')  //case (self closing tag)
		{
			outfile << line << endl;
			continue;
		}


		if (line[0] == '<' && line[1] != '/') //opening tag

		{
			if (line[1] == '!') { outfile << line << endl; continue; }
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

			getline(infile, line);

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
		cout << "Unable to open file";
		exit(1); // terminate with error
	}
	
	while (getline(inFile, input))
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
			if(input[input.length()-2]=='/')
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

	cout << "Time taken by function: "
		<< duration.count() << " microseconds" << endl;
	//-----------------------
	//testing for the tree
	Node * root = tree.get_root();
	//pp(root, tree,0);
	//---------------
	return 0;
}

void pp(Node * k, xml_tree tree,int m ) {
	//function to print all the tags in the file 
	for (int i = 0; i < m; i++) {
		cout << " ";
	}
	tree.print(k);
	m++;
	vector<Node*> ss = tree.get_children(k);
	if (tree.get_children(k).size() > 0) {
		for (int i = 0; i <ss.size(); i++) {

			pp(ss[i], tree ,m);
			m++;

		}
	}
}
