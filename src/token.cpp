#include "token.hpp"

/*
    kukuma 0 e
    mibiba kukuma 0 e
        siha "" siha
    iba
*/

namespace YY
{
    namespace Token
    {
        Location::Location()
        {
            source = 0;
            line = 0;
            column = 0;
        }

        Token::Token()
        {
            enclosing = -1;
            text = 0;
            docs = 0;
            length = 0;
            docs_length = 0;
            location = Location();
            kind = Kind::T_EOF;
        }

        const std::map<std::string, Kind> specialTokens = {
            {"=", Kind::O_ASSIGN},
            {"+", Kind::O_PLUS},
            {"+=", Kind::O_PLUS_EQ},
        };

        const std::map<std::string, Kind> keywordTokens = {
            {"import", Kind::K_IMPORT},
            {"as", Kind::K_AS},
            {"namespace", Kind::K_NAMESPACE},
            {"var", Kind::K_VAR},
            {"class", Kind::K_CLASS},
            {"function", Kind::K_FUNCTION},
            {"return", Kind::K_RETURN}};

        const std::map<char, Kind> literalTokens = {
            {'(', Kind::T_OPEN_PAREN},
            {')', Kind::T_CLOSE_PAREN},
            {'{', Kind::T_OPEN_CURLY},
            {'}', Kind::T_CLOSE_CURLY},
            {'[', Kind::T_OPEN_SQUARE},
            {']', Kind::T_CLOSE_SQUARE},
            {';', Kind::T_SEMICOLON},
        };

        std::string kindName(Kind kind)
        {
            switch (kind)
            {
            case Kind::T_EOF:
                return "End of file";
            case Kind::T_SEMICOLON:
                return "End of expr";
            case Kind::T_INVALID:
                return "Invalid token";
            case Kind::T_IDENTIFIER:
                return "Symbol name";
            case Kind::T_PUNCTUATOR:
                return "Punctuator";

            case Kind::T_OPEN_PAREN:
                return "Open paren";
            case Kind::T_CLOSE_PAREN:
                return "Close paren";
            case Kind::T_OPEN_CURLY:
                return "Open curly";
            case Kind::T_CLOSE_CURLY:
                return "Close curly";
            case Kind::T_OPEN_SQUARE:
                return "Open square";
            case Kind::T_CLOSE_SQUARE:
                return "Close square";

            case Kind::K_IMPORT:
                return "KW import";
            case Kind::K_AS:
                return "KW as";
            case Kind::K_NAMESPACE:
                return "KW namespace";
            case Kind::K_VAR:
                return "KW var";
            case Kind::K_CLASS:
                return "KW class";
            case Kind::K_FUNCTION:
                return "KW function";
            case Kind::K_RETURN:
                return "KW return";

            case Kind::C_STRING:
                return "String literal";
            case Kind::C_NUMBER:
                return "Number literal";
            case Kind::C_COMMENT:
                return "Comment literal";

            case Kind::O_ASSIGN:
                return "Assign";
            case Kind::O_PLUS:
                return "Plus";
            case Kind::O_PLUS_EQ:
                return "Plus eq";
            default:
                return "unnamed";
            }
        }

        Token::operator std::string() const
        {
            return std::format("{} <{}> L:{} C:{} e:{}", (text ? std::string(text, length) : ""), kindName(kind), std::to_string(location.line), std::to_string(location.column), std::to_string(enclosing));
        }
    }
}