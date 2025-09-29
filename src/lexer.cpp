#include "lexer.hpp"

namespace YY
{
    namespace Lexer
    {
        uint8_t dig_hex_to_n(unsigned char x)
        {
            if (x >= '0' && x <= '9')
            {
                return x - '0';
            }
            else if (x >= 'a' && x <= 'f')
            {
                return x - 'a' + 10;
            }
            else if (x >= 'A' && x <= 'F')
            {
                return x - 'A' + 10;
            }
            return -1;
        }

        uint8_t dig_bin_to_n(unsigned char x)
        {
            if (x == '0' || x == '1')
            {
                return x - '0';
            }
            return -1;
        }

        uint8_t dig_oct_to_n(unsigned char x)
        {
            if (x >= '0' && x <= '7')
            {
                return x - '0';
            }
            return -1;
        }

        uint8_t dig_dec_to_n(unsigned char x)
        {
            if (x >= '0' && x <= '9')
            {
                return x - '0';
            }
            return -1;
        }

        bool textToNumeric(const char *buf, const int16_t len, int64_t& numeric_int, int64_t& numeric_float)
        {
            // TODO: Check big integer overflows;

            uint8_t x = 0;
            numeric_int = 0;
            numeric_float = 0;

            if (len >= 2 && buf[1] == 'x')
            {
                if (len == 2)
                {
                    return true;
                }

                for (int i = 2; i < len; i++)
                {
                    x = dig_hex_to_n(buf[i]);
                    if (x == 0xff)
                    {
                        return true;
                    }
                    numeric_int <<= 4;
                    numeric_int |= x;
                }
            }
            else if (len >= 2 && buf[1] == 'o')
            {
                if (len == 2)
                {
                    return true;
                }

                for (int i = 2; i < len; i++)
                {
                    x = dig_oct_to_n(buf[i]);
                    if (x == 0xff)
                    {
                        return true;
                    }
                    numeric_int <<= 3;
                    numeric_int |= x;
                }
            }
            else if (len >= 2 && buf[1] == 'b')
            {
                if (len == 2)
                {
                    return true;
                }

                for (int i = 2; i < len; i++)
                {
                    x = dig_bin_to_n(buf[i]);
                    if (x == 0xff)
                    {
                        return true;
                    }
                    numeric_int <<= 1;
                    numeric_int |= x;
                }
            }
            else
            {
                int i = 0;
                bool is_float = 0;

                for ( ; i < len; i++)
                {
                    x = dig_dec_to_n(buf[i]);
                    if (x == 0xff)
                    {
                        if (buf[i] == '.')
                        {
                            is_float = true;
                            i++;
                            break;
                        }
                        return true;
                    }

                    numeric_int *= 10;
                    numeric_int += x;
                }

                if (is_float)
                {
                    int32_t base = i;
                    int64_t delta = 1;

                    for (i = len - 1; i >= base; i--)
                    {
                        x = dig_dec_to_n(buf[i]);

                        if (x == 0xff)
                        {
                            return true;
                        }

                        numeric_float += delta * x;
                        delta *= 10;
                    }
                }
            }

            return false;
        }

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
            token.location.line = line + 1;
            token.location.column = cursor - bol + 1;

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
                    if (content[cursor] == '\n')
                    {
                        line++;
                        bol = ++cursor;
                    }
                    else
                    {
                        cursor++;
                    }
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
                    if (content[cursor] == '\n')
                    {
                        line++;
                        bol = ++cursor;
                    }
                    else
                    {
                        cursor++;
                    }
                    token.length++;
                }
                token.length += 2;
                cursor += 2;
                return token;
            }
            else if (isDigit(content[cursor])) {
                token.text = &content[cursor];
                token.kind = Token::Kind::C_NUMBER;
                while (cursor < length && (isDigit(content[cursor]) || content[cursor] == 'o' || content[cursor] == 'x' || content[cursor] == 'b' || content[cursor] == '.' || content[cursor] == '\'' || (content[cursor] >= 'a' && content[cursor] <= 'f') || (content[cursor] >= 'A' && content[cursor] <= 'F')))
                {
                    cursor++;
                    token.length++;
                }

                textToNumeric(token.text, token.length, token.numeric_int, token.numeric_float);

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