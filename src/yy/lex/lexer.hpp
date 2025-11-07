#if !defined(__hpp_yy_lexer__)
#define __hpp_yy_lexer__

#include <iostream>

#include "token.hpp"

namespace YY
{
    namespace Lexer
    {
        bool textToNumeric(const char *buf, const int16_t len, int64_t& numeric_int, int64_t& numeric_float);

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
                content = nullptr;
                cursor = 0;
                line = 0;
                bol = 0;
            }

            void trim();

            Token::Token next();
        };
    }
}

#endif