#include <fstream>
#include <iostream>
#include <sstream>

#include "symbol_table.h"
#include "scanner.h"
#include "parser.h"

int main() {
    std::ifstream file("../example/test.ark");
    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string source = buffer.str();
    Scanner scanner(source);

    std::vector<Token> tokens = scanner.tokenize();

    auto is_useless = [](const Token &token) { return token.type() == Token::Type::Comment; };
    tokens.erase(std::remove_if(tokens.begin(), tokens.end(), is_useless), tokens.end());

    Parser parser(tokens);

    Program program = parser.parse_program();

    return 0;
}
