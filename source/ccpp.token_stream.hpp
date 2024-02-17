#pragma once

#include "ccpp.input_stream.hpp"
#include "ccpp.token.hpp"

namespace ccpp
{
    class token_stream
    {
        std::shared_ptr<token> current = nullptr;
        std::string keywords = " if then else lambda λ true false ";
        ccpp::input_stream input;

    public:
        token_stream(ccpp::input_stream input) : input(input) {}
        std::shared_ptr<token> next()
        {
            auto tok = current;
            current = nullptr;
            return tok == nullptr ? read_next() : tok;
        }
        std::shared_ptr<token> peek()
        {
            if (current == nullptr)
                current = read_next();
            return current;
        }
        bool eof()
        {
            return peek() == nullptr;
        }
        void croak(std::string msg)
        {
            input.croak(msg);
        }
        bool is_keyword(std::string x)
        {
            return keywords.find(" " + x + " ") != std::string::npos;
        }
        bool is_digit(char ch)
        {
            return std::isdigit(ch);
        }
        bool is_id_start(char ch)
        {
            return std::isalpha(ch) || ch == '_' || ch == 'λ';
        }
        bool is_id(char ch)
        {
            return is_id_start(ch) || std::string("?!-<>=0123456789").find(ch) != std::string::npos;
        }
        bool is_op_char(char ch)
        {
            return std::string("+-*/%=&|<>!").find(ch) != std::string::npos;
        }
        bool is_punc(char ch)
        {
            return std::string(",;(){}[]").find(ch) != std::string::npos;
        }
        bool is_whitespace(char ch)
        {
            return std::string(" \t\n").find(ch) != std::string::npos;
        }
        std::string read_while(std::function<bool(char)> predicate)
        {
            std::string str = "";
            while (!input.eof() && predicate(input.peek()))
                str += input.next();
            return str;
        }
        std::shared_ptr<token> read_number()
        {
            bool has_dot = false;
            std::string number = read_while([&](char ch)
                                            {
                if (ch == '.')
                {
                    if (has_dot)
                        return false;
                    has_dot = true;
                    return true;
                }
                return is_digit(ch); });
            return token::create("num", std::stod(number));
        }
        std::shared_ptr<token> read_ident()
        {
            std::string id = read_while([&](char ch)
                                        { return is_id(ch); });
            return token::create(is_keyword(id) ? "kw" : "var", id);
        }
        std::string read_escaped(char end)
        {
            bool escaped = false;
            std::string str = "";
            input.next();
            while (!input.eof())
            {
                char ch = input.next();
                if (escaped)
                {
                    str += ch;
                    escaped = false;
                }
                else if (ch == '\\')
                {
                    escaped = true;
                }
                else if (ch == end)
                {
                    break;
                }
                else
                {
                    str += ch;
                }
            }
            return str;
        }
        std::shared_ptr<token> read_string()
        {
            return token::create("string", read_escaped('"'));
        }
        void skip_comment()
        {
            read_while([&](char ch)
                       { return ch != '\n'; });
            input.next();
        }
        std::shared_ptr<token> read_next()
        {
            read_while([&](char ch)
                       { return is_whitespace(ch); });
            if (input.eof())
                return nullptr;
            char ch = input.peek();
            if (ch == '#')
            {
                skip_comment();
                return read_next();
            }
            if (ch == '"')
                return read_string();
            if (is_digit(ch))
                return read_number();
            if (is_id_start(ch))
                return read_ident();
            if (is_punc(ch))
                return token::create("punc", std::string(1, input.next()));
            if (is_op_char(ch))
                return token::create("op", read_while([&](char ch)
                                                      { return is_op_char(ch); }));
            input.croak("Can't handle character: " + std::string(1, ch));
            return nullptr;
        }
    };

} // namespace ccpp
