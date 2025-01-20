#include <iostream>
#include <sstream>
#include <string>


int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // Uncomment this block to pass the first stage
  while(true){
    std::cout << "$ ";

    std::string input;
    std::getline(std::cin, input);
  
    if(input == "exit 0"){
      return 0;
    }

    else if(input.substr(0,5) == "echo "){
      std::cout<<input.substr(5,input.size() - 5)<<std::endl;
    }

    else{
      std::cout<<input<<": command not found\n";
    }
    
  }
}
