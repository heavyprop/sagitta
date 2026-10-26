/**
 * @file environment.hpp
 * @brief Declares storage for named variables and lists
 */
#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include "lists.hpp"
#include "object.hpp"

/**
 * @brief Maintains variable bindings and owns named lists.
 *
 * Variable objects use shared ownership. Lists are owned by the environment
 * and deleted when the environment is destroyed.
 *
 */
class Environment {
private:
    std::unordered_map<std::string, std::shared_ptr<Object>> variables;
    std::unordered_map<std::string, List*> arrays;

    void removeArray(const std::string& name);

public:
    Environment() = default;

    /// copying is disabled since would delete the same list twice
    Environment(const Environment&) = delete;
    Environment& operator=(const Environment&) = delete;

    /// creates or replaces a binding with shared ownership of the object
    void setVariable(const std::string& name, std::shared_ptr<Object> value);

    // throwes an std::runtime_error if the variable does not exist.
    std::shared_ptr<Object> getVariable(const std::string& name) const;

    bool variableExists(const std::string& name) const;

    /// Creates an empty list; throws std::runtime_error if the name already exists
    void createArray(const std::string& name);

    /// returns a borrowed pointer, or nullptr if absent.
    List* getArray(const std::string& name);

    /// makes target a copy of source, creating target if it doesn't exist
    void copyArray(const std::string& target, const std::string& source);

    /// returns the list, or nullptr if it does not exist
    bool arrayExists(const std::string& name);

    ~Environment();
};
