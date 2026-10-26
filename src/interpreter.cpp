#include "interpreter.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "ast.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "object.hpp"
#include "token.hpp"

/**
 * @file interpreter.cpp
 * @brief Implements running a single statement
 *
 * tokenizes the input, works out the type of statement from the first tokens,
 * parses it, then updates or reads from the environment.
 */

/**
 * @brief Converts the text of a Number token into an int
 *
 * std::stoi throws std::out_of_range with the message "stoi",
 * so it is caught here and replaced with a clearer error
 *
 * @param text The digits of the number, e.g. "10" or "-5"
 * @return The number as an int
 * @throws std::runtime_error If the number does not fit in an int
 */
static int toInt(const std::string& text) {
    try {
        return std::stoi(text);
    }
    catch (const std::out_of_range&) {
        throw std::runtime_error("Number out of range: " + text);
    }
    catch (const std::invalid_argument&) {
        throw std::runtime_error("Invalid number: " + text);
    }
}

/**
 * @brief The statement types are checked in this order:
 *
 * assignment:    'a = 10;'
 * print:         'print(a);'
 * array:         '[]: items;'
 * method call:   'items->append(10);'
 */
void Interpreter::run(const std::string& input) {
    Lexer lexer(input);
    std::vector<Token> tokens;

    // read every token in the input, the EndOfFile token is kept
    // so the parser knows where the statement ends
    while (true) {
        Token token = lexer.nextToken();
        tokens.push_back(token);

        if (token.type == TokenType::EndOfFile) {
            break;
        }
    }

    // nothing to run, e.g. a line with only white space
    if (tokens[0].type == TokenType::EndOfFile) {
        return;
    }

    Parser parser(tokens);

    // first assignment and reassignment use the same path
    if (tokens.size() > 1 &&
        tokens[0].type == TokenType::Identifier &&
        tokens[1].type == TokenType::Equal) {
        AssignmentStatement statement = parser.parseAssignment();
        std::shared_ptr<Object> value;

        // create the value object depending on the type of token
        switch (statement.value.type) {
            case TokenType::Number:
                value = std::make_shared<IntObject>(
                    toInt(statement.value.value)
                );
                break;

            case TokenType::String:
                value = std::make_shared<StringObject>(statement.value.value);
                break;

            // copies the shared pointer, so both variables point to the same object
            case TokenType::Identifier:
                value = environment.getVariable(statement.value.value);
                break;

            default:
                throw std::runtime_error("Invalid assignment value");
        }

        // creates the variable, or replaces it if it already exists
        environment.setVariable(statement.name, value);
    }

    // print a variable or list
    else if (tokens[0].type == TokenType::Identifier &&
             tokens[0].value == "print") {
        PrintStatement statement = parser.parsePrintStatement();

        if (environment.variableExists(statement.variableName)) {
            auto value = environment.getVariable(statement.variableName);

            // strings are printed without quotes here,
            // StringObject::print() adds quotes for when it is inside a list
            if (auto text = std::dynamic_pointer_cast<StringObject>(value)) {
                std::cout << text->value;
            }
            else {
                value->print();
            }

            std::cout << '\n';

            return;
        }

        // not a variable, so check if it is a list
        List* array = environment.getArray(statement.variableName);

        if (array == nullptr) {
            throw std::runtime_error("Name does not exist: " + statement.variableName);
        }

        array->display();
    }

    // declare a new list, e.g. '[]: items;'
    else if (tokens[0].type == TokenType::LeftBracket) {
        ArrayDecleration declaration = parser.parseArrayDecleration();
        environment.createArray(declaration.name);
    }

    // call a method on a list, e.g. 'items->append(10);' or 'items->pop();'
    else if (tokens[0].type == TokenType::Identifier) {
        MethodCall call = parser.parseMethodCall();
        List* array = environment.getArray(call.objectName);

        if (array == nullptr) {
            throw std::runtime_error("Array does not exist: " + call.objectName);
        }

        if (call.methodName == "append") {
            if (!call.argument) {
                throw std::runtime_error("->append expects one argument");
            }

            // string literal, e.g. items->append("Daniel");
            if (call.argument->type == TokenType::String) {
                array->append(call.argument->value);
            }

            // number literal, e.g. items->append(10);
            else if (call.argument->type == TokenType::Number) {
                array->append(toInt(call.argument->value));
            }

            // variable, e.g. items->append(name);
            // the list stores its own copy of the value, not the variable itself
            else if (call.argument->type == TokenType::Identifier) {
                auto value = environment.getVariable(call.argument->value);

                // check the actual type of the object, nullptr is returned if the cast fails
                if (auto integer = std::dynamic_pointer_cast<IntObject>(value)) {
                    array->append(integer->value);
                }

                else if (auto text = std::dynamic_pointer_cast<StringObject>(value)) {
                    array->append(text->value);
                }

                else if (auto floating = std::dynamic_pointer_cast<FloatObject>(value)) {
                    array->append(floating->value);
                }
                else {
                    throw std::runtime_error("Unsupported variable type for append");
                }
            }

            else {
                throw std::runtime_error("Invalid append value");
            }
        }

        // removes the last item in the list
        else if (call.methodName == "pop") {
            if (call.argument) {
                throw std::runtime_error("->pop expects no arguments");
            }

            array->pop();
        }

        else {
            throw std::runtime_error("Unknown method: " + call.methodName);
        }
    }

    // no statement rule matched
    else {
        throw std::runtime_error("Unknown statement");
    }
}
