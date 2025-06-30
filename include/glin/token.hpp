#ifndef _GLIN_TOKEN_HPP_
#define _GLIN_TOKEN_HPP_

#include <inttypes.h>
#include <string>
#include <map>

namespace Token
{
    enum class Kind : uint16_t
    {
        T_IS_OPERATOR = 0x80'00,
        T_IS_CONSTANT = 0x40'00,
        T_EOF = 0,

        T_SEMICOLON,
        T_INVALID,
        T_IDENTIFIER,
        T_PUNCTUATOR,

        T_OPEN_PAREN,
        T_CLOSE_PAREN,
        T_OPEN_CURLY,
        T_CLOSE_CURLY,
        T_OPEN_SQUARE,
        T_CLOSE_SQUARE,

        K_IMPORT,
        K_AS,
        K_NAMESPACE,
        K_VAR,
        K_CLASS,
        K_FUNCTION,
        K_RETURN,

        C_NUMBER = T_IS_CONSTANT,
        C_TRUE,
        C_FALSE,
        C_INFINITY,
        C_STRING,
        C_COMMENT,

        O_ASSIGN = T_IS_OPERATOR,
        O_PLUS,
        O_PLUS_EQ
    };

    extern const std::map<char, Kind> literalTokens;
    extern const std::map<std::string, Kind> keywordTokens;
    extern const std::map<std::string, Kind> specialTokens;

    std::string kindName(Kind kind);

    struct Location
    {
        const char *source;
        size_t line;
        size_t column;

        Location();
    };

    struct Token
    {
        Location location;
        const char *text;
        const char *docs;
        size_t length;
        size_t docs_length;
        Kind kind;

        Token();

        operator std::string() const;
    };
}

#endif