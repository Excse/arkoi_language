#include "gtest/gtest.h"

#include "scanner.h"

TEST(Scanner, Valid) {
    Scanner scanner("hello");

    auto tokens = scanner.tokenize();
    ASSERT_FALSE(scanner.has_failed());
    ASSERT_EQ(tokens.size(), 2);

    ASSERT_EQ(tokens[0].type(), Token::Type::Identifier);
    ASSERT_EQ(tokens[0].column(), 0);
    ASSERT_EQ(tokens[0].row(), 0);
    ASSERT_EQ(tokens[0].contents(), "hello");

    ASSERT_EQ(tokens[1].type(), Token::Type::EndOfFile);
}