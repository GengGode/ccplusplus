#pragma once
#include <exception>
#include <string>

namespace ccpp
{
    class exception : public std::exception
    {
    public:
        exception(const char *msg) : message(msg) {}
        exception(const std::string &msg) : message(msg.c_str()) {}
        const char *what() const noexcept override { return message; }

    private:
        const char *message;
    };
} // namespace ccpp