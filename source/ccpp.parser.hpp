#pragma once

#include <map>

#include "ccpp.token_stream.hpp"

namespace ccpp
{

    class parser
    {
        std::map<std::string, int> precedence = {
            {"=", 1},
            {"||", 2},
            {"&&", 3},
            {"<", 7},
            {">", 7},
            {"<=", 7},
            {">=", 7},
            {"==", 7},
            {"!=", 7},
            {"+", 10},
            {"-", 10},
            {"*", 20},
            {"/", 20},
            {"%", 20}};
        ccpp::token_stream ts;

    public:
        parser(ccpp::token_stream ts) : ts(ts) {}
        auto parse(ccpp::token_stream ts)
        {
            return parse_toplevel(ts);
        }

        /*
         function is_punc(ch) {
             var tok = input.peek();
             return tok && tok.type == "punc" && (!ch || tok.value == ch) && tok;
         }
         function is_kw(kw) {
             var tok = input.peek();
             return tok && tok.type == "kw" && (!kw || tok.value == kw) && tok;
         }
         function is_op(op) {
             var tok = input.peek();
             return tok && tok.type == "op" && (!op || tok.value == op) && tok;
         }
         function skip_punc(ch) {
             if (is_punc(ch)) input.next();
             else input.croak("Expecting punctuation: \"" + ch + "\"");
         }
         function skip_kw(kw) {
             if (is_kw(kw)) input.next();
             else input.croak("Expecting keyword: \"" + kw + "\"");
         }
         function skip_op(op) {
             if (is_op(op)) input.next();
             else input.croak("Expecting operator: \"" + op + "\"");
         }
         function unexpected() {
             input.croak("Unexpected token: " + JSON.stringify(input.peek()));
         }
         */
        bool is_punc(std::string ch, ccpp::token_stream ts)
        {
            auto tok = ts.peek();
            return tok->type == "punc" && (ch.empty() || std::holds_alternative<std::string>(tok->value) || std::get<std::string>(tok->value) == ch);
        }
        bool is_kw(std::string kw, ccpp::token_stream ts)
        {
            auto tok = ts.peek();
            return tok->type == "kw" && (kw.empty() || std::holds_alternative<std::string>(tok->value) || std::get<std::string>(tok->value) == kw);
        }
        bool is_op(std::string op, ccpp::token_stream ts)
        {
            auto tok = ts.peek();
            return tok->type == "op" && (op.empty() || std::holds_alternative<std::string>(tok->value) || std::get<std::string>(tok->value) == op);
        }
        void skip_punc(std::string ch, ccpp::token_stream ts)
        {
            if (is_punc(ch, ts))
                ts.next();
            else
                ts.croak("Expecting punctuation: \"" + ch + "\"");
        }
        void skip_kw(std::string kw, ccpp::token_stream ts)
        {
            if (is_kw(kw, ts))
                ts.next();
            else
                ts.croak("Expecting keyword: \"" + kw + "\"");
        }
        void skip_op(std::string op, ccpp::token_stream ts)
        {
            if (is_op(op, ts))
                ts.next();
            else
                ts.croak("Expecting operator: \"" + op + "\"");
        }
        void unexpected(ccpp::token_stream ts)
        {
            ts.croak("Unexpected token: " + ts.peek()->type);
        }
        /*
         function maybe_binary(left, my_prec) {
             var tok = is_op();
             if (tok) {
                 var his_prec = PRECEDENCE[tok.value];
                 if (his_prec > my_prec) {
                     input.next();
                     return maybe_binary({
                         type     : tok.value == "=" ? "assign" : "binary",
                         operator : tok.value,
                         left     : left,
                         right    : maybe_binary(parse_atom(), his_prec)
                     }, my_prec);
                 }
             }
             return left;
         }*/
        std::shared_ptr<ccpp::token> maybe_binary(std::shared_ptr<ccpp::token> left, int my_prec, ccpp::token_stream ts)
        {
            if (is_op("", ts))
            {
                auto tok = ts.peek();
                auto his_prec = precedence[std::get<std::string>(tok->value)];
                if (his_prec > my_prec)
                {
                    ts.next();
                    auto right = maybe_binary(parse_atom(ts), his_prec, ts);
                    auto tok = std::make_shared<ccpp::token>();
                    tok->type = std::get<std::string>(tok->value) == "=" ? "assign" : "binary";
                    tok->operator_ = std::get<std::string>(tok->value);
                    tok->left = left;
                    tok->right = right;
                    return maybe_binary(tok, my_prec, ts);
                }
            }
            return left;
        }
        /*
        function delimited(start, stop, separator, parser) {
            var a = [], first = true;
            skip_punc(start);
            while (!input.eof()) {
                if (is_punc(stop)) break;
                if (first) first = false; else skip_punc(separator);
                if (is_punc(stop)) break;
                a.push(parser());
            }
            skip_punc(stop);
            return a;
        }
        */
        auto delimited(std::string start, std::string stop, std::string separator, std::function<std::shared_ptr<ccpp::token>(ccpp::token_stream)> parser, ccpp::token_stream ts)
        {
            std::vector<std::shared_ptr<ccpp::token>> a;
            bool first = true;
            skip_punc(start, ts);
            while (!ts.eof())
            {
                if (is_punc(stop, ts))
                    break;
                if (first)
                    first = false;
                else
                    skip_punc(separator, ts);
                if (is_punc(stop, ts))
                    break;
                a.push_back(parser(ts));
            }
            skip_punc(stop, ts);
            return a;
        }
        /*
         function parse_call(func) {
             return {
                 type: "call",
                 func: func,
                 args: delimited("(", ")", ",", parse_expression),
             };
         }
         function parse_varname() {
             var name = input.next();
             if (name.type != "var") input.croak("Expecting variable name");
             return name.value;
         }
         */
        auto parse_call(std::shared_ptr<ccpp::token> func, ccpp::token_stream ts)
        {
            auto call = std::make_shared<ccpp::token>();
            call->func = func;
            call->args = delimited(
                "(", ")", ",", [&](auto ts)
                { return parse_expression(ts); },
                ts);
            return call;
        }
        auto parse_varname(ccpp::token_stream ts)
        {
            auto name = ts.next();
            if (name->type != "var")
                ts.croak("Expecting variable name");
            return name;
        }

        /*
         function parse_if() {
             skip_kw("if");
             var cond = parse_expression();
             if (!is_punc("{")) skip_kw("then");
             var then = parse_expression();
             var ret = {
                 type: "if",
                 cond: cond,
                 then: then,
             };
             if (is_kw("else")) {
                 input.next();
                 ret.else = parse_expression();
             }
             return ret;
         }
         */
        auto parse_if(ccpp::token_stream ts)
        {
            skip_kw("if", ts);
            auto ret = std::make_shared<ccpp::token>();
            ret->cond = parse_expression(ts);
            if (!is_punc("{", ts))
                skip_kw("then", ts);
            ret->then = parse_expression(ts);
            if (is_kw("else", ts))
            {
                ts.next();
                ret->else_ = parse_expression(ts);
            }
            return ret;
        }
        /*
         function parse_lambda() {
             return {
                 type: "lambda",
                 vars: delimited("(", ")", ",", parse_varname),
                 body: parse_expression()
             };
         }
         */
        auto parse_lambda(ccpp::token_stream ts)
        {
            auto ret = std::make_shared<ccpp::token>();
            ret->type = "lambda";
            ret->vars = delimited(
                "(", ")", ",", [&](auto ts)
                { return parse_expression(ts); },
                ts);
            ret->body = parse_expression(ts);
            return ret;
        }
        /*
         function parse_bool() {
             return {
                 type  : "bool",
                 value : input.next().value == "true"
             };
         }
         */
        auto parse_bool(ccpp::token_stream ts)
        {
            auto ret = std::make_shared<ccpp::token>();
            ret->type = "bool";
            auto tok = ts.next();
            ret->value = std::holds_alternative<std::string>(tok->value) && std::get<std::string>(tok->value) == "true";
            return ret;
        }
        /*
         function maybe_call(expr) {
             expr = expr();
             return is_punc("(") ? parse_call(expr) : expr;
         }
         */
        auto maybe_call(std::function<std::shared_ptr<ccpp::token>()> expr, ccpp::token_stream ts)
        {
            auto expr_ret = expr();
            return is_punc("(", ts) ? parse_call(expr_ret, ts) : expr_ret;
        }
        /*
         function parse_atom() {
             return maybe_call(function(){
                 if (is_punc("(")) {
                     input.next();
                     var exp = parse_expression();
                     skip_punc(")");
                     return exp;
                 }
                 if (is_punc("{")) return parse_prog();
                 if (is_kw("if")) return parse_if();
                 if (is_kw("true") || is_kw("false")) return parse_bool();
                 if (is_kw("lambda") || is_kw("λ")) {
                     input.next();
                     return parse_lambda();
                 }
                 var tok = input.next();
                 if (tok.type == "var" || tok.type == "num" || tok.type == "str")
                     return tok;
                 unexpected();
             });
         }
         */
        std::shared_ptr<ccpp::token> parse_atom(ccpp::token_stream ts)
        {
            return maybe_call([&]()
                              {
            if (is_punc("(", ts))
            {
                ts.next();
                auto exp = parse_expression(ts);
                skip_punc(")", ts);
                return exp;
            }
            if (is_punc("{", ts))
                return parse_prog(ts);
            if (is_kw("if", ts))
                return parse_if(ts);
            if (is_kw("true", ts) || is_kw("false", ts))
                return parse_bool(ts);
            if (is_kw("lambda", ts) || is_kw("λ", ts))
            {
                ts.next();
                return parse_lambda(ts);
            }
            auto tok = ts.next();
            if (tok->type == "var" || tok->type == "num" || tok->type == "str")
                return tok;
            unexpected(ts);
            return std::make_shared<ccpp::token>(); },
                              ts);
        }

        /*
         function parse_toplevel() {
             var prog = [];
             while (!input.eof()) {
                 prog.push(parse_expression());
                 if (!input.eof()) skip_punc(";");
             }
             return { type: "prog", prog: prog };
         }
         */
        std::shared_ptr<ccpp::token> parse_toplevel(ccpp::token_stream ts)
        {
            auto prog = std::make_shared<ccpp::token>();
            prog->type = "prog";
            while (!ts.eof())
            {
                prog->prog.push_back(parse_expression(ts));
                if (!ts.eof())
                    skip_punc(";", ts);
            }
            return prog;
        }
        /*
         function parse_prog() {
             var prog = delimited("{", "}", ";", parse_expression);
             if (prog.length == 0) return FALSE;
             if (prog.length == 1) return prog[0];
             return { type: "prog", prog: prog };
         }
         */
        std::shared_ptr<ccpp::token> parse_prog(ccpp::token_stream ts)
        {
            auto prog = delimited(
                "{", "}", ";", [&](auto ts)
                { return parse_expression(ts); },
                ts);
            if (prog.size() == 0)
                return std::make_shared<ccpp::token>();
            if (prog.size() == 1)
                return prog[0];
            auto ret = std::make_shared<ccpp::token>();
            ret->type = "prog";
            ret->prog = prog;
            return ret;
        }
        /*
         function parse_expression() {
             return maybe_call(function(){
                 return maybe_binary(parse_atom(), 0);
             });
         }
         */
        std::shared_ptr<ccpp::token> parse_expression(ccpp::token_stream ts)
        {
            return maybe_call([&]()
                              { return maybe_binary(parse_atom(ts), 0, ts); },
                              ts);
        }
        /*
     }
     */
    };
}