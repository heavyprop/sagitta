#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "ast.hpp"
#include "token.hpp"

class Parser {
 private:
  const std::vector<Token>& tokens_;
  std::size_t position_ = 0;

  // returns current, current + 1, previous tokens, also advances to next token
  const Token& current() const;
  const Token& peek() const;
  const Token& previous() const;
  void advance();

  /**
   * @brief checks token, and advances
   * @param expected Required token
   * @param message Error message if the token does not match
   * @return A copy of the checked token
   * @throws SyntaxError If the current token has a different type
   */
  Token checkAndAdvance(TokenType expected, const std::string& message);

  // check if statement ends with semicolon
  void checkSemicolonAndAdvance();

  std::unique_ptr<Statement> parseStatement();
  std::unique_ptr<Statement> parseAssignment();
  std::unique_ptr<Statement> parsePrint();
  std::unique_ptr<Statement> parseArrayDecleration();
  std::unique_ptr<Statement> parseMethodCall();

  std::unique_ptr<Expr> parseExpression();
  std::unique_ptr<Expr> parseAddition();
  std::unique_ptr<Expr> parseMultiplication();
  std::unique_ptr<Expr> parseUnary();
  std::unique_ptr<Expr> parseValue();

  // turn text into int, error if too big
  int toInt(const Token& token, const std::string& text) const;

 public:
  /**
   * @brief Creates a parser that borrows token vector
   * @param inputTokens (The lexer's output)
   */
  explicit Parser(const std::vector<Token>& inputTokens);

  /**
   * @brief Parse every statement until the end of tokens
   * @returns the statements in the order that they apear
   * @throws SyntaxError at the first statement that is written wrong
   */
  std::vector<std::unique_ptr<Statement>> parseBlock();
};
