#pragma once

#include <cstddef>
#include <string>

#include "token.hpp"

/**
 * @file lexer.hpp
 * @brief Declares the lexer that converts source text into tokens
 */

// reads tokens sequentially from source text
class Lexer {
 private:
  // the full source being tokenized
  // index of current token (which is a char in the source)
  std::string source_;
  std::size_t position_{0};

  // used for error messages
  std::size_t line_ = 1;
  std::size_t column_ = 1;

  char current() const;
  char peek() const;
  void advance();
  void skipWhiteSpace();

  // reads a whole number, '-' in front is handled by the parser
  Token readNumber();

  // reads a variable name, method name or the fn keyword
  Token readIdentifier();

  // reads the text between double quotes, the quotes are NOT stored
  // @throws SyntaxError if the closing quote is missing
  Token readString();

  // reads a single character symbol such ()...
  // @throws SyntaxError if the character is not recognised
  Token readSymbol();

  // works out which kind of token is next and reads it
  Token readToken();

  // for reading comments, just ignore everything until \n
  void readComment(); 

 public:
  /**
   * @param input Source text to tokenize
   */
  Lexer(const std::string& input);

  /**
   * @brief Reads the next token and traverses through the source
   * @return the next token, or an end of file token when finished
   */
  Token nextToken();
};
