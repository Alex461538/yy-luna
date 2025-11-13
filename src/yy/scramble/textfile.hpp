#if !defined(__hpp_yy_textfile__)
#define __hpp_yy_textfile__

//  --- Global includes ---
#include <optional>
#include <variant>
#include <filesystem>
#include <vector>
//  --- Project includes ---
//  --- Local includes ---
#include "../global.hpp"
#include "../out.hpp"

#include "../lex/token.hpp"
#include "../lex/lexer.hpp"

namespace YY
{
    struct TextFile {
        struct ImportEntry {
            std::string name;
            Global::VersionSelector versionSelector;
            std::optional<std::shared_ptr<TextFile>> reference;
        };

        std::string name;
        std::filesystem::path dir;

        std::string content;
        std::vector<YY::Token::Token> tokens;

        bool fromPath(std::filesystem::path _path);
        bool setContent(std::string new_content);

        bool lex();
    };
}

#endif