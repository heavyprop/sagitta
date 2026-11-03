#include "interpreter.hpp"

#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <vector>

#include "ast.hpp"
#include "error.hpp"
#include "lexer.hpp"
#include "lists.hpp"
#include "object.hpp"
#include "parser.hpp"
#include "token.hpp"

/**
 * @file interpreter.cpp
 * @brief Implements running the syntax tree
 */

void Interpreter::run(const std::string& source) {
  Lexer lexer(source);
  std::vector<Token> tokens;

  // read every token in the source, EndOfFile token is kept
  // parser will use that as end
  while (true) {
    Token token = lexer.nextToken();
    tokens.push_back(token);

    if (token.type == TokenType::EndOfFile) {
      break;
    }
  }

  Parser parser(tokens);
  std::vector<std::unique_ptr<Statement>> root = parser.parseBlock();

  for (const auto& statement : root) {
    try {
      statement->accept(*this);
    }
    // for catching our own error type
    catch (const CodeError&) {
      throw;
    }
    // for catching the errors without positions: from environment, or std
    // library
    catch (const std::exception& error) {
      throw RuntimeError(error.what(), statement->line, statement->column);
    }
  }
}

std::shared_ptr<Object> Interpreter::evaluate(const Expr& expression) {
  return expression.accept(*this);
}

// expressions

std::shared_ptr<Object> Interpreter::visitNumber(const NumberExpr& expr) {
  return std::make_shared<IntObject>(expr.value);
}

std::shared_ptr<Object> Interpreter::visitString(const StringExpr& expr) {
  return std::make_shared<StringObject>(expr.value);
}

std::shared_ptr<Object> Interpreter::visitVariable(const VariableExpr& expr) {
  if (auto value = environment.getVariable(expr.name)) {
    return value;
  } else {
    throw RuntimeError("Name does not exist: " + expr.name, expr.line,
                       expr.column);
  }
}

// the maths is done in a long long which is big enough for 2 ints
// ints are asymmetrics so this check needs to be done, floats are symmetric
// expr passed for errors
static std::shared_ptr<Object> makeCheckedIntObject(long long result,
                                                    const Expr& expr) {
  if (result < std::numeric_limits<int>::min() ||
      result > std::numeric_limits<int>::max()) {
    throw RuntimeError("Integer overflow, the result doesn't fit into an int",
                       expr.line, expr.column);
  }
  return std::make_shared<IntObject>(static_cast<int>(result));
}

// expr has:
// op (-)
// operand (5)
std::shared_ptr<Object> Interpreter::visitUnary(const UnaryExpr& expr) {
  // evaluate the operand first, e.g. -(-1), evaluate the (-1) first
  // the operand can be another unary expression, so it recurses
  auto value = evaluate(*expr.operand);

  // then apply the operator to the result -(result)
  switch (expr.op) {
    case '-': {
      if (auto integer = std::dynamic_pointer_cast<IntObject>(value)) {
        return makeCheckedIntObject(-static_cast<long long>(integer->value()),
                                    expr);
      }

      if (auto floating = std::dynamic_pointer_cast<FloatObject>(value)) {
        return std::make_shared<FloatObject>(-floating->value());
      }

      throw RuntimeError("Can't use - on a " + value->typeName(), expr.line,
                         expr.column);
    }
    // the parser only creates '-', so reaching here is a bug in the interpreter
    default:
      throw std::logic_error(std::string("Internal error: unknown operator ") +
                             expr.op);
  }
}

std::shared_ptr<Object> Interpreter::visitBinary(const BinaryExpr& expr) {
  auto left = evaluate(*expr.left);
  auto right = evaluate(*expr.right);

  // "a" + "b" joins the 2 strings
  auto leftText = dynamic_cast<const StringObject*>(left.get());
  auto rightText = dynamic_cast<const StringObject*>(right.get());

  if (expr.op == '+' && leftText && rightText) {
    return std::make_shared<StringObject>(leftText->value() +
                                          rightText->value());
  }

  if (!left->isNumber() || !right->isNumber()) {
    throw RuntimeError(std::string("Can't use ") + expr.op + " on " +
                           left->typeName() + " and " + right->typeName(),
                       expr.line, expr.column);
  }

  if (expr.op == '/' && right->toDouble() == 0) {
    throw RuntimeError("Division by zero", expr.line, expr.column);
  }

  auto leftInt = dynamic_cast<const IntObject*>(left.get());
  auto rightInt = dynamic_cast<const IntObject*>(right.get());

  // if two ints give an int, except / which always gives a float
  if (leftInt && rightInt && expr.op != '/') {
    long long a = leftInt->value();
    long long b = rightInt->value();

    switch (expr.op) {
      case '+':
        return makeCheckedIntObject(a + b, expr);
      case '-':
        return makeCheckedIntObject(a - b, expr);
      default:
        return makeCheckedIntObject(a * b, expr);
    }
  }

  // working in floats for division
  // or if one is already a float
  double a = left->toDouble();
  double b = right->toDouble();

  switch (expr.op) {
    case '+':
      return std::make_shared<FloatObject>(a + b);
    case '-':
      return std::make_shared<FloatObject>(a - b);
    case '*':
      return std::make_shared<FloatObject>(a * b);
    case '/':
      return std::make_shared<FloatObject>(a / b);
    default:
      throw std::logic_error(std::string("Internal error: unknown operator ") +
                             expr.op);
  }
}

void Interpreter::visitAssign(const AssignmentStatement& statement) {
  auto value = evaluate(*statement.value);
  environment.setVariable(statement.name,
                          std::shared_ptr<Object>(value->clone()));
}

void Interpreter::visitPrint(const PrintStatement& statement) {
  auto value = evaluate(*statement.value);
  value->print();
  std::cout << '\n';
}

void Interpreter::visitArrayDecleration(
    const ArrayDeclerationStatement& statement) {
  environment.setVariable(statement.name, std::make_shared<ListObject>());
}

void Interpreter::visitMethodCall(const MethodCallStatement& statement) {
  auto value = environment.getVariable(statement.objectName);
  if (value == nullptr) {
    throw RuntimeError("Name does not exist: " + statement.objectName,
                       statement.line, statement.column);
  }

  // methods only exists for lists
  if (auto listObject = std::dynamic_pointer_cast<ListObject>(value)) {
    List& list = listObject->list();

    if (statement.methodName == "append") {
      if (!statement.argument) {
        throw RuntimeError("->append expects one argument", statement.line,
                           statement.column);
      }
      // lists owns the items, so we need to clone the value
      list.append(evaluate(*statement.argument)->clone());
    }

    else if (statement.methodName == "pop") {
      if (statement.argument) {
        throw RuntimeError("->pop expects no arguments", statement.line,
                           statement.column);
      }
      list.pop();
    }

    else {
      throw RuntimeError("Unknown method: " + statement.methodName,
                         statement.line, statement.column);
    }
  } else {
    throw RuntimeError(
        statement.objectName + " is a " + value->typeName() + ", not a list",
        statement.line, statement.column);
  }
}
