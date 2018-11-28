#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <vector>
#include <string>
#include <string.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include "Config.h"

std::vector<char *> get_files(const char *dir_name);

int main()
{

  // Open all the files in samples and test open it in Configuration
  // one by one
  std::vector<char *> files = get_files("./samples");
  printf("Sample => size: %lu\n", (unsigned long)files.size());
  for (size_t i = 0; i < files.size(); ++i)
  {

    // If it is an ini file...
    std::string fname(files[i]);
    if (fname.substr(fname.size() - 3, 3) == "ini")
    {
      printf("Reading [%s] ... \n", fname.c_str());
      // open the file
      std::ifstream ini_file;
      ini_file.open(std::string("samples/").append(fname));
      if (!ini_file)
        printf("Could not open [%s]\n", fname.c_str());
      else
      {
        printf("[%s] opened successfully !\n", fname.c_str());
        Config _config(std::string("samples/").append(fname).c_str());

        //bool Config::addEntry(char* section, std::pair<const char*, QuantumProp*> entry)
        for (int j = 1; j < 10; ++j)
        {
          IntProp *new_int = new IntProp(j * 1000);
          QuantumProp *q_prop = new_int;
          // char sect[5];
          // strcpy(sect, "int$");
          std::string label_ = "num_" + std::to_string(j);
          char *key_name = new char[label_.size() + 1];
          strcpy(key_name, label_.c_str());
          _config.addEntry(0, std::pair<const char *, QuantumProp *>(key_name, q_prop));
        }

        //

        std::cout << _config << std::endl;

        std::cout << "Saving " << fname << " to "
                  << "out_" << fname << std::endl;
        _config.save(std::string("out/").append(fname).c_str());
      }
    }

    printf("\n\n");
  }
  return 0;
}

std::vector<char *> get_files(const char *dir_name)
{
  struct dirent *entry;
  DIR *dir = opendir(dir_name);
  std::vector<char *> files;

  if (dir == 0)
  {
    printf("ERR: Directory not found.\n");
    return files;
  }

  while ((entry = readdir(dir)) != 0)
  {

    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
      files.push_back(entry->d_name);
  }

  return files;
}
