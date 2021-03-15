#include <iostream>
#include <fstream>
#include <experimental/filesystem>
#include <string>
#include <vector>
#include <stdexcept>

namespace fs = std::experimental::filesystem;

std::vector<std::string> load_dictionnary(const char *path) {
  if (!fs::exists(path)) {
    std::cerr << "Please provide a dictionnary !" << std::endl;
    throw std::invalid_argument(std::string("Could not read dictionnary file.")); 
  }

  std::ifstream stream(path, std::ios_base::in);

  std::vector<std::string> res;
  std::string temp;

  while(std::getline(stream, temp)) {
    res.push_back(temp);
  }

  return res;

}

bool prompt_stop() {
  while (true) {
    std::cout << "Exit the program? (y/n)" << std::endl;
    char res;
    std::cin >> res;
    if (res == 'y') {
      return true;
    } else if (res == 'n') {
      return false;
    } 
  }
}

int spell_check(std::string word_1, std::string word_2) {
  int errors = 0;
  std::cout << "\x1B[2J\x1B[H";
  std::cout << word_2 << std::endl;

  for (int i = 0; i < word_1.size(); ++i) {
    if (word_1[i] == word_2[i]) {
      std::cout << "v";
    } else {
      std::cout << "x";
      errors ++;
    }
  }
  return errors;
}

int main(int argc, char const *argv[])
{
  std::cout << "Welcome to this dactylo skills measuring software. Words will be given to you and you will have to type them the fastest you can !" << std::endl;
  std::cin.get();
  auto dictionnary = load_dictionnary("dictionnary");
  
  bool stop_flag = false;

  while (!stop_flag) {
    std::cout << "\x1B[2J\x1B[H";
    std::cout << "Word: " << dictionnary[0] << std::endl;

    int errors = 0;

    std::string input;

    std::cin >> input;

    errors += spell_check(dictionnary[0], input);


    std::cout << "you made " << errors << " errors !" << std::endl;
    std::cin.get();

    stop_flag = prompt_stop();
  }
  



  return 0;
}
