#include "lexer.hpp"

#include <cctype>
#include <stdexcept>

#include "error.hpp"

/**
 * @file lexer.cpp
 * @brief Implements the tokenization of the source text
 */

Lexer::Lexer(const std::string& input) : source_(input) {}

char Lexer::current() const {
  if (position_ >= source_.size()) {
    return '\0';
  }

  // string indexing
  return source_[position_];
}

// looks one character ahead without moving
char Lexer::peek() const {
  if (position_ + 1 >= source_.size()) {
    return '\0';
  }
  return source_[position_ + 1];
}

void Lexer::advance() {
  if (position_ < source_.size()) {
    // a new line resets the column back to the start
    if (source_[position_] == '\n') {
      ++line_;
      column_ = 1;
    } else {
      ++column_;
    }
    ++position_;
  }
}

void Lexer::skipWhiteSpace() {
  // <cctype> requires an unsigned char value or EOF
  // fun fact: on apple the isspace uses the bitmapped flag for each character
  while (std::isspace(static_cast<unsigned char>(current()))) {
    advance();
  }
}

/**
 * @brief only reads the digits, -5 is reads as minus then 5
 */
Token Lexer::readNumber() {
  std::string value{};

  while (std::isdigit(static_cast<unsigned char>(current()))) {
    value += current();
    advance();
  }

  return {TokenType::Number, value};
}

/**
 * @brief identifies start with a letter or _, then can contain letters, digits
 * and _ 'fn' is keyword
 */
Token Lexer::readIdentifier() {
  std::string value{};

  // if alphanumeric or _ for snake case
  while (std::isalnum(static_cast<unsigned char>(current())) ||
         current() == '_') {
    value += current();
    advance();
  }

  if (value == "fn") {
    return {TokenType::Fn, value};
  }

  return {TokenType::Identifier, value};
}

/**
 * @brief strings are wrapped in double quotes
 * @throws SyntaxError if the source ends before the closing quotes
 */
Token Lexer::readString() {
  std::string str{};

  // remember where the line stated so an error can point
  // the line_ and column_ will change but we need to remember this point
  auto startLine = line_;
  auto startColumn = column_;

  // skip opening quote
  advance();

  // loop until close quote or end of file
  while (current() != '"' && current() != '\0') {
    str += current();
    advance();
  }

  if (current() == '\0') {
    throw SyntaxError("Unterminated string", startLine, startColumn);
  }

  // skip the closing quote
  advance();

  return {TokenType::String, str};
}

/**
 * @throws SyntaxError if the character is not recognised symbol
 */
Token Lexer::readSymbol() {
  char c = current();
  auto line = line_;
  auto column = column_;
  advance();

  switch (c) {
    case '(':
      return {TokenType::LeftParen, "("};
    case ')':
      return {TokenType::RightParen, ")"};
    case '{':
      return {TokenType::LeftBrace, "{"};
    case '}':
      return {TokenType::RightBrace, "}"};
    case '[':
      return {TokenType::LeftBracket, "["};
    case ']':
      return {TokenType::RightBracket, "]"};
    case ':':
      return {TokenType::Colon, ":"};
    case ';':
      return {TokenType::SemiColon, ";"};
    case ',':
      return {TokenType::Comma, ","};
    case '=':
      return {TokenType::Equal, "="};
    case '+':
      return {TokenType::Plus, "+"};
    // '->' is checked before in read token so can only be a minus
    case '-':
      return {TokenType::Minus, "-"};
    case '*':
      return {TokenType::Star, "*"};
    case '/':
      return {TokenType::Slash, "/"};
    default:
      throw SyntaxError(std::string("Unexpected character: ") + c, line,
                        column);
  }
}

/**
 * @brief checks the current character and calls the matching read method
 */
Token Lexer::readToken() {
  char c = current();

  // if reached the end of souce
  if (c == '\0') {
    return {TokenType::EndOfFile, ""};
  }

  // if the current char is a digit, then use the readNumber() method
  if (std::isdigit(static_cast<unsigned char>(c))) {
    return readNumber();
  }

  // if current char is alpanumeric, then use the readIdentifier() method
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

    return {TokenType::Arrow, "->"};
  }

  // no previous token rule is matched, so try reading symbol
  // read symbol throws an exception if the character is invalid
  return readSymbol();
}

Token Lexer::nextToken() {
  skipWhiteSpace();

  auto line = line_;
  auto column = column_;

  Token token = readToken();
  token.line = line;
  token.column = column;

  return token;
}
