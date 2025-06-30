#ifndef _GLIN_LEXER_HPP_
#define _GLIN_LEXER_HPP_

#include <iostream>
#include "glin/token.hpp"

namespace Lexer
{
    inline bool isWhitespace(char code);
    inline bool isDigit(char code);
    inline bool isAlpha(char code);
    inline bool isWord(char code);
    inline bool isWordStart(char code);

    struct Lexer
    {
        const char *content;
        size_t length;
        size_t cursor;
        size_t line;
        size_t bol;

        Lexer()
        {
            std::cout << "Constructed lexer";
            content = nullptr;
            cursor = 0;
            line = 0;
            bol = 0;
        }

        void trim();

        Token::Token next();
    };
}

#endif