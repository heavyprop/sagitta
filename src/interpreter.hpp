#pragma once

#include <memory>
#include <string>

#include "ast.hpp"
#include "environment.hpp"

/**
 * @file interpreter.hpp
 * @brief Declares the interpreter that runs a syntax tree
 */

/**
 * @brief Runs source text by passing it through the lexer, parser then walking
 * the tree
 *
 * owns the environment, so variables and lists stay alive between calls to
 * run(), which is what lets the REPL remember things from earlier lines.
 *
 * it is a visitor meaning each statement and expression calls back into the
 * matching visit method
 */

class Interpreter : private ExpressionVisitor, private StatementVisitor {
 private:
  // stores every variable and list created by the program
  Environment environment;

  // works out the value of the expression
  std::shared_ptr<Object> evaluate(const Expr& expression);

  std::shared_ptr<Object> visitNumber(const NumberExpr& expr) override;
  std::shared_ptr<Object> visitString(const StringExpr& expr) override;
  std::shared_ptr<Object> visitVariable(const VariableExpr& expr) override;
  std::shared_ptr<Object> visitUnary(const UnaryExpr& expr) override;
  std::shared_ptr<Object> visitBinary(const BinaryExpr& expr) override;

  void visitAssign(const AssignmentStatement& statement) override;
  void visitPrint(const PrintStatement& statement) override;
  void visitArrayDecleration(
      const ArrayDeclerationStatement& statement) override;
  void visitMethodCall(const MethodCallStatement& statement) override;

 public:
  /**
   * @brief Tokenizes, parses and runs source code
   *
   * the whole source is pased before anything runs, so a syntax error means it
   * won't run
   *
   * @param source
   * @throws CodeError with the line and column of the first error
   */
  void run(const std::string& source);
};
