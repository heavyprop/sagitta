#pragma once

#include <string>

#include "environment.hpp"

/**
 * @file interpreter.hpp
 * @brief Declares the interpreter that runs one statement at a time
 */

/**
 * @brief Runs source text by passing it through the lexer, parser and environment
 *
 * owns the environment, so variables and lists stay alive between calls to run()
 */
class Interpreter {
private:
    // stores every variable and list created by the program
    Environment environment;

public:
    /**
     * @brief Tokenizes, parses and executes a single statement
     *
     * e.g. 'a = 10;', 'print(a);', '[]: items;', 'items->append(10);'
     *
     * @param input One line of source text
     * @throws std::runtime_error If the statement is invalid or uses a name that does not exist
     */
    void run(const std::string& input);
};
