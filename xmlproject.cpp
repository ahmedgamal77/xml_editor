

#include "pch.h"
#include"xml_tree.h"
#include <string>
#include <iostream>
#include <fstream>
#include <stack>
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
	xml_cutter("data.adj.xml");//function to make each xml tag in seprate line (the function take xml file as an inpput)
	inFile.open("output.txt");
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
	Node * root = tree.get_root();
	//pp(root, tree,0);
	inFile.close();
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);

	cout << "Time taken by function: "
		<< duration.count() << " microseconds" << endl;

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
