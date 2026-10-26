#include "parser.hpp"
#include "ast.hpp"
#include "token.hpp"

#include <stdexcept>

/**
 * @file parser.cpp
 * @brief Implements statement parsing and syntax validation
 */

Parser::Parser(const std::vector<Token>& inputTokens) : tokens_(inputTokens) {}

const Token& Parser::current() {
    if (position_ >= tokens_.size()) {
        throw std::runtime_error("Unexpected end of input");
    }

    return tokens_[position_];
}

const Token& Parser::peek() {
    if (position_ + 1 >= tokens_.size()) {
        throw std::runtime_error("Unexpected end of input");
    }

    return tokens_[position_ + 1];
}

void Parser::advance() {
    if (position_ < tokens_.size()) {
        ++position_;
    }
}

Token Parser::consume(TokenType expected, const char* message) {
    if (current().type != expected) {
        throw std::runtime_error(message);
    }

    // save the current token
    Token token = current();
    advance();

    return token;
}

void Parser::finishStatement() {
    consume(TokenType::SemiColon, "Expect ;");

    if (current().type != TokenType::EndOfFile) {
        throw std::runtime_error("Unexpected tokens after statement");
    }
}

/**
 * @brief The parsing dealing with the assignment of variables
 * @return Struct (AssignmentStatement) holding the variable and its value
 *
 * e.g. parsing for the format of: 'a = 3;'
 */
AssignmentStatement Parser::parseAssignment() {
    AssignmentStatement statement;
    statement.name = consume(TokenType::Identifier, "Expected variable name").value;

    consume(TokenType::Equal, "Expected =");

    TokenType type = current().type;

    if (type != TokenType::Number &&
        type != TokenType::String &&
        type != TokenType::Identifier) {
        throw std::runtime_error("Expected number, string, or variable name");
    }

    statement.value = current();
    advance();
    finishStatement();
    return statement;
}

/**
 * @brief The parsing dealing with the print statement
 * @return Struct (PrintStatement) that holds the value to print
 *
 * e.g. parsing for the format of: 'print(name);'
 */
PrintStatement Parser::parsePrintStatement() {
    if (consume(TokenType::Identifier, "Expected print").value != "print") {
        throw std::runtime_error("Expected print");
    }

    consume(TokenType::LeftParen, "Expected (");

    PrintStatement statement;
    statement.variableName = consume(TokenType::Identifier, "Expected variable name").value;

    consume(TokenType::RightParen, "Expected )");

    finishStatement();
    return statement;
}

/**
 * @brief The parsing dealing with declaring arrays
 * @return Struct (ArrayDecleration) that holds the name of the array
 *
 * e.g. parsing for the format of: '[]: items;'
 */
ArrayDecleration Parser::parseArrayDecleration() {
    consume(TokenType::LeftBracket, "Expected [");
    consume(TokenType::RightBracket, "Expected ]");
    consume(TokenType::Colon, "Expected :");

    ArrayDecleration declaration;
    declaration.name = consume(TokenType::Identifier, "Expected array name").value;

    finishStatement();
    return declaration;
}

/**
 * @brief The parsing dealing with method calls
 * @return Struct (MethodCall) that holds the name of the object, method name and argument
 *
 * e.g parsing for the format of: 'object->append(argument);'
 */
MethodCall Parser::parseMethodCall() {
    MethodCall call;

    call.objectName = consume(TokenType::Identifier, "Expected object name").value;
    consume(TokenType::Arrow, "Expected ->");
    call.methodName = consume(TokenType::Identifier, "Expected method name").value;
    consume(TokenType::LeftParen, "Expected (");

    // for the optional argument
    if (current().type != TokenType::RightParen) {
        if (current().type == TokenType::EndOfFile) {
            throw std::runtime_error("Expected argument or )");
        }

        call.argument = current();
        advance();
    }

    consume(TokenType::RightParen, "Expected )");
    finishStatement();

    return call;
}
