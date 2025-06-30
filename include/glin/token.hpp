#ifndef _GLIN_TOKEN_HPP_
#define _GLIN_TOKEN_HPP_

#include <inttypes.h>
#include <string>

namespace Token {
    enum class Kind: uint16_t {
        T_EOF = 0,
        T_EOL,
        T_INVALID,
        T_CONSTANT,
        T_STRING,
        T_KEYWORD,
        T_IDENTIFIER,
        T_PUNCTUATOR
    };

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