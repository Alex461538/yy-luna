#include "glin/lexer.hpp"

#include <iostream>
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
        return isAlpha(code) || isDigit(code) || code == '_';
    }

    inline bool isWordStart(char code)
    {
        return isAlpha(code) || code == '_';
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
        Token::Token token = Token::Token();

        token.text = &content[length - 1];

        if (cursor >= length)
        {
            return token;
        }

        if (isWordStart(content[cursor]))
        {
            token.text = &content[cursor];
            token.kind = Token::Kind::T_IDENTIFIER;
            while (cursor < length && isWord(content[cursor]))
            {
                cursor++;
                token.length++;
            }
            return token;
        }

        token.kind = Token::Kind::T_INVALID;
        token.text = &content[cursor];
        token.length = 1;

        cursor++;

        return token;
    }
}