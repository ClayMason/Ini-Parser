#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <vector>
#include <string>
#include <string.h>
#include <cstring>
#include <fstream>
#include <cassert>
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

        try
        {
          _config.init(); // initialize config from file given
        }
        catch (ConfigParseException &e)
        {
          std::cout << "Error code " << e.get_code() << ": " << e.get_msg() << std::endl;
        }
        catch (std::exception &e)
        {
          std::cout << e.what() << std::endl;
        }

        // Test 1 : Config::addEntry() tests -- WORKS
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

        // Test 2 : Config::getSection() tests -- WORKS
        std::cout << "\nGetSection() Tests" << std::endl;
        std::cout << "Section 1:\n"
                  << *(_config.getSection("SectionOne")) << std::endl;
        std::cout << "Section 2:\n"
                  << *(_config.getSection("SectionTwo")) << std::endl;
        std::cout << "<unlabeled>:\n"
                  << *(_config.getSection(0)) << std::endl; // print the <unlabeled> section
        assert(*(_config.getSection(0)) == *(_config.getSection("<unlabeled>")));

        // std::cout << _config << std::endl; // => Config Print WORKS

        // Test 3 : ConfSection::removeEntry ()
        ConfSection *section_1 = _config.getSection("SectionOne");
        assert(section_1 != 0);
        std::cout << "\n\nTest 3 - ConfSection::remmoveEntry()\n";
        section_1->removeEntry("string1");
        section_1->removeEntry("string2");
        section_1->removeEntry("key");
        section_1->removeEntry("integer");

        // Test 4 : ConfSection::updateEntry () -- WORKS
        std::cout << "\nTest 4 - ConfSection::updateEntry()\n";
        std::cout << *section_1 << std::endl
                  << section_1->size() << std::endl
                  << std::endl;
        section_1->addEntry(std::pair<const char *, QuantumProp *>("new_entry", QuantumProp::create("this is surely a new entry")));
        std::cout << *section_1 << std::endl;
        std::cout << "\nUpdating \"new_entry\"...\n"
                  << std::endl;
        section_1->updateEntry("new_entry", QuantumProp::create("UwU"));
        std::cout << *section_1 << std::endl;

        std::cout << "Updating to a different data type." << std::endl;
        section_1->updateEntry("new_entry", QuantumProp::create(3.14));
        std::cout << *section_1 << std::endl;

        // Test 5 : ConfSection::get () -- WORKS
        QuantumProp *query_1 = section_1->get("multivalue[]");
        QuantumProp *query_2 = section_1->get("yerrr");
        assert(query_1 != 0);
        assert(query_2 == 0);
        std::cout << "Quantum value retrieved -> " << *query_1 << std::endl;

        // Test 6 :  Config::getValue(prop_)
        std::list<QuantumProp::Pair> repeats = _config.getValue("repeat");
        std::cout << "\nTest 6 -- getValue(prop)" << std::endl;
        std::cout << "size : " << repeats.size() << std::endl;

        std::list<QuantumProp::Pair>::iterator lst_itr;
        for (lst_itr = repeats.begin(); lst_itr != repeats.end(); ++lst_itr)
        {
          std::cout << "\t [Section: " << lst_itr->first << "] => repeat = " << *(lst_itr->second) << std::endl;
        }

        // Test 7 : Config::getValue(section, prop) -- Works
        std::cout << "\nTest 7 -- getValue(section, prop)" << std::endl;
        std::list<char *> sections = _config.getSections();
        std::list<char *>::iterator sect_iter = sections.begin();
        std::cout << "First Section is " << *sect_iter << std::endl;
        QuantumProp *get_test = _config.getValue(*sect_iter, "sample_get");
        std::cout << "Config.get() returned \"" << *get_test << "\"from \"sample_get\" in " << *sect_iter << " section" << std::endl;

        // Test 8 : ConfSection - [] operator overload
        std::cout << "\n Tst 8 -- ConfSection [] operator overload\n";
        ConfSection *p_sect = _config.getSection(*(++sect_iter));
        std::cout << "current_section: " << *sect_iter << std::endl;
        assert(p_sect != 0);
        ConfSection sect = *p_sect;
        std::cout << sect << std::endl;
        QuantumProp *string1_val = sect["repeat"];
        assert(string1_val != 0);
        std::cout << "repeat = " << *string1_val << std::endl;

        // Test 9 : Confin - [] operator overload
        std::cout << "\nTest 9 - Config [] operator overload\n";
        ConfSection *sample_1 = _config["SectionOne"];
        ConfSection *sample_2 = _config["section one"];
        ConfSection *sample_3 = _config[0];
        assert(sample_1 != 0);
        assert(sample_2 == 0);
        assert(sample_3 != 0);
        std::cout << "SectionOne successfully recieved" << std::endl;
        std::cout << *sample_1 << std::endl;
        std::cout << "\n<unlabeled> successfully recieved" << std::endl;
        std::cout << *sample_3 << std::endl
                  << std::endl;

        // Saving -- Works !
        std::cout
            << "Saving " << fname << " to "
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
