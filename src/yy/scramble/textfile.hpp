#if !defined(__hpp_yy_textfile__)
#define __hpp_yy_textfile__

//  --- Global includes ---
#include <optional>
#include <variant>
#include <filesystem>
#include <vector>
#include <map>
//  --- Project includes ---
//  --- Local includes ---
#include "../global.hpp"
#include "../out.hpp"

#include "../lex/token.hpp"
#include "../lex/lexer.hpp"

namespace YY
{
    struct Package;

    struct TextFile {
        struct ImportEntry {
            std::string name;
            std::optional<Global::VersionSelector> versionSelector;
            std::optional<std::variant<std::shared_ptr<TextFile>, std::shared_ptr<Package>>> reference;
            std::map<std::string, std::string> namespaces;

            ImportEntry();
            ImportEntry( std::string query );
        };

        std::string name;
        std::filesystem::path dir;

        std::string content;

        std::vector<YY::Token::Token> tokens;

        std::vector<TextFile::ImportEntry> imports;

        bool fromPath(std::filesystem::path _path);
        bool setContent(std::string new_content);

        bool lex();
        void preprocess();
    };
}

#endif