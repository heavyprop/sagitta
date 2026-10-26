/**
 * @file environment.cpp
 * @brief Implements variable bindings and named list storage.
 */

#include "environment.hpp"

#include <stdexcept>
#include <utility>

// Variables

void Environment::setVariable(const std::string& name, std::shared_ptr<Object> value) {
    removeArray(name);

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
    // declaring a name again replaces whatever it held - like python
    variables.erase(name);
    removeArray(name);

    arrays[name] = new List();
}

void Environment::copyArray(const std::string& target, const std::string& source) {
    List* from = getArray(source);

    if (from == nullptr) {
        throw std::runtime_error("Array does not exist: " + source);
    }

    // if target was a variable it becomes a list
    variables.erase(target);

    List* to = getArray(target);

    // target is new, so build it with the copy constructor
    if (to == nullptr) {
        arrays[target] = new List(*from);
    }

    else {
        // that is the operator we defined for list (that uses temp etc)
        *to = *from;
    }
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

void Environment::removeArray(const std::string& name) {
    auto it = arrays.find(name);

    if (it == arrays.end()) {
        return;
    }

    delete it->second;
    arrays.erase(it);
}

Environment::~Environment() {
    // lists are stored as raw pointers, so they have to be deleted here
    for (auto& item : arrays) {
        delete item.second;
    }
}
