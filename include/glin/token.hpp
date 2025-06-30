#ifndef _GLIN_TOKEN_HPP_
#define _GLIN_TOKEN_HPP_

#include <inttypes.h>
#include <string>
#include <map>

namespace Token
{
    enum class Kind : uint16_t
    {
        T_EOF = 0,
        T_SEMICOLON,
        T_INVALID,
        T_CONSTANT,
        T_STRING,
        T_KEYWORD,
        T_IDENTIFIER,
        T_PUNCTUATOR,

        T_OPEN_PAREN,
        T_CLOSE_PAREN,
        T_OPEN_CURLY,
        T_CLOSE_CURLY,
        T_OPEN_SQUARE,
        T_CLOSE_SQUARE,

        T_IMPORT,
        T_AS,
        T_NAMESPACE,
        T_VAR,
        T_CLASS,
        T_FUNCTION,
        T_RETURN
    };

    extern const std::map<char, Kind> literalTokens;
    extern const std::map<std::string, Kind> keywordTokens;

    std::string kindName(Kind kind);

    struct Location
    {
        const char *source;
        long line;
        long column;
    };

    struct Token
    {
        const char *text;
        const char *docs;
        size_t length;
        size_t docs_length;
        Kind kind = Kind::T_EOF;

        operator std::string() const;
    };
}

#endif