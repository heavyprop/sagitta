#pragma once

#include <cstddef>
#include <stdexcept>
#include <string>

/**
 * @file error.hpp
 * @brief Declares the error thrown for mistakes
 */

/**
 * @brief An error that knows where in the source is happened
 *
 * thrown by the lexer and parser when the code is written wrong
 * and by the interpreter when a statement fails while running
 */
class CodeError : public std::runtime_error {
 private:
  std::size_t line_;
  std::size_t column_;

 public:
  CodeError(const std::string& message, std::size_t line, std::size_t column)
      : std::runtime_error(message), line_(line), column_(column) {}

  std::size_t line() const { return line_; }
  std::size_t column() const { return column_; }
};

class SyntaxError : public CodeError {
 public:
  using CodeError::CodeError;  // re-using the constructor
};

class RuntimeError : public CodeError {
 public:
  using CodeError::CodeError;
};
