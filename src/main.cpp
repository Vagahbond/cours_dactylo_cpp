#include <iostream>
#include <fstream>
#include <experimental/filesystem>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdlib>

// redefine namespace for better usability
namespace fs = std::experimental::filesystem;

// load the dictionnary of possible words from a file
std::vector<std::string> load_dictionnary(const char *path)
{
  if (!fs::exists(path))
  {
    std::cerr << "Please provide a dictionnary !" << std::endl;
    throw std::invalid_argument(std::string("Could not read dictionnary file."));
  }

  std::ifstream stream(path, std::ios_base::in);

  std::vector<std::string> res;
  std::string temp;

  while (std::getline(stream, temp))
  {
    res.push_back(temp);
  }

  return res;
}

// prompt the user to know if they want to exit the app. Correct answer required to get out of the loop.
bool prompt_stop()
{
  while (true)
  {
    std::cout << "\x1B[2J\x1B[H";
    std::cout << "Exit the program? (y/n)" << std::endl;
    char res;
    std::cin >> res;
    if (res == 'y')
    {
      return true;
    }
    else if (res == 'n')
    {
      return false;
    }
  }
}

// check errors and print them individualy
int spell_check(std::string source, std::string check)
{
  int errors = 0;
  std::cout << "\x1B[2J\x1B[H";
  std::cout << "Errors for : " << std::endl;
  std::cout << check << std::endl;

  for (int i = 0; i < source.size(); ++i)
  {
    if (source[i] == check[i])
    {
      std::cout << " ";
    }
    else
    {
      std::cout << "^";
      errors++;
    }
  }

  for (int j = source.length(); j < check.length(); ++j)
  {
    std::cout << "x";
  }

  std::cout << std::endl;

  for (int i = 0; i < source.size(); ++i)
  {
    if (source[i] == check[i])
    {
      std::cout << " ";
    }
    else
    {
      std::cout << source[i];
      errors++;
    }
  }

  std::cout << std::endl;

  if (errors == 0)
  {
    std::cout << "Perfect !" << std::endl;
  }
  return errors;
}

std::vector<std::pair<std::string, std::string>> prompt_words(int nb_words, std::vector<std::string> &dictionnary)
{
  std::vector<std::pair<std::string, std::string>> res;

  for (int i = 0; i < nb_words; ++i)
  {
    std::string word = dictionnary[rand() % (dictionnary.size() - 1)];

    std::string input;
    std::cout << "\x1B[2J\x1B[H";
    std::cout << "Word: " << word << std::endl;
    std::cin >> input;

    res.push_back(std::make_pair(word, input));
  }

  return res;
}

void display_errors(std::vector<std::pair<std::string, std::string>> words)
{
  std::cout << "\x1B[2J\x1B[H";
  std::cout << "Let's see your errors !" << std::endl;
  std::cin.get();
  int errors = 0;
  for (int i = 0; i < words.size(); ++i)
  {
    errors += spell_check(words[i].first, words[i].second);
    std::cin.get();
  }

  std::cout << "\x1B[2J\x1B[H";
  std::cout << "you made " << errors << " errors !" << std::endl;
  std::cin.get();
}

void main_loop(std::vector<std::string> dictionnary)
{
  bool stop_flag = false;

  while (!stop_flag)
  {
    int nb_words = 0;

    std::cout << "\x1B[2J\x1B[H";

    std::cout << "Game start ! How many words are you willing to type?" << std::endl;
    std::cin >> nb_words;
    std::cout << "\x1B[2J\x1B[H";

    std::cout << nb_words << " words will be prompted. Press any key when you're ready !" << std::endl;
    std::cin.get();

    auto words = prompt_words(nb_words, dictionnary);

    display_errors(words);

    stop_flag = prompt_stop();
  }
}

int main(int argc, char const *argv[])
{
  std::cout << "Welcome to this dactylo skills measuring software. Words will be given to you and you will have to type them the fastest you can !" << std::endl;
  std::cin.get();

  auto dictionnary = load_dictionnary("dictionnary");

  main_loop(dictionnary);

  return 0;
}
