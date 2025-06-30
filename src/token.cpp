#include "glin/token.hpp"

#include <iostream>
#include <string>

namespace Token {
    const std::map<std::string, Kind> keywordTokens = {
        {"import", Kind::T_IMPORT},
        {"as", Kind::T_AS},
        {"namespace", Kind::T_NAMESPACE},
        {"var", Kind::T_VAR},
        {"class", Kind::T_CLASS},
        {"function", Kind::T_FUNCTION},
        {"return", Kind::T_RETURN}
    };

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
        case Kind::T_EOF: return "End of file";
        case Kind::T_SEMICOLON: return "End of expr";
        case Kind::T_INVALID: return "Invalid token";
        case Kind::T_CONSTANT: return "Constant";
        case Kind::T_STRING: return "String literal";
        case Kind::T_KEYWORD: return "Keyword";
        case Kind::T_IDENTIFIER: return "Symbol name";
        case Kind::T_PUNCTUATOR: return "Punctuator";
        case Kind::T_OPEN_PAREN: return "Open paren";
        case Kind::T_CLOSE_PAREN: return "Close paren";
        case Kind::T_OPEN_CURLY: return "Open curly";
        case Kind::T_CLOSE_CURLY: return "Close curly";
        case Kind::T_OPEN_SQUARE: return "Open square";
        case Kind::T_CLOSE_SQUARE: return "Close square";
        default:
            return "unnamed";
        }
    }

    Token::operator std::string() const {
        return (text ? std::string(text, length) : "") + " <" + kindName(kind) + ">";
    }
}