# INI Parser
This is a library used to parse ini files.
This file parser was initially created for use on the [Soul Engine](https://github.com/ClayMason/Soul-Engine) project.

# Usage
To use the INI Parser class, include the "Config.h" header file into your project.

# Rules
The INI File parser follows the basic rules of initialization files.

A section is recognized by the line that begins with a opening square bracket with a following closing square bracket on the same line.
example `[sample section]` -> `sample section`

The parser recognizes numerous value types:
* `Bool -> True or False`
* `Int -> Integer`
* `Float -> Number that contains a decimal`
* `String -> Collection of characters that are not Bool, Int or Float`

Comments are not saved from the initial initialization file.

After parsing an INI File, the values can be altered with the provided functions and saved to the original file location or a new file destination.
