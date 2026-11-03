#pragma once

#include <string>

/**
 * @file object.hpp
 * @brief Declares the value types that variables and lists can store
 */

/**
 * @brief Base class for every value in the language
 *
 * derived classes override print(), so a value can be printed without knowing
 * its type
 */
class Object {
 public:
  // prints the value to std::cout
  virtual void print() const = 0;

  // returns a new copy of the object
  virtual Object* clone() const = 0;

  // to name the type during an error message
  virtual std::string typeName() const = 0;

  // string inherits this
  virtual bool isNumber() const { return false; }

  // string inherits this
  virtual double toDouble() const;

  // virtual so deleting through an Object* also destroys the derived object
  virtual ~Object() = default;
};

// stores an integer value, e.g. 10
class IntObject : public Object {
 private:
  int value_{};

 public:
  explicit IntObject(int value);
  int value() const;
  void print() const override;
  Object* clone() const override;
  std::string typeName() const override;
  bool isNumber() const override;
  double toDouble() const override;
};

// stores a string value, e.g. "Daniel"
class StringObject : public Object {
 private:
  std::string value_{};

 public:
  explicit StringObject(const std::string& value);
  std::string value() const;
  void print() const override;
  std::string typeName() const override;
  Object* clone() const override;
};

// stores a floating point value, e.g. 30.3
class FloatObject : public Object {
 private:
  double value_{};

 public:
  explicit FloatObject(double value);
  double value() const;
  void print() const override;
  std::string typeName() const override;
  bool isNumber() const override;
  double toDouble() const override;
  Object* clone() const override;
};
