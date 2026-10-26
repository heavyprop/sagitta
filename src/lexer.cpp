#include "lexer.hpp"

#include <cctype>
#include <stdexcept>

/**
 * @file lexer.cpp
 * @brief Implements the tokenization of source text.
 *
 * Recognizes integers, strings, identifiers and Fn keyword,
 * punctuation, and the -> operator. White space is skipped.
 * Reports unterminated strings and unexpected characters.
 */

Lexer::Lexer(const std::string& input) : source_(input) {}

char Lexer::current() const {
    if (position_ >= source_.size()) {
        return '\0';
    }

    // string indexing
    return source_[position_];
}

// looks one character ahead without moving, used for '->' and negative numbers
char Lexer::peek() const {
    if (position_ + 1 >= source_.size()) {
        return '\0';
    }

    return source_[position_ + 1];
}

void Lexer::advance() {
    // never moves past the end, so current() keeps returning '\0'
    if (position_ < source_.size()) {
        ++position_;
    }
}

void Lexer::skipWhiteSpace() {
    // <cctype> requires an unsigned char value or EOF
    // on apple: isspace uses the bitmapped flag for each character
    while (std::isspace(static_cast<unsigned char>(current()))) {
        advance();
    }
}

/**
 * @brief number can have a '-' prefixed
 */
Token Lexer::readNumber() {
    std::string value{};

    if (current() == '-') {
        value += current();
        advance();
    }

    // dealing with digits still as 'chars'
    while (std::isdigit(static_cast<unsigned char>(current()))) {
        value += current();
        advance();
    }

    return {
        TokenType::Number,
        value
    };
}

/**
 * @brief identifiers start with a letter or _, then can contain letters, digits and _
 * 'fn' is reserved as a keyword
 */
Token Lexer::readIdentifier() {
    std::string value{};

    // if alpanumeric, or _ for snake case
    while (std::isalnum(static_cast<unsigned char>(current())) || current() == '_') {
        value += current();
        advance();
    }

    // check if the word is a keyword before treating it as a name
    if (value == "fn") {
        return {
            TokenType::Fn,
            value
        };
    }

    return {
        TokenType::Identifier,
        value
    };
}

/**
 * @brief strings are wrapped in double quotes, e.g. "Daniel"
 * @throws std::runtime_error If the source ends before the closing quote
 */
Token Lexer::readString() {
    std::string value{};

    // Skip the opening quote.
    advance();

    // loop until close quote or EndOfFile
    while (current() != '"' && current() != '\0') {
        value += current();
        advance();
    }

    if (current() == '\0') {
        throw std::runtime_error("Unterminated string");
    }

    // skip the closing quote
    advance();

    return {
        TokenType::String,
        value
    };
}

/**
 * @brief single character symbols, '->' is handled in nextToken() since it is two characters
 * @throws std::runtime_error If the character is not a recognised symbol
 */
Token Lexer::readSymbol() {
    char c = current();
    advance();

    switch (c) {
        case '(':
            return {
                TokenType::LeftParen,
                "("
            };

        case ')':
            return {
                TokenType::RightParen,
                ")"
            };

        case '{':
            return {
                TokenType::LeftBrace,
                "{"
            };

        case '}':
            return {
                TokenType::RightBrace,
                "}"
            };

        case '[':
            return {
                TokenType::LeftBracket,
                "["
            };

        case ']':
            return {
                TokenType::RightBracket,
                "]"
            };

        case ':':
            return {
                TokenType::Colon,
                ":"
            };

        case ';':
            return {
                TokenType::SemiColon,
                ";"
            };

        case ',':
            return {
                TokenType::Comma,
                ","
            };

        case '=':
            return {
                TokenType::Equal,
                "="
            };

        default:
            throw std::runtime_error(
                std::string("Unexpected character: ") + c
            );
    }
}

/**
 * @brief checks the current character and calls the matching read method
 */
Token Lexer::nextToken() {
    skipWhiteSpace();

    char c = current();

    // reached the end of the source
    if (c == '\0') {
        return {
            TokenType::EndOfFile,
            ""
        };
    }

    // if current char is digit, then use the readNumber() method
    if (std::isdigit(static_cast<unsigned char>(c)) ||
        (c == '-' && std::isdigit(static_cast<unsigned char>(peek())))) {
        return readNumber();
    }

    // if current char is alphanumeric, then use the readIdentifier() method
    if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
        return readIdentifier();
    }

    // if current char is quote, then use the readString() method
    if (c == '"') {
        return readString();
    }

    // if current and current + 1 char is -> then return the token ->
    if (c == '-' && peek() == '>') {
        advance();
        advance();

        return {
            TokenType::Arrow,
            "->"
        };
    }

    // no previous token rule matched, so try reading a symbol
    // readSymbol() throws an exception if the character is invalid.
    return readSymbol();

}
