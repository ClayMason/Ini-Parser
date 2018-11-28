#include "Config.h"
#include <sstream>
#include <regex>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <fstream>
#include <utility>
#include <cassert>

Config::Config(const char *fname)
{
  this->file_name = fname;
  // default constructor - if the filename does not exist, create empty file
  // otherwise, read file
  if (!isValid(std::string(fname)))
    throw "Invalid filetype. (Expected .ini file)\n";
  std::ifstream ini_file;
  ini_file.open(fname);

  if (!ini_file)
    throw std::string("Could not open file ").append(fname).c_str();
  printf("Successfully opened %s\n", fname);

  // Parse file information
  std::string line;
  int x = 0;

  // TODO get rid of x variable ( above and in while loop and print statement )
  printf("\nContents of [%s]\n", fname);
  std::string section_label("<unlabeled>");
  char *new_section = new char[section_label.size() + 1];
  strcpy(new_section, section_label.c_str());

  while (std::getline(ini_file, line))
  {
    printf("[Line Value] => %s\n", section_label.c_str());

    if (strcmp(new_section, section_label.c_str()) != 0)
    {
      new_section = new char[section_label.size() + 1];
      strcpy(new_section, section_label.c_str());
    }

    ++x;
    const char *line_type = this->getLineType(line);
    printf("Line #%d => %s \t(Line Type = %s)\n", x, line.c_str(), line_type);

    if (strcmp(line_type, "section") == 0)
    {
      printf("\t====> Section Name: %s\n", this->extractSection(line));
      section_label = this->extractSection(line);
      printf("Section Label => %s\n", section_label.c_str());
    }
    else if (strcmp(line_type, "kvpair") == 0)
    {
      // REIMPLEMENTATION
      /*
      Double check if there exists a key with value equal to new_section, already in in_data. If not, then create the entry and make a new map.
      */
      QMap::iterator q_itr = ini_data.find(new_section);
      if (q_itr == ini_data.end())
      {
        //std::map<const char*, QuantumProp*> new_section_map;
        ConfSection new_section_map(new_section);
        std::pair<QMap::iterator, bool> new_entry = ini_data.insert(std::pair<char *, ConfSection>(new_section, new_section_map));
        q_itr = new_entry.first;
      }

      // now add the entry to the map
      // printf("P1\n");
      std::vector<const char *> kv_arr = this->extractKVPair(line);
      // printf ("\tLeft: %s\tRight: %s\n", kv_arr[0], kv_arr[1]);
      // printf("P2\n");
      std::string val = simplify(kv_arr[1]);
      // printf("P3\n");
      QuantumProp *q_prop = getValueType(val);
      // printf("P4\n");
      std::pair<const char *, QuantumProp *> entry(kv_arr[0], q_prop);
      // printf("P5\n");
      q_itr->second.addEntry(entry);
      // printf("P6\n");
    }
  } // end while
  printf("End while \n");
}

/*
* Regex strings:
- Section Match: ^(\[){1}[^[]+(]){1}$
- Comment Match: (;)+(.*)
- Key-Value: (.*)(=){1}(.*)   => Search for key and value with: [^=]+
  - For value parsing:
      - Int: [0-9]+
      - Float: [0-9]+(\.){1}[0-9]+
      - Bool: (Yes|No){1}
      - Path ? : ([a-zA-Z]+(:)?)?([\/]+)?([a-zA-Z0-9_]+(\/)?)+
*/

bool regex_isInt(std::string value)
{
  try
  {
    bool ret = std::regex_match(value.c_str(), std::regex("^[0-9]+$"));
    return ret;
  }
  catch (const std::regex_error &e)
  {
    printf("Regex Error [%d: %s\n", e.code(), e.what());
    return false;
  }
}

bool regex_isBool(std::string value)
{
  try
  {
    bool ret = std::regex_match(value.c_str(), std::regex("^(Yes|No)$"));
    return ret;
  }
  catch (const std::regex_error &e)
  {
    printf("Regex Error [%d: %s\n", e.code(), e.what());
    return false;
  }
}

bool regex_isFloat(std::string value)
{
  try
  {
    bool ret = std::regex_match(value.c_str(), std::regex("^([0-9]+)?(\\.){1}([0-9]+)?$"));
    return ret;
  }
  catch (const std::regex_error &e)
  {
    printf("Regex Error [%d: %s\n", e.code(), e.what());
    return false;
  }
}

bool regex_isPath(std::string value)
{
  try
  {
    bool ret = std::regex_match(value.c_str(), std::regex("^([A-Z]{1}(:))?(()/)?[a-zA-Z_-]+){1}(((/){1}[a-zA-Z_-]+)+)?$"));
    return ret;
  }
  catch (const std::regex_error &e)
  {
    printf("Regex Error [%d: %s\n", e.code(), e.what());
    return false;
  }
}

QuantumProp *Config::getValueType(std::string value)
{
  // determine whether a value is an int, string, path, float, bool, or nil (not applicable to any)
  // 0 = int, 1 = string, 2 = path, 3 = float, 4 = bool
  // printf ("[GET VALUE TYPE] => ");
  if (regex_isInt(value))
  {
    printf("returning int value type\n");
    std::stringstream ss(value);
    int int_val = 0;
    ss >> int_val;

    IntProp *prop = new IntProp(int_val);
    QuantumProp *master = prop;
    return master;
  }
  else if (regex_isBool(value))
  {
    printf("returning bool value type\n");
    bool bool_val;
    if (value.compare("Yes") == 0)
      bool_val = true;
    else
      bool_val = false;

    BoolProp *prop = new BoolProp(bool_val);
    QuantumProp *master = prop;
    return master;
  }
  else if (regex_isFloat(value))
  {
    printf("returning float value type\n");

    std::string new_val("0");
    new_val.append(value).append("0");
    float float_val = std::stof(new_val);

    FloatProp *prop = new FloatProp(float_val);
    QuantumProp *master = prop;
    return master;
  }
  else if (regex_isPath(value))
  {
    printf("returning path value type\n");
    PathProp *prop = new PathProp(value);
    QuantumProp *master = prop;
    return master;
  }
  else
  {
    printf("returning string value type\n");
    StringProp *prop = new StringProp(value);
    QuantumProp *master = prop;
    return master;
  }
}

std::string Config::simplify(const char *value)
{
  // given a string, simplify it by removing any empty characters that may be in front or behind
  // empty characters = (' ', '\r', '\n')
  std::string new_val(value);
  printf("@@@Simplifying [%s] =>", value);

  /* TODO 11/28/2018:
  /* Before doing anything, start from rightmost of value and move left until a ";" or "#" is found.
  /* Remove this part b/c it's 100% a comment that should not be part of the value
  */

  while (new_val.substr(0, 1).compare("\"") == 0 || new_val.substr(0, 1).compare(" ") == 0 || new_val.substr(0, 1).compare("\n") == 0 || new_val.substr(0, 1).compare("\r") == 0)
  {
    // printf ("Removing a [%s]\n", new_val.substr(0, 1).c_str());
    new_val = new_val.substr(1, new_val.size() - 1);
  }

  while (new_val.substr(new_val.size() - 1, 1).compare("\"") == 0 || new_val.substr(new_val.size() - 1, 1).compare(" ") == 0 || new_val.substr(new_val.size() - 1, 1).compare("\n") == 0 || new_val.substr(new_val.size() - 1, 1).compare("\r") == 0)
  {
    // printf ("Removing a [%s]\n",  new_val.substr(new_val.size() - 1, 1).c_str());
    new_val = new_val.substr(0, new_val.size() - 1);
  }

  printf("[%s]\n", value);
  // printf ("\t new_val => [%s]\n", new_val.c_str());
  // printf ("\t\tPrefix Loop Count: %d\n\t\tSuffix Loop Count: %d\n", x, y);
  return new_val;
}

// getLineType => Determines whether the line is a section title, comment, or key-value pair ( or none of the above )
const char *Config::getLineType(std::string line)
{
  // determine whether line is a comment, section title, key-value pair
  if (std::regex_match(line.c_str(), std::regex("^(\\[){1}.*(\\]){1}$")))
    return "section";
  else if (std::regex_match(line.c_str(), std::regex("^(;)+(.*)$")))
    return "comment";
  else if (std::regex_match(line.c_str(), std::regex("^[^=^;]+(=){1}[^=]+$")))
    return "kvpair";
  else
    return "nil";
}

// Given a line that has been identified to be a section, extract the section name
const char *Config::extractSection(std::string line)
{
  std::smatch match_;
  if (std::regex_search(line, match_, std::regex("(\\[){1}.*(\\]){1}")))
  {
    std::string result = std::string(match_[0]);
    return result.substr(1, result.length() - 2).c_str();
  }
  // if it for some damn reason chooses not to match
  printf("[ERR] No section match found.");
  return 0;
}

std::string Config::strip(std::string key)
{
  std::string new_val(key);
  printf("###Stripping [%s] => ", new_val.c_str());
  while (new_val.substr(0, 1).compare(" ") == 0)
    new_val = new_val.substr(1, new_val.size() - 1);

  while (new_val.substr(new_val.size() - 1, 1).compare(" ") == 0)
    new_val = new_val.substr(0, new_val.size() - 1);
  printf("[%s]\n", new_val.c_str());
  return new_val;
}

/* Given a line that is a key-value pair, extract the key and the value and return them
*  It returns:
*     - 0 (aka null ptr) if none found
*     - a pointer array to 2 char*, with indexes:
*           [0] key name
*           [1] value contents
*/

std::vector<const char *> Config::extractKVPair(std::string line)
{
  // Step 1 : Split by "="
  // Step 2 : Remove anything in value that's after a ";" (if there is one -- in case of inline comments)
  // Step 3 : Store the key and value into a char** and return it
  std::smatch match_;
  std::vector<const char *> kv_arr;
  if (std::regex_search(line, match_, std::regex("^[^=^;]+(=){1}[^=]+")))
  {
    std::string result = std::string(match_[0]);
    std::stringstream ss(result);
    std::string segment;
    int i = 0;

    while (i < 2 && std::getline(ss, segment, '='))
    {
      segment = strip(segment);
      char *val = new char[segment.size()];
      strcpy(val, segment.c_str());
      kv_arr.push_back(val);
      i++;
    }
  }
  return kv_arr;
}

// Given a section name, return a pair<map<>, bool>.
// the map pointer may be assigned or be 0. If the bool is false, then the map is null, otherwise, it is not.
ConfSection *Config::getSection(const char *section_name)
{
  char s_name[strlen(section_name) + 1];
  strcpy(s_name, section_name);
  Config::QMap::iterator q_itr = ini_data.find(s_name);
  if (q_itr == ini_data.end())
    return 0;
  else
    return &(q_itr->second);
}

// Given section name, and property key, return pointer to property value if exists, or null ptr otherwise
QuantumProp *Config::getValue(const char *section_name, const char *prop_key)
{
  ConfSection *section = this->getSection(section_name);
  if (section == 0)
    return 0;
  return section->get(prop_key);
}

std::list<std::pair<char *, QuantumProp *>> Config::getValue(const char *prop_key)
{
  QMap::iterator q_itr = ini_data.begin();
  std::list<std::pair<char *, QuantumProp *>> lst;
  while (q_itr != ini_data.end())
  {
    QuantumProp *val = this->getValue(q_itr->first, prop_key);
    if (val != 0)
    {
      std::pair<char *, QuantumProp *> p(q_itr->first, val);
      lst.push_back(p);
    }
    ++q_itr;
  }
  return lst;
}
//unlabeled
bool Config::addEntry(char *section, std::pair<const char *, QuantumProp *> entry)
{
  if (section == 0)
  {
    section = new char[12];
    strcpy(section, "<unlabeled>");
  }
  // section should not be null anymore...
  QMap::iterator q_itr = ini_data.find(section);

  /// DEBUGGING ///
  bool CHECK = q_itr == ini_data.end();
  printf("Result when trying to add [%s] => ", entry.first);
  if (CHECK)
    printf("Section %s does not exist. Creating [%s]\n", section, section);
  else
    printf("Section %s already exists. Adding to existing...\n", section);
  /// END DEBUGGING ///

  if (q_itr == ini_data.end())
  { // if the iterator could not find the section, create the section and add it
    // create the section and add it to the list
    ConfSection s(section);
    std::pair<QMap::iterator, bool> ret = ini_data.insert(std::pair<char *, ConfSection>(section, s));
    q_itr = ret.first;
  }
  else
    delete section; // section exists already so delete the new variable created in 1st if.
  return q_itr->second.addEntry(entry);
}

void Config::save(const char *fname) const
{
  // save the ini_data into fname -- should end in .ini
  printf("Saving to %s\n", fname);
  if (!isValid(std::string(fname)))
  {
    printf("Error Saving -- file name [%s] is invalid\n", fname);
    exit(EXIT_FAILURE);
  }
  else
  {
    printf("File is valid ...\n");
    std::ofstream out_file;
    out_file.open(fname);

    // write data in <unlabeled> section at the top before writing the rest
    printf("A\n");
    char to_find[12];
    strcpy(to_find, "<unlabeled>");
    printf("B\n");
    QMap::const_iterator q_itr = ini_data.find(to_find);
    ConfSection::SMap::const_iterator s_itr;
    if (q_itr != ini_data.end())
    {
      printf("C\n");
      s_itr = q_itr->second.begin();
      printf("Size: %d\n", q_itr->second.size());
      printf("D\n");
      while (s_itr != q_itr->second.end())
      {
        printf("Writing kv:");
        printf("%s", s_itr->first);
        printf("=");
        printf("%s", s_itr->second->strValue());
        printf("\n");
        out_file << s_itr->first << " = " << s_itr->second->strValue() << "\n";
        ++s_itr;
      }
    }
    printf("E\n");
    // iterate through all the sections in ini_data -- except <unlabeled>
    q_itr = ini_data.begin();
    while (q_itr != ini_data.end())
    {
      if (strcmp(q_itr->first, "<unlabeled>") != 0)
      {
        // Write the section name
        out_file << "[" << q_itr->first << "]\n";

        // Write the key-value pairs
        out_file << "\n";
        // read contents
        s_itr = q_itr->second.begin();
        while (s_itr != q_itr->second.end())
        {
          out_file << s_itr->first << " = " << s_itr->second->strValue() << "\n";
          ++s_itr;
        }
        out_file << "\n";
      }
      ++q_itr;
    }

    out_file.close();
  }
}

void Config::save() const
{
  // save the ini_data in the same ini_file as specified
  save(this->file_name);
}

void Config::save(std::string fname) const
{
  save(fname.c_str());
}

// ConfSection -- Helper Functions
std::ostream &operator<<(std::ostream &os, const ConfSection &c_section)
{
  ConfSection::SMap::const_iterator s_itr = c_section.sect_map.begin();
  while (s_itr != c_section.sect_map.end())
  {
    os << "\t" << s_itr->first << " = ";
    // get the values
    if (strcmp(s_itr->second->getType(), "int") == 0)
    {
      IntProp *int_prop = static_cast<IntProp *>(s_itr->second);
      os << int_prop->value;
    }
    else if (strcmp(s_itr->second->getType(), "bool") == 0)
    {
      BoolProp *bool_prop = static_cast<BoolProp *>(s_itr->second);
      if (bool_prop->value)
        os << "True";
      else
        os << "False";
    }
    else if (strcmp(s_itr->second->getType(), "string") == 0)
    {
      StringProp *str_prop = static_cast<StringProp *>(s_itr->second);
      os << str_prop->value;
    }
    else if (strcmp(s_itr->second->getType(), "float") == 0)
    {
      FloatProp *float_prop = static_cast<FloatProp *>(s_itr->second);
      os << float_prop->value;
    }
    else if (strcmp(s_itr->second->getType(), "path") == 0)
    {
      PathProp *path_prop = static_cast<PathProp *>(s_itr->second);
      os << path_prop->value;
    }
    os << "\n";
    ++s_itr;
  }

  return os;
}

// Config -- Helper Functions
std::ostream &operator<<(std::ostream &os, const Config &conf)
{
  // Print out the values stored in the map of ini_data in config

  Config::QMap::const_iterator q_itr = conf.ini_data.begin();
  while (q_itr != conf.ini_data.end())
  {
    // os << "KEY: " << q_itr->first << "\n";
    os << "~ Section [" << q_itr->first << "] ~\n"
       << q_itr->second;
    ++q_itr;
  }

  return os;
}

ConfSection *Config::operator[](const char *section_name)
{
  char s_name[strlen(section_name)];
  strcpy(s_name, section_name);
  QMap::iterator itr = ini_data.find(s_name);
  if (itr == ini_data.end())
    return 0;
  return &(itr->second);
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////CONF_SECTION IMPLEMENTATION/////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

ConfSection::ConfSection(const char *sect_name)
{
  this->section_name = sect_name;
}

bool ConfSection::addEntry(std::pair<const char *, QuantumProp *> new_entry)
{
  // return true if anything was added
  if (new_entry.second->strValue() == 0)
    return false;
  else
  {
    printf("Attempting to add [%s=%s]\n", new_entry.first, new_entry.second->strValue());
    std::pair<SMap::iterator, bool> result = sect_map.insert(std::pair<const char *, QuantumProp *>(new_entry.first, new_entry.second));
    if (result.second)
      printf("Entry added ... size = %d\n", this->size());
    else
      printf("Error adding %s to %s ... \n", new_entry.first, this->section_name);
    return result.second;
  }
}

bool ConfSection::removeEntry(const char *key_name)
{
  // return true if anything was erased
  int result = sect_map.erase(key_name);
  return result > 0;
}

QuantumProp *ConfSection::get(const char *key_name)
{
  // find key_name from sect_map
  SMap::iterator res = sect_map.find(key_name);
  if (res == sect_map.end())
    return 0;
  else
    return res->second;
}

bool ConfSection::updateEntry(std::pair<const char *, QuantumProp *> updated_entry)
{
  // check if the updated_entry->first exists
  SMap::iterator s_itr = sect_map.find(updated_entry.first);
  if (s_itr == sect_map.end())
    return false;
  else
    s_itr->second = updated_entry.second;

  return true;
}
