#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using std::cout;
using std::endl;
using std::string;
using std::stringstream;
using std::vector;



int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // Uncomment this block to pass the first stage
  while(true){
    cout << "$ ";

    string input;
    std::getline(std::cin, input);

    if(input == "exit 0"){
      return 0;
    }

    vector<string> words;
    stringstream s(input);
    string word;
    while(s >>word){
      words.push_back(word);
    }
    if(!words.empty() && words[0] == "echo"){
      if(input.size() > 4)
        cout<<input.substr(5,input.size() - 5)<<endl;
      else
        cout<<endl;
    }
    else if(!words.empty() && words[0] == "type"){
      if(words.size() > 1 && (words[1] == "echo" || words[1] == "type" || words[1] == "exit")){
        cout<<words[1]<<" is a shell builtin"<<endl;
      }else {
        cout<<words[1]<<": not found"<<endl;
      }
    }

    else{
      cout<<input<<": command not found\n";
    }
    
  }
}
