

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
void HypernymsOfAWord(xml_tree tree, vector<Node*> &NoOFSynsets, string word, string id, vector<string> &Hypernyms);
string WordDefinition(xml_tree tree, vector<Node*> &NoOFSynsets, string word, string id);
void pp(Node* k, xml_tree tree, int m);
int SetNumber(xml_tree tree, vector<Node*> &NoOFSynsets);
void print_all_children(Node* n, xml_tree tree, ofstream & final, int i);
vector <string> att_cutter(string input);
vector<int> sort_children(vector<Node *> &children);
void print_node(Node* node_ptr, int &tab, int repeat);
void xml_to_json(xml_tree tree);
string tabs(int i);
ofstream json;
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
	
	int num_of_errors=0;
	string error ;
        string char1 = "<";  string char2 = "/";  string char3 = ">";

	


	//for (int i=0 ; i<3 ; i++) {std::string line1; getline( infile, line1 ); outfile<<line1<<endl;}
	//std::string line1; getline( infile, line1 ); outfile<<line1;  


	
        for (std::string line; getline(infile, line);)  //line bye line
        {
            int data = 0;
            if (line[0] == '<' &&  line[line.length() - 2] == '/')  //case (self closing tag)
            {
                outfile << line.substr(0,line.length()-2)<<">"<< endl;
                stringstream str (line);
                str >> line;
                outfile << "<" << "/" << line.substr(1) <<">"<<endl;
                continue;
            }

            if (line.length() >= 2)
            {
                if (line[line.length() - 2] == '-' && line[line.length() - 1] == '>') continue;
            }



             if (line[0] != '<') //data
            {

                data = 1;
                outfile << line << endl; //print the data

                //print its closing tag
                stringstream str(s1.top());
                s1.pop();
                str >> line;  //to take the tag without its attributes
                if (line[line.length() - 1] == '>')  //check if the last char is ">"
                {
                   outfile << "<" << "/" << line.substr(1) << endl;
                    error =  char1 + char2 + line.substr(1) ;
                }
                else
                {
                    outfile << "<" << "/" << line.substr(1) << ">" << endl;
                    error = char1 + char2 + line.substr(1) + char3 ;
                }


                getline(infile, line);

            }



             if (line[0] == '<' && line[1] != '/') //opening tag

             {
                 if (data==1) num_of_errors++;
                 if (line[1] == '!' || line[1] == '?')  continue;  //comment tags (ignore them)
                 s1.push(line);  //we push the hole line
                 outfile << s1.top() << endl; //print opening tag
                 continue;
             }


             if (line[0] == '<' && line[1] == '/')  //closing tag
            {

                if (s1.empty()) continue;  //if the stack was empty
                if (data) { data = 0; if (error != line ) num_of_errors++ ;continue; } //if the previous line was a data

                stringstream str(s1.top());
                s1.pop();
                str >> line;  //to take the tag without its attributes
                if (line[line.length() - 1] == '>')  //check if the last char is ">"
                {
                     outfile << "<" << "/" << line.substr(1) << endl;
                     error =  char1 + char2 + line.substr(1) ;
                }

                else
                {
                     outfile << "<" << "/" << line.substr(1) << ">" << endl;
                     error =  char1 + char2 + line.substr(1) + char3;
                }

                //if (error != line ) cout <<"error";

            }


        }

        //if there are no more closing tags but the stack is note empty (remaining opening tags)
        string line;
        while (!s1.empty())
        {
            stringstream str(s1.top());
            s1.pop();
            str >> line; //to take the tag without its attributes
            if (line[line.length() - 1] == '>')  //check if the last char is ">"
                 outfile << "<" << "/" << line.substr(1) << endl;

            else
                outfile << "<" << "/" << line.substr(1) << ">" << endl;

            num_of_errors ++;
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
			string att = input.substr(index + 1, (input.size() - index - 2));
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
	xml_to_json(tree);
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



	vector<Node*>NoOFSynsets;
	
	//--------1---------looping to get to synsets numbers-----------------------
	int number=SetNumber(tree,NoOFSynsets);

	if (number != 0) {
		cout<<"Number of SynSets is : "<<number<<endl;
	}
	else {
		cout<<"No Synsets Available"<<endl;
	}


	//---------2--------------getting word and output its definition ---------------------------------
	cout<<"input a word to get the definition"<<endl;
	string word;
	std::getline(cin, word);
	
	string id;
	cin>>id;
	
	string def= WordDefinition(tree,NoOFSynsets, word, id);

	if (def == "") {
		cout<<"Word does not exist , Please Try Another Word!"<<endl;
	}
	else {
		cout<<"The Definition is : "<<def<<endl;
	}

	//........3..........................Getting the Hypernyms....................



	std::cout<<"input a word to get the hypernyms"<<endl;
	cin.ignore();
	std::getline(cin, word);

	std::cin>>id;
	vector<string>Hypernyms;
	HypernymsOfAWord(tree,NoOFSynsets, word, id, Hypernyms);

		cout<<"The Hypernyms are :"<<endl;
		for (int i = 0; i < Hypernyms.size(); i++) {
			cout<<Hypernyms[i]<<endl;
		}

	return 0;
}





int SetNumber(xml_tree tree,vector<Node*> &NoOFSynsets) {
	Node* root = tree.get_root();
	vector<Node*>tags_children = tree.get_children(root);
	vector<Node*>synsets(0);

	if (tags_children.size() == 1 && tree.get_tag(tags_children[0]) == "synsets") {
		NoOFSynsets = tree.get_children(tags_children[0]);
		return NoOFSynsets.size();
	}
	else if (tree.get_tag(root) == "synsets") {

		NoOFSynsets = tree.get_children(root);
		return NoOFSynsets.size();
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
		return NoOFSynsets.size();
	}

}


string WordDefinition(xml_tree tree,vector<Node*> &NoOFSynsets,string word) {
	string def;
	int flag = 0;
	for (int i = 0; i < NoOFSynsets.size(); i++) {
		vector<Node*>child = tree.get_children(NoOFSynsets[i]);
		Node* words;
		for (int j = 0; j < child.size(); j++) {
			if (tree.get_tag(child[j]) == "word") {
				if (tree.get_data(child[j]) == word ) {
					flag = 1;
					break;

				}
			}
		}
		if (flag) {
			for (int j = 0; j < child.size(); j++) {
				if (tree.get_tag(child[j]) == "def") {
					def = tree.get_data(child[j]);
					break;
				}
			}

			break;

		}

	}
	return def;

}

void HypernymsOfAWord(xml_tree tree,vector<Node*> &NoOFSynsets,string word , vector<string> &Hypernyms) {
	vector<string>refs(0);
	Hypernyms.resize(0);
	int flag=0;
	for (int i = 0; i < NoOFSynsets.size(); i++) {

		vector<Node*>child = tree.get_children(NoOFSynsets[i]);
		//getting the word;


		for (int j = 0; j < child.size(); j++) {
			if (tree.get_tag(child[j]) == "word") {
				if (tree.get_data(child[j]) == word ) {
					flag = 1;
					break;

				}
			}
		}
		//getting the hypernyms refs 
		if (flag) {
			for (int j = 0; j < child.size(); j++) {

				if (tree.get_data(child[j]) == "Hypernym") {

					string s = tree.get_attributes(child[j]);
					int i = s.find("n");
					int fin = s.find(">");
					s = s.substr(i, fin - i + 1);

					while (s.find("n") != std::string::npos) {
						int i = s.find("n");
						int j = s.find(" ");
						int w = s.find(">");
						int l;
						if (j != std::string::npos && (j + 1 != w)) {
							l = j - i;
						}
						else {
							l = w - i - 1;
						}

						refs.push_back(s.substr(i, l));
						s.erase(i, l + 1);


					}






				}
			}

			break;

		}

	}

	flag = 0;
	
	//getting hypernyms
	for (int i = 0; i < refs.size(); i++) {

		for (int j = 0; j < NoOFSynsets.size(); j++) {
			string id = tree.get_attributes(NoOFSynsets[j]);
			if (id.find(refs[i]) != std::string::npos) {

				vector<Node*>child = tree.get_children(NoOFSynsets[j]);
				Node* words;
				for (int k = 0; k < child.size(); k++) {
					if (tree.get_tag(child[k]) == "word") {
						Hypernyms.push_back(tree.get_data(child[k]));

					}
				}
			}

		}
	}


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
	final << tabs(i) << "<" << tree.get_tag(n) << (tree.get_attributes(n) == "" ? ">" : " " + tree.get_attributes(n)+">") << endl;//prints the opening tag

	if (tree.get_data(n) != "") //prints data if exists
	{
		final << tabs(i) << tree.get_data(n) << endl;
	}

	vector<Node*>child = tree.get_children(n);//check for children
	if (child.empty()) {
		if (tree.get_attributes(n) == "") //if no children exists and there is no attributes ,print closing tag
		{
			//closing tags
			final << tabs(i) << "</" << tree.get_tag(n) << ">" << endl;
			return;
		}
		else {
			string att = tree.get_attributes(n);
			if (att[att.length() - 2] != '/') {
				final << tabs(i) << "</" << tree.get_tag(n) << ">" << endl;
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
			final << tabs(i - 1) << "</" << tree.get_tag(n) << ">" << endl;

		}
		else {
			string att = tree.get_attributes(n);
			if (att.length() > 3 && att[att.length() - 2] != '/') {
				final << tabs(i - 1) << "</" << tree.get_tag(n) << ">" << endl;
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
vector <string> att_cutter(string input) {
	string att;
	int start, end, temp;
	vector<string> output;
	start = 0;
	string c = "\"";
	string c1 = ":";
	int counter = 0;
	int size = input.size();
	for (int i = 0; i < size; i++) {
		if ((input[i] == '\"') || (i == input.size() - 1)) {
			counter++;
			if (counter == 2)
			{
				end = i + 1;
				att = input.substr(start, end - start + 1);
				for (int j = 0; j < att.size(); j++) {
					if (att[j] == '=')
						temp = j + 1;
				}
				att.insert(0, c);
				att.insert(temp, c);
				att.replace(temp + 1, 1, c1);
				if (att[att.length() - 1] == '/')
					att.erase(att.length() - 1);
				output.push_back(att);
				start = i + 1;
				counter = 0;
			}
		}


	}

	return output;

}

vector<int> sort_children(vector<Node *> &children) {
	vector<int> repeat;
	string temp;
	Node * temp_ptr;
	for (int i = 0; i < children.size(); i++)
		for (int j = i + 1; j < children.size(); j++) {
			if ((children[j]->tag_name) < (children[i]->tag_name))
			{
				temp_ptr = children[j];
				children[j] = children[i];
				children[i] = temp_ptr;
			}
		}

	if (children.size() > 0) {
		temp = children[0]->tag_name;
		repeat.push_back(1);
		int j = 0;
		for (int i = 1; i < children.size(); i++) {
			if ((children[i]->tag_name) == temp) {
				repeat[j]++;
			}
			else
			{
				repeat.push_back(1);
				j++;
				temp = children[i]->tag_name;
			}
		}

	}
	return repeat;

}

void print_node(Node* node_ptr, int &tab, int repeat, int repeat_max) {
	if (node_ptr->children.size()) {
		for (int i = 0; i < tab; i++) {
			json << "\t";
		}
		if (repeat > 0) {

			json << ",{" << endl;

		}
		else if (repeat == 0) {

			json << "\"" << node_ptr->tag_name << "\":[" << endl;
			tab++;
			for (int i = 0; i < tab; i++) {
				json << "\t";
			}
			json << "{" << endl;
			tab++;

		}
		else if (repeat == -1) {

			json << "\"" << node_ptr->tag_name << "\":{" << endl;
			tab++;
		}

		vector<int> repeat = sort_children(node_ptr->children);
		int j = 0;
		for (int k = 0; k < repeat.size(); k++) {
			if (repeat[k] > 1)
			{

				for (int i = 0; i < repeat[k]; i++) {
					print_node(node_ptr->children[j], tab, i, repeat[k]);
					tab--;
					j++;
				}
				for (int i = 0; i < tab; i++) {
					json << "\t";
				}
				json << "]";

			}
			else {
				print_node(node_ptr->children[j], tab, -1, 1);
				tab--;
				j++;
			}
			if (k != repeat.size() - 1 || node_ptr->att.size())
				json << "," << endl;
			else
				json << endl;
		}
		if (!node_ptr->att.size())
			json << "}" << endl;
		/*else
			json<<*/

	}if (node_ptr->att.size() || node_ptr->data.size()) {
		if (!node_ptr->children.size()) {
			for (int i = 0; i < tab; i++) {
				json << "\t";
			}

			if (repeat == 0) {

				json << "\"" << node_ptr->tag_name << "\":[" << endl;
				tab++;

			}
			else if (repeat == -1) {

				json << "\"" << node_ptr->tag_name << "\":";
			}
		}
		if (node_ptr->att.size()) {
			vector<string> attributes = att_cutter(node_ptr->att);
			for (int i = 0; i < attributes.size(); i++) {
				if (i != 0)
					for (int i = 0; i < tab; i++) {
						json << "\t";
					}
				else {
					if (!node_ptr->children.size())
						json << "{" << endl;
				}
				json << attributes[i];
				if (i != attributes.size() - 1)
					json << "," << endl;
			}
			if (node_ptr->data.size())
				json << "," << endl;
			else
				json << endl;
			if (!node_ptr->data.size())
				json << "}" << endl;
		}if (node_ptr->data.size()) {
			if (repeat == 0 || node_ptr->att.size())
				for (int i = 0; i < tab; i++) {
					json << "\t";
				}
			if (node_ptr->att.size()) {

				json << "\"text\":" << "\"" << node_ptr->data << "\"";
				json << "}" << endl;
			}
			else {
				json << "\"" << node_ptr->data << "\"";
			}
			if (repeat >= 0 && repeat != repeat_max - 1)
				json << "," << endl;
			else if (repeat == repeat_max - 1)
				json << endl;
		}


	}

}

void xml_to_json(xml_tree tree) {

	json.open("json.txt");
	int tab = 1;
	json << "{" << endl;
	print_node(tree.get_root(), tab, -1, 1);
	json << "}" << endl;

	json.close();


}

