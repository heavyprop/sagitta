#pragma once

#include <cstddef>
#include <string>

#include "token.hpp"

/**
 * @file lexer.hpp
 * @brief Declares the lexer that converts souce text into tokens
 */

// Reads tokens sequentially from source text.
class Lexer {
private:
    // the full source text being tokenized
    std::string source_;
    // index of the current character in source_
    std::size_t position_ = 0;

    // returns the current character, or '\0' if it has reached the end of the source
    char current() const;
    // returns the next character without advancing, or '\0' if there isn't one
    char peek() const;
    // moves to the next character if it has not reached the end of the source
    void advance();
    // skips spaces, tabs and new lines
    void skipWhiteSpace();

    // reads an integer such as 10 or -5
    Token readNumber();
    // reads a variable name, method name, or the fn keyword
    Token readIdentifier();
    // reads the text between double quotes, the quotes are not stored
    // @throws std::runtime_error If the closing quote is missing
    Token readString();
    // reads a single character symbol such as ( ) { } [ ] : ; , =
    // @throws std::runtime_error If the character is not recognised
    Token readSymbol();

public:
    /**
     * @param input Source text to tokenize
     */
    Lexer(const std::string& input);

    /**
     * @brief Reads the next token and advances through the source
     * @return The next token, or an end EndOfFile token when finished
     */
    Token nextToken();
};
