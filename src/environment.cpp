/**
 * @file environment.cpp
 * @brief Implements variable bindings and named list storage.
 */

#include "environment.hpp"

#include <stdexcept>
#include <utility>

// Variables

void Environment::setVariable(const std::string& name,std::shared_ptr<Object> value) {
    variables[name] = std::move(value);
}

std::shared_ptr<Object> Environment::getVariable(const std::string& name) const {
    auto it = variables.find(name);

    if (it == variables.end()) {
        throw std::runtime_error("Variable does not exist: " + name);
    }

    return it->second;
}

bool Environment::variableExists(const std::string& name) const {
    return variables.find(name) != variables.end();
}

// Lists

void Environment::createArray(const std::string& name) {
    if (arrayExists(name)) {
        throw std::runtime_error("Array already exists: " + name);
    }

    if (variableExists(name)) {
        throw std::runtime_error("Variable with this name already exists: " + name);
    }

    // unordered map storing name of array with the a pointer to list
    arrays[name] = new List();
}

List* Environment::getArray(const std::string& name) {
    auto it = arrays.find(name);

    if (it == arrays.end()) {
        return nullptr;
    }

    return it->second;
}

bool Environment::arrayExists(const std::string& name) {
    return arrays.find(name) != arrays.end();
}

Environment::~Environment() {
    // lists are stored as raw pointers, so they have to be deleted here
    for (auto& item : arrays) {
        delete item.second;
    }
}
