#if !defined(__hpp_yy_token__)
#define __hpp_yy_token__

#include <inttypes.h>
#include <string>
#include <map>
#include <format>

namespace YY
{
    namespace Token
    {
        enum class Kind : uint16_t
        {
            T_IS_OPERATOR = 0x80'00,
            T_IS_CONSTANT = 0x40'00,
            T_EOF = 0,

            T_COMMA,
            T_SEMICOLON,
            T_INVALID,
            T_IDENTIFIER,
            T_COLON,
            T_PUNCTUATOR,

            T_OPEN_PAREN,
            T_CLOSE_PAREN,
            T_OPEN_CURLY,
            T_CLOSE_CURLY,
            T_OPEN_SQUARE,
            T_CLOSE_SQUARE,

            K_IMPORT,
            K_AS,
            K_FROM,
            K_NAMESPACE,
            K_VAR,
            K_CLASS,
            K_FUNCTION,
            K_RETURN,

            C_NUMBER = T_IS_CONSTANT,
            C_BOOL,
            C_INFINITY,
            C_STRING,
            C_COMMENT,
            C_NAN,

            O_ASSIGN = T_IS_OPERATOR,
            O_ACCESS,
            O_PLUS,
            O_PLUS_EQ
        };

        extern const std::map<char, Kind> literalTokens;
        extern const std::map<std::string, Kind> keywordTokens;
        extern const std::map<std::string, Kind> specialTokens;

        std::string kindName(Kind kind);

        struct Location
        {
            const char *source;
            int32_t line;
            int32_t column;

            Location();
        };

        struct Token
        {
            Location location;
            const char *text;
            int64_t numeric_int;
            int64_t numeric_float;
            int32_t length;
            int32_t enclosing;
            Kind kind;
            bool ignore;

            Token();

            operator std::string() const;
        };
    }
}

#endif