#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>
#include <unistd.h>
#include <cstdlib>
using std::cout;
using std::endl;
using std::string;
using std::stringstream;
using std::vector;
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
			if (input.size() > 4)
				cout << input.substr(5, input.size() - 5) << endl;
			else
				cout << endl;
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
			if(words.size() > 1 && fs::is_directory(words[1])){
				chdir((words[1]).c_str());
			}else{
				cout<<"cd: " << words[1]<<": No such file or directory"<<endl;
			}
		}

		else
		{
			string get_path = executable_path(words[0]);
			// cout<<get_path<<endl;
			if(!get_path.empty()){
				std::system(input.c_str());
			}else
				cout << input << ": command not found\n";
		}
	}
}
