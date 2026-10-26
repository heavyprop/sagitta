/**
 * @file parser.hpp
 * @brief Declares the parser that converts tokens into statement structures
 *
 * checks statement syntax and reports unexpected / missing tokens
 *
 * "borrows" the vector of tokens, since we don't actually modify and this is a read only.
 *  vector must remain alive and unchanged while the parser uses it
 */
#pragma once

#include <cstddef>
#include <vector>

#include "ast.hpp"
#include "token.hpp"

class Parser {
private:
    const std::vector<Token>& tokens_;
    std::size_t position_ = 0;

    // returns the current token
    // @throws std::runtime_error If no current token is available.
    const Token& current();

    // returns the next token
    // @throws std::runtime_error If no next token exists
    const Token& peek();

    // moves to next position if it has not reached the end of the vector
    void advance();

    /**
     * @brief Checks and consumes a token of the expected type
     *
     * @param expected required token
     * @param message Error message used if the token does not match
     * @return A copy of the consumed token
     * @throws std::runtime_error If the expected token is unavailable or has unexpected type
     */
    Token consume(TokenType expected, const char* message);

    /**
     * @brief Handles the semi colon and checks for left over tokens
     *
     * @param semiColonRequired: whether a statement must end with a semi colon
     * @throws std::runtime_error If a required semi colon is missing
     * or unexpected token tokens remain, or input ends unexpectedly.
     */
    void finishStatement();

public:
    /**
     * @brief Creates a parser that borrows the supplied token vector
     * @param inputTokens Lexer output, including the final EndOfFile token
     */
    Parser(const std::vector<Token>& inputTokens);

    // Parse an assignment such as a = 10 or a = b
    // @throws std::runtime_error if the statment syntax is invalid
    AssignmentStatement parseAssignment();

    // parses a print statement such as print(a)
    // @throws std::runtime_error if the statment syntax is invalid
    PrintStatement parsePrintStatement();

    // parses a list decleration such as []: items;
    // @throws std::runtime_error if the statment syntax is invalid
    ArrayDecleration parseArrayDecleration();

    // pases a method call such as items->append(10)
    // items->pop();
    // @throws std::runtime_error if the statment syntax is invalid
    MethodCall parseMethodCall();
};
