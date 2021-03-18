#include <iostream>
#include <vector>
#include <string>
#include <experimental/filesystem>
#include <fstream>
#include <stdlib.h>
#include <thread>
#include <chrono>
#include <atomic>
#include <unistd.h> 



namespace fs = std::experimental::filesystem;

typedef struct {
  std::string source;
  std::string input;
  size_t errors;
  size_t milliseconds;
} type_result;

double mili_to_seconds(size_t milliseconds) {
  return static_cast<double>(milliseconds) / 1000;
}


void count_milliseconds(std::atomic<size_t> &time_value, std::atomic<bool> &stop_flag) {
  while(stop_flag.load() == false) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    size_t current_value = time_value.load();
    time_value.store(current_value + 10);
  }
}

std::vector<std::string> load_dictionnary(const char *path) {
  if (!fs::exists(path)) {
    std::cerr << "please give a valid file name" << std::endl;
    exit(1);
  }

  std::ifstream stream(path, std::ios_base::in);

  std::vector<std::string> words;
  std::string temp;

  while(std::getline(stream, temp)) {
    words.push_back(temp);
  }


  return words;
}

std::vector<std::string> get_random_words_from_list(size_t nb_words, std::vector<std::string> dictionnary) {
  std::vector<std::string> res;
  srand(time(NULL)); 

  for (int i = 0; i < nb_words; ++i) {
    int index = rand() % (dictionnary.size() - 1);
    res.push_back(dictionnary[index]);
  }

  return res;
}

size_t spellcheck(std::string source, std::string input) {
  size_t errors = 0;
  for (int i = 0; i < input.size(); ++i) {
    if (i >= source.size() || source[i] != input[i]) {
      errors++;
    }
  }

  for (int i = source.size(); i < input.size(); ++i) {
    errors++;
  }
  return errors;
}

std::vector<type_result> prompt_words(std::vector<std::string> words) {
  std::vector<type_result> results;
  for (int i = 0; i < words.size(); ++i) {
    std::cout << "\x1B[2J\x1B[H";
    std::cout << "type : " << words[i] << std::endl;

    type_result res;

    res.source = words[i];

    std::string input;

    std::atomic<size_t> time_value(0);

    std::atomic<bool> stop_flag(false);

    std::thread chrono_thread(count_milliseconds, std::ref(time_value), std::ref(stop_flag));

    std::cin >> input;

    
    stop_flag.store(true);

    chrono_thread.join();
    
    res.input = input;  
    res.errors = spellcheck(words[i], input);

    res.milliseconds = time_value.load();
    
    results.push_back(res);


  }
  return results;
}

void print_errors(std::string source, std::string input) {
  std::cout << input << std::endl;

  for (int i = 0; i < input.size(); ++i) {
    if(i < source.size() && source[i] != input[i]) {
      std::cout << "^";
    } else if (i < source.size()) {
      std::cout << "o";
    } else {
      std::cout << "x";
    }
  }

  std::cout << std::endl;

  for (int i = 0; i < source.size(); ++i) {
    if(i < input.size() && source[i] != input[i]) {
      std::cout << source[i];
    } else {
      std::cout << " ";
    }
  }
  std::cout << std::endl;

}

void recap(std::vector<type_result> results) {
  for (int i = 0; i < results.size(); ++i) {
    std::cout << "\x1B[2J\x1B[H";

    std::cout << "word: " << results[i].source << std::endl;
    print_errors(results[i].source, results[i].input);
    
    // std::cout << "you wrote :" << results[i].input << std::endl;

    std::cout << "you made " << results[i].errors << " errors." << std::endl;

    std::cout << "typing this word took you " << mili_to_seconds(results[i].milliseconds) << " seconds." << std::endl;

    std::cin.get();
  }
}

void prompt_words_2(std::atomic_bool &stop_flag, std::atomic_size_t &nb_word, std::vector<std::string> dictionnary) {
  std::cout << "secondary thread ID is " << getpid() << std::endl;

  while(stop_flag.load() == false) {
    std::string word = get_random_words_from_list(1, dictionnary)[0];

    std::cout << "\x1B[2J\x1B[H";

    bool error = true;

    while (error) {
      std::cout << "word : " << word << std::endl;

      std::string input;
      std::cin >> input;

      std::cout << "Try again , you were wrong" << std::endl;


      if (spellcheck(word, input) == 0) {
        error = false;
      }
      
    }
    
    size_t current_nb_words = nb_word.load();
    nb_word.store(current_nb_words + 1);

  }
}

void start_challenge(std::vector<std::string>  dictionnary) {
  std::cout << "1 minute to type a max of words" << std::endl;

  std::cin.get();

  std::atomic_bool stop_flag(false);
  std::atomic_size_t nb_words(0);

  std::thread thread(prompt_words_2, std::ref(stop_flag), std::ref(nb_words), dictionnary);

  std::this_thread::sleep_for(std::chrono::seconds(10));

  

  stop_flag.store(true);
  thread.detach();

  std::cout << "\x1B[2J\x1B[H";

  std::cout << "time's up !" << std::endl;
  std::cout << "you wrote " << nb_words.load() << " words !" << std::endl;
}

// int main(int argc, char const *argv[])
// {
//   std::vector<std::string> dictionnary = load_dictionnary("../dictionnary");

//   std::cout << "How many words do you want to type? " << std::endl;

//   size_t nb_words;
//   std::cin >> nb_words;

//   auto words = get_random_words_from_list(nb_words, dictionnary);

//   auto results = prompt_words(words);

//   recap(results); 




//   /* code */
//   return 0;
// }

int main(int argc, char const *argv[])
{
  auto dictionnary = load_dictionnary("../dictionnary");

  std::cout << "main thread ID is " << getpid() << std::endl;

  std::cout << "Welocme to this" << std::endl;

  start_challenge(dictionnary);


  return 0;
}

