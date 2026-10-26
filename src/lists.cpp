#include "lists.hpp"

#include <iostream>

/**
 * @file lists.cpp
 * @brief Implements the dynamic array used for lists in the language
 */

List::List()
    : size(0), capacity(4)
{
    data = new Object*[capacity];
}

void List::grow() {
    std::size_t newCapacity = capacity * 2;

    Object** newData = new Object*[newCapacity];

    // only the pointers are copied, not the objects
    for (std::size_t i = 0; i < size; ++i) {
        newData[i] = data[i];
    }

    delete[] data;

    data = newData;
    capacity = newCapacity;
}

void List::append(int value) {
    append(new IntObject(value));
}

void List::append(double value) {
    append(new FloatObject(value));
}

void List::append(const std::string& value) {
    append(new StringObject(value));
}

void List::append(const char* value) {
    append(new StringObject(value));
}

void List::append(Object* object) {
    if (size == capacity) {
        grow();
    }

    data[size++] = object;
}

void List::set(std::size_t index, Object* object) {
    if (index >= size) {
        return;
    }

    delete data[index];
    data[index] = object;
}

void List::set(std::size_t index, int value) {
    set(index, new IntObject(value));
}

void List::set(std::size_t index, double value) {
    set(index, new FloatObject(value));
}

void List::set(std::size_t index, const std::string& value) {
    set(index, new StringObject(value));
}

void List::set(std::size_t index, const char* value) {
    set(index, new StringObject(value));
}

void List::pop() {
    if (size == 0) {
        return;
    }

    delete data[size - 1];
    --size;
}

void List::display() {
    format();
}

void List::display_at_index(std::size_t index) {
    if (index >= size) {
        return;
    }

    data[index]->print();
}

void List::format() {
    std::cout << "[";

    for (std::size_t i = 0; i < size; ++i) {
        data[i]->print();

            if (i + 1 < size) {
            std::cout << ", ";
        }
    }

    std::cout << "]\n";
}

Object* List::get(std::size_t index) {
    if (index >= size) {
        return nullptr;
    }

    return data[index];
}

std::size_t List::len() const {
    return size;
}

List::~List() {
    for (std::size_t i = 0; i < size; ++i) {
        delete data[i];
    }

    delete[] data;
}
