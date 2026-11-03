#include <unistd.h>

#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "error.hpp"
#include "interpreter.hpp"

/**
 * @file main.cpp
 * @brief Entry point for Sagitta
 *
 * ./sagitta starts the interactive REPL
 * ./sagitta file.sgt runs the source file
 */

// ANSI colour code, the terminal reads these as instructions
static const char* const BOLD = "\033[1m";
static const char* const RED = "\033[1;31m";
static const char* const BLUE = "\033[1;34m";
static const char* const RESET = "\033[0m";

// only colour the output when it goes to a terminal, not into a file or another
// program, and turn it off if the user has set NO_COLOR
static const char* colour(const char* code) {
  static const bool enabled =
      isatty(STDERR_FILENO) && std::getenv("NO_COLOR") == nullptr;
  return enabled ? code : "";
}

/**
 * @brief Prints a ^ under the column where the error is
 *
 * @param indent How many character come before the line of the screen
 * @param column The column of the mistake, starting at 1
 */
static void printCaret(std::size_t indent, std::size_t column) {
  std::cerr << std::string(indent + column - 1, ' ') << "^\n";
}

// returns one line of the source, lines start at 1
static std::string getLine(const std::string& source, std::size_t lineNumber) {
  std::istringstream stream(source);
  std::string line;

  for (std::size_t i = 0; i < lineNumber; ++i) {
    std::getline(stream, line);
  }

  return line;
}

/**
 * @brief Reads one line at a time from the user and runs it
 *
 * type 'exit' to quit
 */
static int runRepl() {
  /// created once, so variables and lists are kept between lines
  Interpreter interpreter;
  std::string input{};

  while (true) {
    std::cout << ">> ";

    // stop athe end end of input
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
      interpreter.run(input);
    }
    // point at the mistake under what the user typed, 3 is the width of >>
    catch (const CodeError& error) {
      std::cerr << colour(BLUE);
      printCaret(3, error.column());
      std::cerr << colour(RED) << "Error: " << colour(RESET) << error.what()
                << '\n';
    }
    // print the error and keep the REPL running
    catch (const std::exception& error) {
      std::cerr << "Error: " << error.what() << '\n';
    }
  }
  return 0;
}

/**
 * @brief Runs a whole source file, stopping at the first error
 *
 * @param path Path to the source file
 * @return 0 on success, 1 if the file could not be read or the program failed
 */
static int runFile(const std::string& path) {
  std::ifstream file(path);

  if (!file.is_open()) {
    std::cerr << "Could not open file: " << path << '\n';
    return 1;
  }

  // reads the whole file, so statements can go over more than one line
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string source = buffer.str();

  Interpreter interpreter;

  try {
    interpreter.run(source);
  }
  // show the line with a ^ under the mistake
  catch (const CodeError& error) {
    std::cerr << colour(BOLD) << path << ":" << error.line() << ":"
              << error.column() << ": " << colour(RED)
              << "error: " << colour(RESET) << colour(BOLD) << error.what()
              << colour(RESET) << '\n';
    std::cerr << "    " << getLine(source, error.line()) << '\n';
    std::cerr << colour(BLUE);
    printCaret(4, error.column());
    std::cerr << colour(RESET);
    return 1;
  } catch (const std::exception& error) {
    std::cerr << path << ": " << error.what() << '\n';
    return 1;
  }

  return 0;
}

int main(int argc, char* argv[]) {
  // no arguments, so start the REPL
  if (argc == 1) {
    return runRepl();
  }

  // one argument the parth to the source file
  if (argc == 2) {
    return runFile(argv[1]);
  }

  std::cerr << "Usage: " << argv[0] << " [file.sgt]\n";
  return 1;
}
