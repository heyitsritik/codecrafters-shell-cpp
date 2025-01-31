#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>
#include <unistd.h>
#include <fstream>
using std::cout;
using std::endl;
using std::string;
using std::stringstream;
using std::vector;
using std::fstream;
namespace fs = std::filesystem;


string get_address(string command)
{
	string get_paths = std::getenv("PATH");
	string path;
	stringstream ss(get_paths);

	while (!ss.eof())
	{
		getline(ss, path, ':');
		string get_abs_path = path + '/' + command;
		if(std::filesystem::exists(get_abs_path)){
			return get_abs_path;
		}
	}
	return "";
}

string executable_path(string command){
	string get_paths = std::getenv("PATH");
	string path;
	stringstream ss(get_paths);

	while (!ss.eof())
	{
		getline(ss, path, ':');
		string get_abs_path = path + '/' + command;
		if(!access(get_abs_path.c_str(),X_OK)){
			return get_abs_path;
		}
	}
	return "";
}
string handleEcho(string input, vector<string>& words, bool print){
	int len = input.size();
	string output = "";
	if(input[5] == '\''){
		string sent = "";
		vector<string> sents;
		bool flag = false;
		for(int i = 5; i<len - 1;i++){
			if(input[i] == '\'' && input[i+1] == '\''){
				i++;
				continue;
			}else if(input[i] == '\''){
				flag = !flag;
				if(!flag){
					sents.push_back(sent);
					sent = "";
				}
				continue;
			}
			if(flag)
				sent += input[i];
		}
		sents.push_back(sent);
		for(string st:sents){
			output += st + " ";
		}
		cout<<endl;
	}else if(input[5] == '\"'){
		string sent = "";
		vector<string> sents;
		bool flag = false;
		for(int i = 5; i<len - 1;i++){
			if((input[i] == '\"' && input[i+1] == '\"')){
				i++;
				continue;
			}else if(input[i] == '\\' && input[i+1] == '\\'){
				i++;
				sent += '\\';
				continue;
			}else if(input[i] == '\\' && input[i+1] == '\"'){
				i++;
				sent += input[i];
				continue;
			}else if(input[i] == '\"'){
				flag = !flag;
				if(!flag){
					sents.push_back(sent);
					sent = "";
					if(i+1 < len && input[i+1] == ' '){
						sents.push_back(" ");
					}
				}
				continue;
			}else if(sent.empty() && !flag && input[i] != ' '){
				flag = !flag;
				sent += input[i];
				continue;
			}else if(sent.empty() && flag && input[i] == ' '){
				flag = !flag;
				sents.push_back(sent);
				sent = "";
				if(i+1 < len && input[i+1] == ' '){
					sents.push_back(" ");
				}
				continue;
			}
			if(flag)
				sent += input[i];
		}
		sents.push_back(sent);
		for(string st:sents){
			output += st;
		}
		output += "\n";
	}else if(input.find('\\') != string::npos){
			string sample = "";
			for(int i = 5; i<len; i++){
				if(input[i] == '\\'){
					continue;
				}
				sample += input[i];
			}
			output += sample + "\n";
	}else{
		string word;
		int t = 1;
		for(int i = 1; i<words.size(); i++){
			output += words[i] + " ";
		}
		output += "\n";
	}

	if(print){
		cout<<output<<endl;
		return "";
	}
	return output;
		
}
string handleCat(string input, bool print){
	int idx = 4;
	int len = input.size();
	string output = "";
	char chr;
	if(input.find('\'') < input.find('\"')){
		chr = '\'';
	}else{
		chr = '\"';
	}
	vector<string> paths;
	string path = "";
	bool flag = false;
	while(idx < len){
		if(input[idx] == chr){
			idx++;
			flag = !flag;
			if(flag == false){
				paths.push_back(path);
				path = "";
			}
		}
		else if(flag){
			path += input[idx];
			idx++;
		}else{
			idx++;
		}
	}
	path = "";
	for(auto p:paths){
		std::ifstream file(p);
		if(file.fail()){
			cout<<"cat: "<<p<<" No such file or directory";
		}else{
			while(getline(file, path)){
				output += path;
			}
		}
		file.close();
	}
	output += "\n";

	if(print){
		cout<<output<<endl;
		return "";
	}
	return output;
}


int main()
{
	// Flush after every std::cout / std:cerr
	std::cout << std::unitbuf;
	std::cerr << std::unitbuf;

	// Uncomment this block to pass the first stage
	while (true)
	{
		cout << "$ ";

		string input;
		std::getline(std::cin, input);

		if(input.back() == '\\'){
			input.pop_back();
			input += "\\n";
					string extra = "";
					cout<<"> ";
					getline(std::cin, extra);
					while(extra.back() != '\\'){
						extra.pop_back();
						extra += "\\n";
						input += extra;
						extra = "";
						cout<<"> ";
						getline(std::cin,extra);
					}
					input += extra;
					cout<<input<<endl;
		}
		int len = input.size();
		if (input == "exit 0" || input == "exit0")
		{
			return 0;
		}


		vector<string> words;
		stringstream s(input);
		string word;
		while (s >> word)
		{
			words.push_back(word);
		}
		if (!words.empty() && words[0] == "echo")
		{
			if(input.find(">") != string::npos || input.find("1>") != string::npos){
				// string fileAppended = executable_path(words.back());
				int pos = input.find(">");
				int ex = 1;
				if(input.find("1>") != string::npos){
					ex = 2;
				}
				std::ofstream outfile;
				outfile.open(words.back(), std::ios_base::app);
				outfile << handleEcho(input.substr(0,pos-ex),words,false);
				outfile.close();
			}else{
				handleEcho(input,words,true);
			}
		}
		else if (!words.empty() && words[0] == "type")
		{
			if (words.size() > 1 && (words[1] == "echo" || words[1] == "type" || words[1] == "exit" || words[1] == "pwd" || words[1] == "cd" || words[1] == "ls"))
			{
				cout << words[1] << " is a shell builtin" << endl;
			}
			else
			{
				string path = get_address(words[1]);
				if(!path.empty())
					cout<<words[1]<< " is "<< path<<endl;
				else
					cout << words[1] << ": not found" << endl;
			}
		}else if(!words.empty() && words[0] == "pwd"){
			string pwd = std::filesystem::current_path();
			cout<<pwd<<endl;
		}else if(!words.empty() && words[0] == "cd"){
			if(words.size() > 1 && words[1] == "~"){
				chdir(std::getenv("HOME"));
			}
			else if(words.size() > 1 && fs::is_directory(words[1])){
				chdir((words[1]).c_str());
			}else{
				cout<<"cd: " << words[1]<<": No such file or directory"<<endl;
			}
		}
		else if(!word.empty() && (input[0] == '\'' || input[0] == '\"')){
			string path;
			std::ifstream file(words.back());
				while(getline(file, path)){
					cout<<path;
				}
			file.close();
			cout<<endl;

		}else if(words.empty() && words[0] == "ls"){
			cout<<"world"<<endl;
			if(input.find('>') != string::npos or input.find("1>") != string::npos){
				cout<<"hello"<<endl;
				int idx = 1;
				string output = "";
				while(words[idx] != ">" and words[idx] != "1>"){
					if(words[idx].find('-') == string::npos){
						cout<<words[idx]<<endl;
						for(const auto entry: fs::directory_iterator(words[idx])){
							output += entry.path();
							cout<<entry.path()<<endl;;
							output += "\n";
						}
					}
					idx++;
				}
				// string fileAppended = executable_path(words.back());
				cout<<output<<endl;
				std::ofstream outfile;
				outfile.open(words.back(),std::ios_base::app);
				outfile<<output;
				outfile.close();
			}
		}
		else if(!words.empty() && words[0] == "cat"){
			if(input.find(">") != string::npos || input.find("1>") != string::npos){
				// string fileAppended = executable_path(words.back());
				int pos = input.find(">");
				int ex = 1;
				if(input.find("1>") != string::npos){
					ex = 2;
				}
				std::ofstream outfile;
				outfile.open(words.back(), std::ios_base::app);
				outfile << handleCat(input.substr(0,pos - ex),false);
				outfile.close();
			}else{
				handleCat(input,true);
			}
		}
		else{
			string get_path = executable_path(words[0]);
			// cout<<get_path<<endl;
			if(!get_path.empty()){
				std::system(input.c_str());
			}else
				cout << input << ": command not found\n";
		}
	}
}
