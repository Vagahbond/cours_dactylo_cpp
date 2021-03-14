#include <iostream>
#include <fstream>
#include <experimental/filesystem>
#include <string>
#include <vector>

namespace fs = std::experimental::filesystem;

int main(int argc, char const *argv[])
{
  std::cout << "Welcome to this dactylo skills measuring software. Words will be given to you and you will have to type them the fastest you can !" << std::endl;

  if (!fs::exists("dictionnary")) {
    std::cerr << "Please provide a dictionnary !" << std::endl;
    exit(1); 
  }

  std::ifstream stream("dictionnary", std::ios_base::in);
  std::vector<std::string> dictionnary;

  std::string temp;

  while(std::getline(stream, temp)) {
      dictionnary.push_back(temp);
  }

  for (int i = 0; i < dictionnary.size(); ++i) {
    std::cout << dictionnary[i] << std::endl;
  }

  return 0;
}
