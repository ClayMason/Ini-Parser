#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <fstream>
#include <string>
#include <map>
#include <utility>
#include <list>
#include <stdio.h>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <vector>

#define DEBUG true

/*
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DOCUMENTATION
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

* class QuantumProp:
*   - constructors:
*       - QuantumProp() - initializes empty QuantumProp object. type and str_value are null pointers
*       - QuantumProp (const char* t) - init QuantumProp w/ type = t and str_value as null.
*
*  - functions:
*       - bool isBool () - returns whether QuantumProp is parent of a BoolProp object
*       - bool isInt () - returns whether QuantumProp is parent of a IntPorp object
*       - bool isFloat () - returns whether QuantumProp is parent of a FloatProp object
*       - bool isString () - returns whether QuantumProp is parent of a StringProp object
*       - bool isPath () - returns whether QuantumProp is parent of a PathProp object
*       - bool getType () - returns the type of property QuantumProp holds
*       - bool strValue () - returns the string value of the property
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
* class BoolProp (public QuantumProp):
*   - constructor:
*       - BoolProp (bool v) - initialize BoolProp with value of v. Sets QuantumProp->str_value as string value of v
*
*   - functions:
*       - char* stringify(bool v) - returns a char* version of the boolean v
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
* class IntProp (public QuantumProp):
*   - constructor:
*       - BoolProp (int v) - initialize BoolProp with value of v. Sets QuantumProp->str_value as string value of v
*
*   - functions:
*       - char* stringify(int v) - returns a char* version of the int v
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
* class StringProp (public QuantumProp):
*   - constructor:
*       - BoolProp (std::string v) - initialize BoolProp with value of v. Sets QuantumProp->str_value as string value of v
*
*   - functions:
*       - char* stringify(std::string v) - returns a char* version of the string v
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
* class floatProp (public QuantumProp):
*   - constructor:
*       - BoolProp (float v) - initialize BoolProp with value of v. Sets QuantumProp->str_value as string value of v
*
*   - functions:
*       - char* stringify(float v) - returns a char* version of the float v
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
* class PathProp (public QuantumProp):
*   - constructor:
*       - BoolProp (std::string v) - initialize BoolProp with value of v. Sets QuantumProp->str_value as string value of v
*
*   - functions:
*       - char* stringify(std::string v) - returns a char* version of the string v
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
* class Config:
*   - constructor:
*       - Config (const char* fname) - Given the filename, if it exists and is a valid ini file, parse the file sections, and key-value properties
*
*   - functions:
*       - ConfSection* getSection (const char* name) - Given a section name, return a pointer to ConfSection. If the section doesn't exist, the pointer is null (==0)
*       - QuantumProp* getValue(const char* section, const char* prop) - returns a pointer to a QuantumProp given a section and a property key. If the property
*                 doesn't exist, the pointer is null.
*       - std::list<std::pair<char*, QuantumProp*>> getValue(cont char* prop) - given a property, returns a list to a pair of char* and QuantumProp pointer which are all property values with the key of prop.
*                 (different sections can have same prop). The first of pair is the section which the property exists within
*       - bool addEntry(char* section, std::pair<const char*, QuantumProp*> entry) - Add an entry to section named section, if it exists. Returns true if anything is added.
*       - void save (const char* fname)
*       - void save (std::string fname) - save the data into the given fname. If the file exists, it will be overwritten
*       - void save () - save to original filename that initialized the config file.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
* class ConfSection:
*   - constructor:
*       - ConfSection (const char* sect_name) - initialize a ConfSection with name sect_name
*
*   - functions:
*       - bool addEntry (std::pair<const char*, QuantumProp*> new_entry) - add key-value pair to the map, with key of type const char* and value of type QuantumProp*. Returns true if anything was added.
*       - bool removeEntry (const char* key_name) - Remove an entry, if exists, with the key name of key_name. Returns true if anything was removed.
*       - bool updateEntry (std::pair<const char*, QuantumProp*> updated_entry) - Updates an entry in the map, if it exists. Returns true if anything was updated.
*       - QuantumProp* get (const char* name) - Returns a pointer to the QuantumProp with key of name. A null pointer is returned if it doesn't exist.
*       - SMap::iterator begin () - returns an iterator to the beginning of the section map.
*       - SMap::const_iterator begin () - returns a const iterator to the beginning of the section map.
*       - SMap::iterator end () - returns an iterator to the end of the section map.
*       - SMap::const_iterator end () - returns a const iterator to the end of the section map.
*       - int save () - returns the size of map containing the key-value properties (number of entries).
*/
class QuantumProp
{
protected:
  const char *type;
  char *str_value;

public:
  typedef std::pair<char *, QuantumProp *> Pair;

  QuantumProp() : type(0), str_value(0) {}
  QuantumProp(const char *_type) : type(_type), str_value(0) {}
  ~QuantumProp() {}

  // accessors
  bool isBool() { return type != 0 && strcmp(type, "bool") == 0; }
  bool isInt() { return type != 0 && strcmp(type, "int") == 0; }
  bool isFloat() { return type != 0 && strcmp(type, "float") == 0; }
  bool isString() { return type != 0 && strcmp(type, "string") == 0; }
  bool isPath() { return type != 0 && strcmp(type, "path") == 0; }
  const char *getType() { return type; }
  char *strValue() { return str_value; }

  // printing
  friend std::ostream &operator<<(std::ostream &os, const QuantumProp &prop);

  /*
   * Create static function should convert the given type into a QuantumProp object
   * that can be downcasted to its appropriate child class (IntProp, BoolProp, etc.).
  */
  static QuantumProp *create(int val);
  static QuantumProp *create(float val);
  static QuantumProp *create(double val);
  static QuantumProp *create(char *val);
  static QuantumProp *create(const char *val);
  static QuantumProp *create(std::string val);
  static QuantumProp *create(bool val);
  static QuantumProp *create(char *val, bool is_path);
  static QuantumProp *create(const char *val, bool is_path);
  static QuantumProp *create(std::string val, bool is_path);
};

////////////////////////////INT PROP////////////////////////////////
class IntProp : public QuantumProp
{
public:
  int value;
  IntProp(int val) : QuantumProp("int"), value(val) { this->str_value = stringify(val); }
  ~IntProp() {}

private:
  char *stringify(int v)
  {
    std::string s = std::to_string(v);
    char *str_val = new char[s.size() + 1];
    strcpy(str_val, s.c_str());
    return str_val;
  }
};

////////////////////////////BOOL PROP////////////////////////////////
class BoolProp : public QuantumProp
{
public:
  bool value;
  BoolProp(bool val) : QuantumProp("bool"), value(val) { this->str_value = stringify(val); }
  ~BoolProp() {}

private:
  char *stringify(bool v)
  {
    char *str_val;
    if (v)
    {
      str_val = new char[4];
      strcpy(str_val, "Yes");
    }
    else
    {
      str_val = new char[3];
      strcpy(str_val, "No");
    }
    return str_val;
  }
};

////////////////////////////STRING PROP////////////////////////////////
class StringProp : public QuantumProp
{
public:
  std::string value;
  StringProp(std::string val) : QuantumProp("string"), value(val) { this->str_value = stringify(val); }
  ~StringProp() {}

private:
  char *stringify(std::string val)
  {
    char *str_val = new char[val.size() + 1];
    strcpy(str_val, val.c_str());
    return str_val;
  }
};

////////////////////////////FLOAT PROP////////////////////////////////
class FloatProp : public QuantumProp
{
public:
  float value;
  FloatProp(float val) : QuantumProp("float"), value(val) { this->str_value = stringify(val); }
  ~FloatProp() {}

private:
  char *stringify(float val)
  {
    std::string s_val = std::to_string(val);
    char *str_val = new char[s_val.size() + 1];
    strcpy(str_val, s_val.c_str());
    return str_val;
  }
};

////////////////////////////PATH PROP////////////////////////////////
class PathProp : public QuantumProp
{
public:
  std::string value;
  PathProp(std::string val) : QuantumProp("path"), value(val) { this->str_value = stringify(val); }
  ~PathProp() {}

private:
  char *stringify(std::string val)
  {
    char *str_val = new char[val.size() + 1];
    strcpy(str_val, val.c_str());
    return str_val;
  }
};

//////////////////////////////COMPARISON FUNC//////////////////////////////////
struct cmp_str
{
  bool operator()(const char *a, const char *b) const
  {
    return std::strcmp(a, b) < 0;
  }
};
///////////////////////////////////////////////////////////////////////////////

class ConfSection;
class ConfigParseException;
////////////////////////////CONFIGURATION///////////////////////////////////////////////////////////////////////
class Config
{

public:
  // typedef std::map<char*, std::map<const char*, QuantumProp*>> QMap;
  typedef std::map<char *, ConfSection, cmp_str> QMap;

  Config(const char *fname);

  // operator overloads
  friend std::ostream &operator<<(std::ostream &os, const Config &conf);

  // functionality
  // TODO: make init function and add error handeling.

  std::list<char *> getSections();
  ConfSection *getSection(const char *section_name);
  QuantumProp *getValue(const char *section_name, const char *prop_key);
  std::list<QuantumProp::Pair> getValue(const char *prop_key);
  bool addEntry(char *section, std::pair<const char *, QuantumProp *> entry);
  void save() const;
  void save(const char *fname) const;
  void save(std::string fname) const;

private:
  // map of list to represent each key-value pair, separated by sections, within the list
  QMap ini_data;
  const char *file_name;

  bool isValid(std::string fname) const
  {
    return fname.substr(fname.size() - 4, 4).compare(".ini") == 0;
  }

  std::string strip(std::string key);
  std::string simplify(const char *value);      // removes excess apostrophes, spaces, or line breaks in start and end of string
  QuantumProp *getValueType(std::string value); // returns the value type as a QuantumProp
  const char *getLineType(std::string line);    // returns the type of the line based on its structure

  // extracting values from lines
  const char *extractSection(std::string line);
  std::vector<const char *> extractKVPair(std::string line);
  ConfSection *operator[](const char *section_name);
};

////////////////////////////CONFSECTION///////////////////////////////////////////////////////////////////////
class ConfSection
{
public:
  // TODO: might not need sect_name -- repetition
  ConfSection(const char *sect_name);
  typedef std::map<const char *, QuantumProp *, cmp_str> SMap;

  friend std::ostream &operator<<(std::ostream &os, const ConfSection &c_section);

  // modifiers
  bool addEntry(std::pair<const char *, QuantumProp *> new_entry); // works
  bool removeEntry(const char *key_name);
  bool updateEntry(std::pair<const char *, QuantumProp *> updated_entry);
  bool updateEntry(const char *update_key, QuantumProp *updated_val);
  QuantumProp *get(const char *key_name);

  // accessors
  SMap::iterator begin() { return sect_map.begin(); }
  SMap::iterator end() { return sect_map.end(); }
  SMap::const_iterator begin() const { return sect_map.begin(); }
  SMap::const_iterator end() const { return sect_map.end(); }
  int size() const { return sect_map.size(); }
  bool operator==(const ConfSection &other);
  const char *get_name() { return section_name; }

private:
  const char *section_name;
  SMap sect_map;
};

////////////////////////////CONFIG PARSE EXCEPTION///////////////////////////////////////////////////////////////////////
class ConfigParseException : std::logic_error
{

private:
  unsigned int err_code;

public:
  // Error codes
  static const unsigned int MISSING_FILE = 1;
  static const unsigned int SAVE_ERROR = 2;

  ConfigParseException(unsigned int code_, const char *msg) : std::logic_error(msg)
  {
    this->err_code = code_;
  }

  unsigned int get_code() { return this->err_code; }

  /*
      * Given an error code, return a corresponding message that describes the code
    */
  static const char *get_error_message(unsigned int err_code)
  {
    switch (err_code)
    {
    case ConfigParseException::MISSING_FILE:
      return "Could not find the file specified";
      break;
    case ConfigParseException::SAVE_ERROR:
      return "Problem saving the data to the ini config file";
      break;
    default:
      return "undefined error";
    }
  }
};
#endif
