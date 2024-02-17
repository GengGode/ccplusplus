#pragma once

#include <string>
#include <optional>
#include <variant>
#include <functional>
#include <memory>
#include <iostream>

#include "ccpp.execption.hpp"

namespace ccpp
{
    enum class token_type
    {
        number_t, // type, value
        string_t, // type, value
        bool_t,   // type, value
        var_t,    // type, value
        lambda_t, // type, vars, body
        call_t,   // type, func, args
        if_t,     // type, cond, then, else
        binary_t, // type, operator, left, right
        prog_t,   // type, prog
        let_t,    // type, vars, body
    };
    std::string to_string(token_type type)
    {
        switch (type)
        {
        case token_type::number_t:
            return "number";
        case token_type::string_t:
            return "string";
        case token_type::bool_t:
            return "bool";
        case token_type::var_t:
            return "var";
        case token_type::lambda_t:
            return "lambda";
        case token_type::call_t:
            return "call";
        case token_type::if_t:
            return "if";
        case token_type::binary_t:
            return "binary";
        case token_type::prog_t:
            return "prog";
        case token_type::let_t:
            return "let";
        default:
            throw exception("Invalid token type");
        }
    }

    /*

    {
        type,value,vars,body,func,args,cond,then,else,operator,left,right,prog
    }
    */
    class token_inface
    {
    protected:
    public:
        std::string type = "token_inface";
        std::variant<int, float, double, std::string, bool> value; // number, string, bool, var
        std::vector<std::string> vars;                             // lambda, let
        std::shared_ptr<token_inface> body;                        // lambda, let
        std::shared_ptr<token_inface> func;                        // call
        std::vector<std::shared_ptr<token_inface>> args;           // call
        std::shared_ptr<token_inface> cond;                        // if
        std::shared_ptr<token_inface> then;                        // if
        std::shared_ptr<token_inface> else_;                       // if
        std::string operator_;                                     // binary
        std::shared_ptr<token_inface> left;                        // binary
        std::shared_ptr<token_inface> right;                       // binary
        std::vector<std::shared_ptr<token_inface>> prog;           // prog
    public:
    public:
        token_inface() {}

        std::string get_type() { return type; }
        std::optional<std::variant<int, float, double, std::string, bool>> get_value() { return value; }
        std::optional<std::vector<std::string>> get_vars() { return vars; }
        std::optional<std::shared_ptr<token_inface>> get_body() { return body; }
        std::optional<std::shared_ptr<token_inface>> get_func() { return func; }
        std::optional<std::vector<std::shared_ptr<token_inface>>>
        get_args() { return args; }
        std::optional<std::shared_ptr<token_inface>> get_cond() { return cond; }
        std::optional<std::shared_ptr<token_inface>> get_then() { return then; }
        std::optional<std::shared_ptr<token_inface>> get_else() { return else_; }
        std::optional<std::string> get_operator() { return operator_; }
        std::optional<std::shared_ptr<token_inface>> get_left() { return left; }
        std::optional<std::shared_ptr<token_inface>> get_right() { return right; }
        std::optional<std::vector<std::shared_ptr<token_inface>>>
        get_prog() { return prog; }

        virtual std::string string() { return "token_inface"; }
    };

    class number_token : public token_inface
    {
    public:
        number_token(int value)
        {
            type = to_string(token_type::number_t);
            this->value = value;
        }
        number_token(float value)
        {
            type = to_string(token_type::number_t);
            this->value = value;
        }
        number_token(double value)
        {
            type = to_string(token_type::number_t);
            this->value = value;
        }

        std::string string() override
        {
            return "number token: " + std::to_string(std::get<double>(value));
        }
    };

    class string_token : public token_inface
    {
    public:
        string_token(std::string value)
        {
            type = to_string(token_type::string_t);
            this->value = value;
        }

        std::string string() override
        {
            return "string token: " + std::get<std::string>(value);
        }
    };

    class bool_token : public token_inface
    {
    public:
        bool_token(bool value)
        {
            type = to_string(token_type::bool_t);
            this->value = value;
        }

        std::string string() override
        {
            return "bool token: " + std::to_string(std::get<bool>(value));
        }
    };

    class var_token : public token_inface
    {
    public:
        var_token(std::string value)
        {
            type = to_string(token_type::var_t);
            this->value = value;
        }

        std::string string() override
        {
            return "var token: " + std::get<std::string>(value);
        }
    };

    class lambda_token : public token_inface
    {
    public:
        lambda_token(std::vector<std::string> vars, std::shared_ptr<token_inface> body)
        {
            type = to_string(token_type::lambda_t);
            this->vars = vars;
            this->body = body;
        }

        std::string string() override
        {
            std::string str = "lambda token: [](";
            for (auto &var : vars)
            {
                str += var + " ";
            }
            str += ") { " + body->string() + " }";
            return str;
        }
    };

    class call_token : public token_inface
    {
    public:
        call_token(std::shared_ptr<token_inface> func, std::vector<std::shared_ptr<token_inface>> args)
        {
            type = to_string(token_type::call_t);
            this->func = func;
            this->args = args;
        }

        std::string string() override
        {
            std::string str = "call token: " + func->string() + "(";
            for (auto &arg : args)
            {
                str += arg->string() + " ";
            }
            str += ")";
            return str;
        }
    };

    class if_token : public token_inface
    {
    public:
        if_token(std::shared_ptr<token_inface> cond, std::shared_ptr<token_inface> then, std::shared_ptr<token_inface> else_)
        {
            type = to_string(token_type::if_t);
            this->cond = cond;
            this->then = then;
            this->else_ = else_;
        }

        std::string string() override
        {
            return "if token: if " + cond->string() + " then " + then->string() + " else " + else_->string();
        }
    };

    class binary_token : public token_inface
    {
    public:
        binary_token(std::string operator_, std::shared_ptr<token_inface> left, std::shared_ptr<token_inface> right)
        {
            type = to_string(token_type::binary_t);
            this->operator_ = operator_;
            this->left = left;
            this->right = right;
        }

        std::string string() override
        {
            return "binary token: " + left->string() + " " + operator_ + " " + right->string();
        }
    };

    class prog_token : public token_inface
    {
    public:
        prog_token(std::vector<std::shared_ptr<token_inface>> prog)
        {
            type = to_string(token_type::prog_t);
            this->prog = prog;
        }

        std::string string() override
        {
            std::string str = "prog token: ";
            for (auto &p : prog)
            {
                str += p->string() + " ";
            }
            return str;
        }
    };

    class let_token : public token_inface
    {
    public:
        let_token(std::vector<std::string> vars, std::shared_ptr<token_inface> body)
        {
            type = to_string(token_type::let_t);
            this->vars = vars;
            this->body = body;
        }

        std::string string() override
        {
            std::string str = "let token: let ";
            for (auto &var : vars)
            {
                str += var + " ";
            }
            str += "in " + body->string();
            return str;
        }
    };

    struct token
    {
        std::string type;
        std::variant<bool, int, float, double, std::string, std::function<token(token)>> value;

        std::vector<std::shared_ptr<ccpp::token>> vars; // lambda, let
        std::shared_ptr<token> body;                    // lambda, let

        std::shared_ptr<ccpp::token> func;
        std::vector<std::shared_ptr<ccpp::token>> args;

        std::shared_ptr<token> cond;  // if
        std::shared_ptr<token> then;  // if
        std::shared_ptr<token> else_; // if

        std::string operator_;                    // binary
        std::shared_ptr<token> left;              // binary
        std::shared_ptr<token> right;             // binary
        std::vector<std::shared_ptr<token>> prog; // prog

        std::shared_ptr<token> next;
        std::string peek;
        std::string eof;
        std::shared_ptr<ccpp::exception> croak;

        token() {}
        token(std::string type, std::variant<bool, int, float, double, std::string, std::function<token(token)>> value) : type(type), value(value) {}

        template <typename T>
            requires std::is_same_v<T, bool> || std::is_same_v<T, int> || std::is_same_v<T, float> || std::is_same_v<T, double> || std::is_same_v<T, std::string> || std::is_same_v<T, std::function<token(token)>>
        static std::shared_ptr<token> create(std::string type, T value)
        {
            return std::make_shared<token>(type, value);
        }

        // std::cout <<
        friend std::ostream &operator<<(std::ostream &os, const token &t)
        {
            std::visit([&os](auto &&arg)
                       {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, bool>)
                {
                    os << std::boolalpha << arg;
                }
                else if constexpr (std::is_same_v<T, int>)
                {
                    os << arg;
                }
                else if constexpr (std::is_same_v<T, float>)
                {
                    os << arg;
                }
                else if constexpr (std::is_same_v<T, double>)
                {
                    os << arg;
                }
                else if constexpr (std::is_same_v<T, std::string>)
                {
                    os << arg;
                }
                else if constexpr (std::is_same_v<T, std::function<token(token)>>)
                {
                    os << "function";
                } },
                       t.value);
            return os;
        }
    };
}