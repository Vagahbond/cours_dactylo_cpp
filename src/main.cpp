#include <iostream>
#include <fstream>
#include <experimental/filesystem>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <atomic>
#include <time.h>

// redefine namespace for better usability
namespace fs = std::experimental::filesystem;

typedef struct {
  std::string source;
  std::string input;
  size_t time;
} type_result;

// convert decaseconds to seconds
double decaseconds_to_seconds(size_t decaseconds)
{
  return static_cast<long double>(decaseconds) / 1000;
}

// Timer routine
void count_time(std::atomic<size_t> &time_value, std::atomic<bool> &stop_flag)
{
  while (stop_flag.load() == false)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    size_t current_time = time_value.load();

    time_value.store(current_time + 10);
  }
}

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

// Finds errors, indicates their location, and return how many there were
int find_and_point_typos(std::string source, std::string check)
{
  int errors = 0;
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

  for (int j = source.size(); j < check.size(); ++j)
  {
    std::cout << "x";
  }


  std::cout << std::endl;

  return errors;
}

void print_base_character_when_error(std::string source, std::string check)
{
  for (int i = 0; i < source.size(); ++i)
  {
    if (source[i] == check[i])
    {
      std::cout << " ";
    }
    else
    {
      std::cout << source[i];
    }
  }


  std::cout << std::endl;
}

// check errors and print them individualy
int spell_check(std::string source, std::string check, size_t time)
{

  std::cout << "\x1B[2J\x1B[H";
  std::cout << "Errors for : " << std::endl;
  std::cout << check << std::endl;

  int errors = find_and_point_typos(source, check);

  print_base_character_when_error(source, check);

  if (errors == 0)
  {
    std::cout << "Perfect !" << std::endl;
  }

  std::cout << "You took " << decaseconds_to_seconds(time) << " seconds to write it!" << std::endl;

  return errors;
}

std::string rand_word_from_dict(std::vector<std::string> dictionnary) {
  return dictionnary[rand() % (dictionnary.size() - 1)];
}

std::vector<type_result> prompt_words(int nb_words, std::vector<std::string> &dictionnary)
{
  std::vector<type_result> res;

  for (int i = 0; i < nb_words; ++i)
  {
    std::atomic<size_t> chrono(0);

    std::atomic<bool> stop_flag(false);

    std::string word = rand_word_from_dict(dictionnary);

    std::string input;
    std::cout << "\x1B[2J\x1B[H";
    std::cout << "Word: " << word << std::endl;

    std::thread chrono_thread(count_time, std::ref(chrono), std::ref(stop_flag));
    std::cin >> input;

    stop_flag.store(true);

    chrono_thread.join();

    type_result word_result;
    word_result.input = input;
    word_result.source = word;
    word_result.time = chrono.load();

    res.push_back(word_result);
  }

  return res;
}

void display_errors(std::vector<type_result> words)
{
  std::cout << "\x1B[2J\x1B[H";
  std::cout << "Let's see your errors !" << std::endl;
  std::cin.get();

  int errors = 0;
  size_t total_time = 0;
  size_t total_letters = 0;

  for (int i = 0; i < words.size(); ++i)
  {
    errors += spell_check(words[i].source, words[i].input, words[i].time);
    total_time += words[i].time;
    total_letters += words[i].source.size();

    std::cin.get();
  }

  double total_time_seconds = decaseconds_to_seconds(total_time);

  std::cout << "\x1B[2J\x1B[H";
  std::cout << "You made " << errors << " errors!" << std::endl;
  std::cout << "It took you " << total_time_seconds << " seconds to write " << words.size() << " words." << std::endl;
  std::cout << "That's an average of " << total_time_seconds / words.size() << " seconds per word, or " << total_time_seconds / total_letters << " per letter!" << std::endl;
  std::cout << "it is equivalent to " << (60 * words.size()) / total_time_seconds << " words or " << (60 * total_letters) / total_time_seconds << " letters in a minute." << std::endl;

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
  /* initialize random seed: */
  srand(time(NULL));

  std::cout << "Welcome to this dactylo skills measuring software. Words will be given to you and you will have to type them the fastest you can !" << std::endl;
  std::cin.get();

  auto dictionnary = load_dictionnary("dictionnary");

  main_loop(dictionnary);

  return 0;
}
