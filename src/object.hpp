#pragma once

#include <string>

/**
 * @file object.hpp
 * @brief Declares the value types that variables and lists can store
 */

/**
 * @brief Base class for every value in the language
 *
 * derived classes override print(), so a value can be printed without knowing its type
 */
class Object {
public:
    // prints the value to std::cout
    virtual void print() const = 0;
    // virtual so deleting through an Object* also destroys the derived object
    virtual ~Object() = default;
};

// stores an integer value, e.g. 10
class IntObject : public Object {
public:
    int value;

    IntObject(int v);

    void print() const override;
};

// stores a string value, e.g. "Daniel"
class StringObject : public Object {
public:
    std::string value;

    StringObject(const std::string& v);

    void print() const override;
};

// stores a floating point value, e.g. 30.3
class FloatObject : public Object {
public:
    double value;

    FloatObject(double v);

    void print() const override;
};
