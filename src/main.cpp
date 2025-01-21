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
			if(input[5] == '\''){
				string sent = "";
				vector<string> sents;
				bool flag = false;
				for(int i = 5; i<input.size() - 1;i++){
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
					cout<<st<<" ";
				}
				cout<<endl;
			}else if(input[5] == '\"'){
				string sent = "";
				vector<string> sents;
				bool flag = false;
				for(int i = 5; i<input.size() - 1;i++){
					if(input[i] == '\"' && input[i+1] == '\"'){
						i++;
						continue;
					}else if(input[i] == '\"'){
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
					cout<<st<<" ";
				}
				cout<<endl;
			}else{
				string word;
				int t = 1;
				for(int i = 1; i<words.size(); i++){
					cout<<words[i]<<" ";
				}
				cout<<endl;
			}
		}
		else if (!words.empty() && words[0] == "type")
		{
			if (words.size() > 1 && (words[1] == "echo" || words[1] == "type" || words[1] == "exit" || words[1] == "pwd" || words[1] == "cd"))
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
		else if(!words.empty() && words[0] == "cat"){
			int idx = 4;
			vector<string> paths;
			string path = "";
			bool flag = false;
			while(idx < input.size()){
				if(input[idx] == '\'' || input[idx] == '\"'){
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
				while(getline(file, path)){
					cout<<path;
				}
				file.close();
			}
			cout<<endl;
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
