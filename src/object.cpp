#include "object.hpp"

#include <iostream>

/**
 * @file object.cpp
 * @brief Implements printing for each value type
 */

// base object
// Object is abstract, so there is nothing to implement here

// int object
IntObject::IntObject(int v) : value(v) {}

void IntObject::print() const {
    std::cout << value;
}

// string object
StringObject::StringObject(const std::string& v) : value(v) {}

// printed with single quotes, e.g. 'Daniel', so strings stand out inside a list
void StringObject::print() const {
    std::cout << "'" << value << "'";
}

// float object
FloatObject::FloatObject(double v) : value(v) {}

void FloatObject::print() const {
    std::cout << value;
}
