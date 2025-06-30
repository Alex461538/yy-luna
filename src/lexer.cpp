#include "glin/lexer.hpp"

#include "glin/token.hpp"

namespace Lexer
{
    inline bool isWhitespace(char code)
    {
        return code == ' ' || code == '\n' || code == '\t' || code == '\r';
    }

    inline bool isDigit(char code)
    {
        return code >= '0' && code <= '9';
    }

    inline bool isAlpha(char code)
    {
        return (code >= 'A' && code <= 'Z') || (code >= 'a' && code <= 'z');
    }

    inline bool isWord(char code)
    {
        return isAlpha(code) || isDigit(code);
    }

    void Lexer::Lever()
    {
        content = nullptr;
        cursor = 0;
        line = 0;
        bol = 0;
    }

    void Lexer::trim()
    {
        if (!content)
        {
            return;
        }

        while (cursor < length && isWhitespace(content[cursor]))
        {
            if (content[cursor] == '\n')
            {
                line++;
                bol = ++cursor;
            }
            else
            {
                cursor++;
            }
        }
    }

    Token::Token Lexer::next()
    {
        Token::Token token;

        token.text = &content[length - 1];

        if (cursor >= length)
        {
            return token;
        }

        return token;
    }
}