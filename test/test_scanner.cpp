#include "gtest/gtest.h"

#include "frontend/scanner.hpp"

TEST(Scanner, Comment) {
    Scanner scanner("# This is a test");

    auto tokens = scanner.tokenize();
    ASSERT_FALSE(scanner.has_failed());
    ASSERT_EQ(tokens.size(), 2);

    ASSERT_EQ(tokens[0].type(), Token::Type::Comment);
    ASSERT_EQ(tokens[0].column(), 0);
    ASSERT_EQ(tokens[0].row(), 0);
    ASSERT_EQ(tokens[0].contents(), "# This is a test");

    ASSERT_EQ(tokens[1].type(), Token::Type::EndOfFile);
}