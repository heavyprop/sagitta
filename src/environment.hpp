/**
 * @file environment.hpp
 * @brief Declares storage for named variables
 */

#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "object.hpp"

/**
 * @brief Maps each value to its name
 *
 * every value is an Object
 * values use shared ownership and are deleted once nothing uses them
 */
class Environment {
 private:
  std::unordered_map<std::string, std::shared_ptr<Object>> variables;

 public:
  void setVariable(const std::string& name, std::shared_ptr<Object> value);
  std::shared_ptr<Object> getVariable(const std::string& name) const;
};
