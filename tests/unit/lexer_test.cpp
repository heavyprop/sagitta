#include "lexer.hpp"

#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "error.hpp"

static std::vector<Token> tokenize(const std::string& source) {
  Lexer lexer(source);
  std::vector<Token> tokens;

  while (true) {
    tokens.push_back(lexer.nextToken());

    if (tokens.back().type == TokenType::EndOfFile) {
      return tokens;
    }
  }
}

static std::vector<TokenType> types(const std::string& source) {
  std::vector<TokenType> result;

  for (const Token& token : tokenize(source)) {
    result.push_back(token.type);
  }
  return result;
}

TEST(LexerTest, EmptySourceIsJustEndOfFile) {
  EXPECT_EQ(types(""), std::vector<TokenType>{TokenType::EndOfFile});
}

TEST(LexerTest, WhiteSpaceOnlyIsJustEndOfFile) {
  EXPECT_EQ(types("     \n\t   "),
            std::vector<TokenType>{TokenType::EndOfFile});
}

TEST(LexerTest, KeepsReturningEndOfFile) {
  Lexer lexer("a");
  lexer.nextToken();

  EXPECT_EQ(lexer.nextToken().type, TokenType::EndOfFile);

  // if we do it again
  EXPECT_EQ(lexer.nextToken().type, TokenType::EndOfFile);
}

TEST(LexerTest, ReadsEverySymbol) {
  std::vector<TokenType> expected = {
      TokenType::LeftParen,  TokenType::RightParen,  TokenType::LeftBrace,
      TokenType::RightBrace, TokenType::LeftBracket, TokenType::RightBracket,
      TokenType::Colon,      TokenType::SemiColon,   TokenType::Comma,
      TokenType::Equal,      TokenType::Plus,        TokenType::Minus,
      TokenType::Star,       TokenType::Slash,       TokenType::EndOfFile,
  };

  EXPECT_EQ(types("( ) { } [ ] : ; , = + - * /"), expected);
}

TEST(LexerTest, ReadsAssignment) {
  auto tokens = tokenize("a = 1;");

  ASSERT_EQ(tokens.size(), 5u);
  EXPECT_EQ(tokens[0].type, TokenType::Identifier);
  EXPECT_EQ(tokens[0].value, "a");
  EXPECT_EQ(tokens[1].type, TokenType::Equal);
  EXPECT_EQ(tokens[2].type, TokenType::Number);
  EXPECT_EQ(tokens[2].value, "1");
  EXPECT_EQ(tokens[3].type, TokenType::SemiColon);
  EXPECT_EQ(tokens[4].type, TokenType::EndOfFile);
}

// Numbers
TEST(LexerTest, ReadsNumber) {
  auto tokens = tokenize("42");

  EXPECT_EQ(tokens[0].type, TokenType::Number);
  EXPECT_EQ(tokens[0].value, "42");
}

TEST(LexerTest, MinusIsSeperateFromNumber) {
  auto tokens = tokenize("-5");

  ASSERT_EQ(tokens.size(), 3u);
  EXPECT_EQ(tokens[0].type, TokenType::Minus);
  EXPECT_EQ(tokens[1].type, TokenType::Number);
  EXPECT_EQ(tokens[1].value, "5");
}
