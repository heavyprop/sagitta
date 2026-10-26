#include "lists.hpp"

#include <iostream>
#include <utility>

/**
 * @file lists.cpp
 * @brief Implements the dynamic array used for lists in the language
 */

List::List() : size_(0), capacity_(4)
{
    data_ = new Object*[capacity_];
}

// new array, and a clone of every object in it.
// copying just the pointers would leave two lists deleting the same objects
List::List(const List& other)
    : data_(new Object*[other.capacity_]),
      size_(other.size_),
      capacity_(other.capacity_) {
    for (std::size_t i = 0; i < size_; ++i) {
        data_[i] = other.data_[i]->clone();
    }
}

// say we have a = b
// copy b into temp, then swap so a gets the copy and temp gets a's old data.
// temp is destroyed at the end of the function, which deletes the old data.
// the copy happens before anything is deleted, so if it fails a is left as it was
List& List::operator=(const List& other) {
    // a = a, nothing to do
    if (this == &other) {
        return *this;
    }

    List temp(other);

    std::swap(data_, temp.data_);
    std::swap(size_, temp.size_);
    std::swap(capacity_, temp.capacity_);

    return *this;
}

void List::grow() {
    std::size_t newCapacity = capacity_ * 2;

    Object** newData = new Object*[newCapacity];

    // only the pointers are copied, not the objects
    for (std::size_t i = 0; i < size_; ++i) {
        newData[i] = data_[i];
    }

    delete[] data_;

    data_ = newData;
    capacity_ = newCapacity;
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
    if (size_ == capacity_) {
        grow();
    }

    data_[size_++] = object;
}

void List::set(std::size_t index, Object* object) {
    if (index >= size_) {
        return;
    }

    delete data_[index];
    data_[index] = object;
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
    if (size_ == 0) {
        return;
    }

    delete data_[size_ - 1];
    --size_;
}

void List::display() {
    format();
}

void List::display_at_index(std::size_t index) {
    if (index >= size_) {
        return;
    }

    data_[index]->print();
}

void List::format() {
    std::cout << "[";

    for (std::size_t i = 0; i < size_; ++i) {
        data_[i]->print();

        if (i + 1 < size_) {
            std::cout << ", ";
        }
    }

    std::cout << "]\n";
}

Object* List::get(std::size_t index) {
    if (index >= size_) {
        return nullptr;
    }

    return data_[index];
}

std::size_t List::len() const {
    return size_;
}

List::~List() {
    for (std::size_t i = 0; i < size_; ++i) {
        delete data_[i];
    }

    delete[] data_;
}
