#include <iostream>

#include <map>

#include "ccpp.parser.hpp"

int main()
{
    {
        ccpp::input_stream is("if (a == 2) { return 3; } else { return 4; }");
        ccpp::token_stream ts(is);
        while (!ts.eof())
        {
            auto tok = ts.next();
            std::cout << tok->type << " : " << *tok << std::endl;
        }
    }
    ccpp::input_stream is("a = 2;");
    ccpp::token_stream ts(is);
    ccpp::parser p(ts);

    auto ast = p.parse(ts);
    std::cout << "AST: " << ast->type << std::endl;

    return 0;
}