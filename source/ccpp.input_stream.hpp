#pragma once

#include <string>

#include "ccpp.execption.hpp"

namespace ccpp
{
    class input_stream
    {
        int pos = 0;
        int line = 1;
        int col = 0;
        std::string input;

    public:
        input_stream(std::string input) : input(input) {}
        char next()
        {
            char ch = input[pos++];
            if (ch == '\n')
            {
                line++;
                col = 0;
            }
            else
            {
                col++;
            }
            return ch;
        }
        char peek()
        {
            return input[pos];
        }
        bool eof()
        {
            return peek() == '\0';
        }
        void croak(std::string msg)
        {
            throw exception((msg + " (" + std::to_string(line) + ":" + std::to_string(col) + ")"));
        }
    };
} // namespace ccpp
