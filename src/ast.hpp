#pragma once

#include <algorithm>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "object.hpp"

/**
 * @file ast.hpp
 * @brief Defines the nodes of the syntax tree that the parser builds
 *
 * expressions are things that have a value e.g. 10,
 * statements are something that do something
 *
 * the interpreter walks the tree using the visitor pattern
 * each node has accept(), which calls the matching visit method on the visitor
 */

struct NumberExpr;
struct StringExpr;
struct VariableExpr;
struct UnaryExpr;
struct BinaryExpr;

struct AssignmentStatement;
struct PrintStatement;
struct ArrayDeclerationStatement;
struct MethodCallStatement;

// one visit method for each kind of expression, each returns the value of it
// used so that the logic can be left for the interpreter
// a node's accept() calls its matching visit function
class ExpressionVisitor {
 public:
  virtual ~ExpressionVisitor() = default;

  virtual std::shared_ptr<Object> visitNumber(const NumberExpr& expr) = 0;
  virtual std::shared_ptr<Object> visitString(const StringExpr& expr) = 0;
  virtual std::shared_ptr<Object> visitVariable(const VariableExpr& expr) = 0;
  virtual std::shared_ptr<Object> visitUnary(const UnaryExpr& expr) = 0;
  virtual std::shared_ptr<Object> visitBinary(const BinaryExpr& expr) = 0;
};

class StatementVisitor {
 public:
  virtual ~StatementVisitor() = default;

  virtual void visitAssign(const AssignmentStatement& statement) = 0;
  virtual void visitPrint(const PrintStatement& statement) = 0;
  virtual void visitArrayDecleration(
      const ArrayDeclerationStatement& statement) = 0;
  virtual void visitMethodCall(const MethodCallStatement& statement) = 0;
};

// Expressions

/**
 * @brief Base for every expression
 *
 * stores where the expression starts, so errors can point at it
 */
struct Expr {
  std::size_t line = 1;
  std::size_t column = 1;

  virtual ~Expr() = default;

  virtual std::shared_ptr<Object> accept(ExpressionVisitor& visitor) const = 0;
};

// a whole number
struct NumberExpr : Expr {
  int value = 0;

  std::shared_ptr<Object> accept(ExpressionVisitor& visitor) const override {
    return visitor.visitNumber(*this);
  }
};

// text in double quotes
struct StringExpr : Expr {
  std::string value;

  std::shared_ptr<Object> accept(ExpressionVisitor& visitor) const override {
    return visitor.visitString(*this);
  }
};

// the name of the variable or list
struct VariableExpr : Expr {
  std::string name;

  std::shared_ptr<Object> accept(ExpressionVisitor& visitor) const override {
    return visitor.visitVariable(*this);
  }
};

struct UnaryExpr : Expr {
  char op = '-';
  // e.g. operand can be int(5) or (5 + 3)
  std::unique_ptr<Expr> operand;

  std::shared_ptr<Object> accept(ExpressionVisitor& visitor) const override {
    return visitor.visitUnary(*this);
  }
};

// operator between 2 values
// for 1 + 2 * 3 the right itself is a binary expr for 2 * 3
struct BinaryExpr : Expr {
  char op = '+';
  std::unique_ptr<Expr> left;
  std::unique_ptr<Expr> right;

  std::shared_ptr<Object> accept(ExpressionVisitor& visitor) const override {
    return visitor.visitBinary(*this);
  }
};

// statements

/**
 * @brief Base for every statement
 *
 * stores where the statement starts, so errors can point to it
 */
struct Statement {
  std::size_t line = 1;
  std::size_t column = 1;

  virtual ~Statement() = default;

  virtual void accept(StatementVisitor& visitor) const = 0;
};

struct AssignmentStatement : Statement {
  std::string name;
  std::unique_ptr<Expr> value;

  void accept(StatementVisitor& visitor) const override {
    visitor.visitAssign(*this);
  }
};

struct PrintStatement : Statement {
  std::unique_ptr<Expr> value;

  void accept(StatementVisitor& visitor) const override {
    visitor.visitPrint(*this);
  }
};

struct ArrayDeclerationStatement : Statement {
  std::string name;

  void accept(StatementVisitor& visitor) const override {
    visitor.visitArrayDecleration(*this);
  }
};

struct MethodCallStatement : Statement {
  std::string objectName;
  std::string methodName;
  std::unique_ptr<Expr> argument;

  void accept(StatementVisitor& visitor) const override {
    visitor.visitMethodCall(*this);
  }
};
