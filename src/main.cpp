#include <exception>
#include <fstream>
#include <iostream>
#include <string>

#include "interpreter.hpp"

/**
 * @file main.cpp
 * @brief Entry point for Sagitta
 *
 * ./sagitta              starts the interactive REPL
 * ./sagitta file.sgt     runs a source file
 */

/**
 * @brief Reads one line at a time from the user and runs it
 *
 * type 'exit' to quit.
 * @return The exit code, always 0
 */
static int runRepl() {
    // created once, so variables and lists are kept between lines
    Interpreter interpreter;
    std::string input{};

    while (true) {
        std::cout << ">> ";

        // stop at the end of input, e.g. ctrl + d
        if (!std::getline(std::cin, input)) {
            break;
        }

        if (input == "exit") {
            break;
        }

        // skip empty lines
        if (input.empty()) {
            continue;
        }

        try {
            // call the interpreter's method run
            interpreter.run(input);
        }
        // print the error and keep the REPL running
        catch (const std::exception& error) {
            std::cerr << "Error: " << error.what() << '\n';
        }
    }

    return 0;
}

/**
 * @brief Runs a source file one line at a time and stops at the first error
 *
 * @param path Path to the source file
 * @return 0 on success, 1 if the file could not be read or a line failed
 */
static int runFile(const std::string& path) {
    std::ifstream file(path);

    if (!file.is_open()) {
        std::cerr << "Could not open file: " << path << '\n';
        return 1;
    }

    Interpreter interpreter;
    std::string input{};

    // used in error messages to show which line failed
    std::size_t lineNumber = 0;

    while (std::getline(file, input)) {
        ++lineNumber;

        try {
            interpreter.run(input);
        }
        // report the file and line number of the error, then stop
        catch (const std::exception& error) {
            std::cerr << path << ":" << lineNumber << ": " << error.what() << '\n';
            return 1;
        }
    }

    // if read failed
    if (file.bad()) {
        std::cerr << "Could not read file: " << path << '\n';
        return 1;
    }

    return 0;
}

int main(int argc, char* argv[]) {
    // no arguments, so start the REPL
    if (argc == 1) {
        return runRepl();
    }

    // one argument, the path to the source file
    if (argc == 2) {
        return runFile(argv[1]);
    }

    std::cerr << "Usage: " << argv[0] << " [file.sgt]\n";
    return 1;
}
