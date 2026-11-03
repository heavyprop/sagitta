#include "object.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

/**
 * @file object.cpp
 */

// base object
// derived string inherits this
double Object::toDouble() const {
  throw std::runtime_error("Value is not a number: " + typeName());
}

// int object
IntObject::IntObject(int value) : value_(value) {}

int IntObject::value() const { return value_; }
void IntObject::print() const { std::cout << value_; }

/**
 * @brief When we want to clone the int
 * return a new base class pointer to a new object
 */
Object* IntObject::clone() const { return new IntObject(value_); }

std::string IntObject::typeName() const { return "int"; }

bool IntObject::isNumber() const { return true; }

double IntObject::toDouble() const {
  // set return type to double, so will convert
  return value_;
}

// string object
StringObject::StringObject(const std::string& value) : value_(value) {}

// printed with single quotes, e.g. 'Daniel', so strings stand out inside a list
std::string StringObject::value() const { return value_; }

void StringObject::print() const { std::cout << "'" << value_ << "'"; }

Object* StringObject::clone() const { return new StringObject(value_); }

std::string StringObject::typeName() const { return "string"; }

// float object
FloatObject::FloatObject(double value) : value_(value) {}

double FloatObject::value() const { return value_; }

void FloatObject::print() const {
  std::ostringstream text;
  // put the value into the string stream
  text << value_;

  std::string result = text.str();

  // if doesn't have decimal point, or scientific notation, or Nan
  // then add the .0
  if (result.find_first_of(".en") == std::string::npos) {
    result += ".0";
  }
  std::cout << result;
}

Object* FloatObject::clone() const { return new FloatObject(value_); }

std::string FloatObject::typeName() const { return "float"; }

bool FloatObject::isNumber() const { return true; }

double FloatObject::toDouble() const { return value_; }
