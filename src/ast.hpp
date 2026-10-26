#pragma once

#include <optional>
#include <string>

#include "token.hpp"

/**
* @file ast.hpp
* @details Defines the structs that the parser uses to store the values
* interpreter uses this to fetch the value when execting the instruction
*/

/**
 * @brief This is the struct that is used for assignment statements
 *
 * stores variable name
 * stores the value defined as token in "token.hpp"
 */
struct AssignmentStatement {
    std::string name;
    Token value;
};

/**
 * @brief This is the struct that is used for print statements
 *
 * stores only the variable name
 */
struct PrintStatement {
    std::string variableName;
};

/**
 * @brief This is the struct that is used for declaring an array
 *
 * stores only the variable name
 */
struct ArrayDecleration {
    std::string name;
};

/**
 * @details Struct for when the user wants to call a method on an object
 * Examples: ->pop(), ->append();
 *
 * stoes the name of the object we want to use the method on
 * stores the name of the method
 * stores the ->(argument), which is optional, some methods don't require one
 */
struct MethodCall {
    std::string objectName;
    std::string methodName;
    std::optional<Token> argument; ///c++17 feature, represents a value that might not exist
};
