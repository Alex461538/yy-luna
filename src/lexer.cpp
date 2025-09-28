#include "lexer.hpp"

namespace YY
{
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

            trim();

            token.text = &content[length - 1];

            if (cursor >= length)
            {
                return token;
            }

            if (content[cursor] == '"')
            {
                token.text = &content[cursor];
                token.kind = Token::Kind::C_STRING;
                token.length = 1;
                cursor++;
                while (cursor < length && content[cursor] != '"')
                {
                    cursor++;
                    token.length++;
                }
                token.length++;
                cursor++;
                return token;
            }
            else if (cursor < length - 1 && content[cursor] == '/' && content[cursor + 1] == '*')
            {
                token.text = &content[cursor];
                token.kind = Token::Kind::C_COMMENT;
                token.length = 2;
                cursor += 2;
                while (cursor < length - 1 && !(content[cursor + 1] == '/' && content[cursor] == '*'))
                {
                    cursor++;
                    token.length++;
                }
                token.length += 2;
                cursor += 2;
                return token;
            }
            else if (isWordStart(content[cursor]))
            {
                token.text = &content[cursor];
                token.kind = Token::Kind::T_IDENTIFIER;
                while (cursor < length && isWord(content[cursor]))
                {
                    cursor++;
                    token.length++;
                }
                if (auto keywordToken = Token::keywordTokens.find(std::string(token.text, token.length)); keywordToken != Token::keywordTokens.end())
                {
                    token.kind = keywordToken->second;
                }
                return token;
            }
            else if (auto literalToken = Token::literalTokens.find(content[cursor]); literalToken != Token::literalTokens.end())
            {
                token.kind = literalToken->second;
                token.text = &content[cursor++];
                token.length = 1;
                return token;
            }
            else
            {
                token.text = &content[cursor];
                token.kind = Token::Kind::T_INVALID;

                /**
                 * The green pipe black chained muncher (GPBCM)
                 */

                while (cursor < length)
                {
                    if (auto tempinfo = Token::specialTokens.find(std::string(token.text, token.length + 1)); tempinfo != Token::specialTokens.end())
                    {
                        token.kind = tempinfo->second;
                        token.length++;
                    }
                    else
                    {
                        break;
                    }
                    cursor++;
                }

                if (token.kind != Token::Kind::T_INVALID)
                {
                    return token;
                }
            }

            token.kind = Token::Kind::T_INVALID;
            token.text = &content[cursor];
            token.length = 1;

            cursor++;

            return token;
        }
    }
}