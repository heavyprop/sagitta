#include "parser.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

#include "ast.hpp"
#include "error.hpp"

/**
 * @file parser.cpp
 * @brief Implements parsing tokens into a syntax tree
 */

/**
 * @brief Creates a node (a unique pointer) and copies the position of the token
 into it
 *
 * every node remembers where it came from (line and column numbers)
 * Node is the type to create: any struct that is derived from:
 *
 * Expr:
 * NumberExpr, StringExpr, VariableExpr, UnaryExpr, and BinaryExpr
 *
 * Statement:
 * AssignmentStatement, PrintStatement, ArrayDeclerationStatement, and
 MethodCallStatement
 */
template <typename Node>
static std::unique_ptr<Node> makeNode(const Token& token) {
  auto node = std::make_unique<Node>();
  node->line = token.line;
  node->column = token.column;

  return node;
}

/**
 * @brief Builds a binaryExpr, the operators position is used for errors
 */
static std::unique_ptr<Expr> makeBinary(const Token& op,
                                        std::unique_ptr<Expr> left,
                                        std::unique_ptr<Expr> right) {
  auto expression = makeNode<BinaryExpr>(op);
  // converting std::string to char
  expression->op = op.value[0];
  expression->left = std::move(left);
  expression->right = std::move(right);

  return expression;
}

Parser::Parser(const std::vector<Token>& inputTokens) : tokens_(inputTokens) {}

std::vector<std::unique_ptr<Statement>> Parser::parseBlock() {
  std::vector<std::unique_ptr<Statement>> statements;

  while (current().type != TokenType::EndOfFile) {
    statements.push_back(parseStatement());
  }
  return statements;
}

/**
 * @brief Works out which statement it is from one or two tokens
 */
std::unique_ptr<Statement> Parser::parseStatement() {
  const Token& token = current();

  if (token.type == TokenType::LeftBracket) {
    return parseArrayDecleration();
  }

  if (token.type == TokenType::Fn) {
    throw CodeError("Functions are not supported", token.line, token.column);
  }

  if (token.type == TokenType::Identifier) {
    if (peek().type == TokenType::Equal) {
      return parseAssignment();
    }

    if (peek().type == TokenType::Arrow) {
      return parseMethodCall();
    }

    if (token.value == "print") {
      return parsePrint();
    }
  }

  // if the statement starts with something that doesn't match above
  throw SyntaxError("Expected a statement", token.line, token.column);
}

/**
 * @brief e.g. a = 3;
 */
std::unique_ptr<Statement> Parser::parseAssignment() {
  Token token =
      checkAndAdvance(TokenType::Identifier, "Expected a variable name");
  auto assignment = makeNode<AssignmentStatement>(token);
  assignment->name = token.value;

  checkAndAdvance(TokenType::Equal, "Expected = ");
  // e.g. for: a = parseExpression(3 x 2 / 3)
  assignment->value = parseExpression();
  checkSemicolonAndAdvance();

  return assignment;
}

/**
 * @brief e.g. 'print(name);' or 'print("hello");'
 */
std::unique_ptr<Statement> Parser::parsePrint() {
  Token token = checkAndAdvance(TokenType::Identifier, "Expected print");
  auto print = makeNode<PrintStatement>(token);

  checkAndAdvance(TokenType::LeftParen, "Expected (");
  print->value = parseExpression();
  checkAndAdvance(TokenType::RightParen, "Expected )");
  checkSemicolonAndAdvance();

  return print;
}

/**
 * @brief e.g. []: items;
 */
std::unique_ptr<Statement> Parser::parseArrayDecleration() {
  Token token = checkAndAdvance(TokenType::LeftBracket, "Expected [");
  auto array_decleration = makeNode<ArrayDeclerationStatement>(token);

  checkAndAdvance(TokenType::RightBracket, "Expected ]");
  checkAndAdvance(TokenType::Colon, "Expected :");

  array_decleration->name =
      checkAndAdvance(TokenType::Identifier, "Expected array name").value;
  checkSemicolonAndAdvance();

  return array_decleration;
}

/**
 * @brief e.g. 'items->append(argument);' or pop
 */
std::unique_ptr<Statement> Parser::parseMethodCall() {
  Token object = checkAndAdvance(TokenType::Identifier, "Expected object name");
  auto method_call = makeNode<MethodCallStatement>(object);
  method_call->objectName = object.value;

  checkAndAdvance(TokenType::Arrow, "Expected ->");
  Token name = checkAndAdvance(TokenType::Identifier, "Expected a method name");
  method_call->methodName = name.value;

  checkAndAdvance(TokenType::LeftParen, "Expected (");

  if (current().type != TokenType::RightParen) {
    method_call->argument = parseExpression();
  }

  checkAndAdvance(TokenType::RightParen, "Expected )");
  checkSemicolonAndAdvance();

  return method_call;
}

std::unique_ptr<Expr> Parser::parseExpression() { return parseAddition(); }

/**
 * @brief + and -
 * loops so that 1 - 2 - 3 is (1 - 2) - 3
 */
std::unique_ptr<Expr> Parser::parseAddition() {
  auto expression = parseMultiplication();

  while (current().type == TokenType::Plus ||
         current().type == TokenType::Minus) {
    Token op = current();
    advance();
    expression = makeBinary(op, std::move(expression), parseMultiplication());
  }
  return expression;
}

/**
 * @brief * and /, e.g. a * b / c
 */
std::unique_ptr<Expr> Parser::parseMultiplication() {
  auto expression = parseUnary();

  while (current().type == TokenType::Star ||
         current().type == TokenType::Slash) {
    Token op = current();
    advance();
    // unique_ptr needs to be moved
    expression = makeBinary(op, std::move(expression), parseUnary());
  }
  return expression;
}

std::unique_ptr<Expr> Parser::parseUnary() {
  if (current().type != TokenType::Minus) {
    return parseValue();
  }

  Token op = current();
  advance();

  // -5 is kept as one number
  if (current().type == TokenType::Number) {
    auto expression = makeNode<NumberExpr>(op);
    expression->value = toInt(current(), "-" + current().value);

    advance();
    return expression;
  }

  auto expression = makeNode<UnaryExpr>(op);
  expression->op = '-';
  expression->operand = parseUnary();
  return expression;
}

/**
 *@brief For a single value: a number, string, name or expression in bracket
 */
std::unique_ptr<Expr> Parser::parseValue() {
  const Token& token = current();

  if (token.type == TokenType::Number) {
    auto expression = makeNode<NumberExpr>(token);
    expression->value = toInt(token, token.value);

    advance();
    return expression;
  }

  if (token.type == TokenType::String) {
    auto expression = makeNode<StringExpr>(token);
    expression->value = token.value;

    advance();
    return expression;
  }

  if (token.type == TokenType::Identifier) {
    auto expression = makeNode<VariableExpr>(token);
    expression->name = token.value;

    advance();
    return expression;
  }

  // brackets don't need a node, it is already shown by the tree shape
  if (token.type == TokenType::LeftParen) {
    advance();
    auto expression = parseExpression();
    checkAndAdvance(TokenType::RightParen, "Expected )");

    return expression;
  }
  throw SyntaxError("Expected a number, string or name", token.line,
                    token.column);
}

int Parser::toInt(const Token& token, const std::string& text) const {
  // stoi throws std::out_of_range with message "stoi"
  try {
    return std::stoi(text);
  } catch (const std::out_of_range&) {
    throw CodeError("Number out of range" + text, token.line, token.column);
  }
}

// token helpers
const Token& Parser::current() const {
  if (position_ >= tokens_.size()) {
    return tokens_.back();
  }
  return tokens_[position_];
}

const Token& Parser::peek() const {
  if (position_ + 1 >= tokens_.size()) {
    return tokens_.back();
  }
  return tokens_[position_ + 1];
}

const Token& Parser::previous() const {
  if (position_ == 0) {
    return tokens_.front();
  }
  return tokens_[position_ - 1];
}

void Parser::advance() {
  if (position_ < tokens_.size()) {
    ++position_;
  }
}

Token Parser::checkAndAdvance(TokenType expected, const std::string& message) {
  if (current().type != expected) {
    throw SyntaxError(message, current().line, current().column);
  }

  Token token = current();
  advance();

  return token;
}

void Parser::checkSemicolonAndAdvance() {
  if (current().type != TokenType::SemiColon) {
    throw SyntaxError("Expected ; after this", previous().line,
                      previous().column);
  }
  advance();
}
