#pragma once

#include <string>

/**
 * @token.hpp
 * @brief Defines the token types and token structure used by the lexer.
 */

/**
 * @brief Defines the types of tokens recognised by the lexer
 */
enum class TokenType {
    // Literals
    Number,
    String,
    Identifier,

    // Keywords
    Fn,

    // Symbols
    LeftParen,
    RightParen,
    LeftBrace,
    RightBrace,
    LeftBracket,
    RightBracket,

    Colon,
    SemiColon,
    Comma,

    Equal,
    Arrow,

    EndOfFile,
};

/**
 * @brief Struct token which stores the type from enum call & value.
 */
struct Token {
    TokenType type;
    std::string value;
};
