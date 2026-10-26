#pragma once

#include <string>
#include <cstddef>

#include "object.hpp"

/**
 * @file lists.hpp
 * @brief Declares the dynamic array used for lists in the language
 */

/**
 * @brief A dynamically allocated array that can store different types
 *
 * the list owns every object it stores and deletes them when they are removed.
 */
class List {
private:
    Object** data_;
    std::size_t size_;
    std::size_t capacity_;

    void grow();

public:
    List();

    // copies are deep, each list gets its own objects so nothing is shared
    List(const List& other);
    List& operator=(const List& other);

    // parameter overloading, depending on type
    void append(int value);
    void append(double value);
    void append(const std::string& value);
    void append(const char* value);
    // the list takes ownership of the object
    void append(Object* object);

    void set(std::size_t index, Object* object);
    void set(std::size_t index, int value);
    void set(std::size_t index, double value);
    void set(std::size_t index, const std::string& value);
    void set(std::size_t index, const char* value);

    void pop();

    void display();
    void display_at_index(std::size_t index);
    void format();

    // returns a borrowed pointer, or nullptr if the index is out of range
    Object* get(std::size_t index);

    std::size_t len() const;

    ~List();
};
