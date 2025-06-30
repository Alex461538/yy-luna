#include "glin/token.hpp"

#include <string>

namespace Token {
    std::string kindName(Kind kind)
    {
        switch (kind)
        {
        case Kind::T_EOF: return "End of file";
        case Kind::T_EOL: return "End of line";
        case Kind::T_INVALID: return "Invalid token";
        case Kind::T_CONSTANT: return "Constant";
        case Kind::T_STRING: return "String literal";
        case Kind::T_KEYWORD: return "Keyword";
        case Kind::T_IDENTIFIER: return "Symbol name";
        case Kind::T_PUNCTUATOR: return "Punctuator";
        default:
            return "unnamed";
        }
    }

    Token::operator std::string() const {
        return (text ? "rrr" : "") + kindName(kind);
    }
}