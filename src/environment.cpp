/**
 * @file environment.cpp
 * @brief Implements variables
 */

#include "environment.hpp"

#include <utility>

void Environment::setVariable(const std::string& name,
                              std::shared_ptr<Object> value) {
  variables[name] = std::move(value);
}

std::shared_ptr<Object> Environment::getVariable(
    const std::string& name) const {
  auto it = variables.find(name);

  if (it == variables.end()) {
    return nullptr;
  }

  return it->second;
}
