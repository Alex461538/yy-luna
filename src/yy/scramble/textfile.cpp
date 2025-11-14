#include "textfile.hpp"

namespace YY
{
    bool TextFile::fromPath(std::filesystem::path path)
    {
        if (!path.is_absolute())
        {
            return false;
        }

        name = path.filename();
        dir = path.parent_path();

        std::stringstream buffer = std::stringstream();
        std::ifstream file(path, std::ios::binary);

        if (!file.fail() && !file.bad())
        {
            buffer << file.rdbuf();
            content = buffer.str();
        }
        else
        {
            return false;
        }

        return true;
    }

    bool TextFile::setContent(std::string new_content)
    {
        content = new_content;
        return true;
    }

    bool TextFile::lex()
    {
        bool successful = true;

        tokens.clear();

        std::vector<std::tuple<char, size_t>> brackets;

        Lexer::Lexer lexer = Lexer::Lexer();

        lexer.content = content.data();
        lexer.length = content.size();

        Token::Token token = lexer.next();

        while (token.kind != Token::Kind::T_EOF)
        {
            tokens.push_back(token);

            switch (token.kind)
            {
            case Token::Kind::T_OPEN_PAREN:
            {
                brackets.push_back({'(', tokens.size() - 1});
            }
            break;
            case Token::Kind::T_OPEN_CURLY:
            {
                brackets.push_back({'{', tokens.size() - 1});
            }
            break;
            case Token::Kind::T_OPEN_SQUARE:
            {
                brackets.push_back({'[', tokens.size() - 1});
            }
            break;
            case Token::Kind::T_CLOSE_PAREN:
            {
                auto pair = brackets[brackets.size() - 1];
                tokens[std::get<1>(pair)].enclosing = tokens.size() - 1;
                if (std::get<0>(pair) != '(')
                {
                    // panic(Problem::FileProblem::get(Problem::Type::ERR_STRAY_PAREN, ") expected", meta.path.c_str(), {token.location.line, token.location.column}));
                    successful = false;
                }
                brackets.pop_back();
            }
            break;
            case Token::Kind::T_CLOSE_CURLY:
            {
                auto pair = brackets[brackets.size() - 1];
                tokens[std::get<1>(pair)].enclosing = tokens.size() - 1;
                if (std::get<0>(pair) != '{')
                {
                    // panic(Problem::FileProblem::get(Problem::Type::ERR_STRAY_PAREN, "} expected", meta.path.c_str(), {token.location.line, token.location.column}));
                    successful = false;
                }
                brackets.pop_back();
            }
            break;
            case Token::Kind::T_CLOSE_SQUARE:
            {
                auto pair = brackets[brackets.size() - 1];
                tokens[std::get<1>(pair)].enclosing = tokens.size() - 1;
                if (std::get<0>(pair) != '[')
                {
                    // panic(Problem::FileProblem::get(Problem::Type::ERR_STRAY_PAREN, "] expected", meta.path.c_str(), {token.location.line, token.location.column}));
                    successful = false;
                }
                brackets.pop_back();
            }
            break;
            case Token::Kind::T_INVALID:
            {
                // panic(Problem::FileProblem::get(Problem::Type::ERR_INVALID_TOKEN, "Invalid token at lexing: " + std::string(token.text, token.length), meta.path.c_str(), {token.location.line, token.location.column}));
                successful = false;
            }
            break;
            default:
                break;
            }

            token = lexer.next();
        }

        tokens.push_back(token);

        return successful;
    }

    TextFile::ImportEntry::ImportEntry(){}
    TextFile::ImportEntry::ImportEntry( std::string query )
    {
        semver::version qr_v;
        semver::range_set qr_r;

        std::string::size_type version_sepr = query.find(':');

        name = query.substr(0, version_sepr);

        if (version_sepr != std::string::npos)
        {
            std::string qr_version = query.substr(version_sepr + 1);

            if (semver::parse(qr_version, qr_v))
            {
                versionSelector = qr_v;
            }
            else if (semver::parse(qr_version, qr_r))
            {
                versionSelector = qr_r;
            }
            else
            {
                // panic please
            }
        }
    }

    void TextFile::preprocess()
    {
        for (int index = 0; index < tokens.size();)
        {
            Token::Token *token = &tokens[index++];

            // Verbose output for debugging
            // std::printf("Processing token: %s\n", std::string(token).c_str());

            // handle import statements
            if (token->kind == Token::Kind::K_IMPORT)
            {
                // mark import
                token->ignore = true;

                Token::Token *symbolName = nullptr;
                Token::Token *tk_from_as_comma = nullptr;
                Token::Token *aliasName = nullptr;
                Token::Token *pathToken = nullptr;

                std::vector<std::pair<std::string, std::string>> deps;

                // Consume if at least there can be a import <symbol> from "path"
                while (index + 3 < tokens.size())
                {
                    // Peek a symbol
                    symbolName = &tokens[index++];
                    // Check for identifier
                    if (symbolName->kind != Token::Kind::T_IDENTIFIER)
                    {
                        index--; // Prevent phantom consumption
                        //panic(Problem::FileProblem::get(Problem::Type::ERR_EXPECTED, "<identifier> expected", meta.path.c_str(), {token->location.line, token->location.column}));
                        break;
                    }

                    // mark symbol name
                    symbolName->ignore = true;

                    // Peek for as | from
                    tk_from_as_comma = &tokens[index++];

                    bool hasAlias = false;

                    // Check for an alias
                    if (tk_from_as_comma->kind == Token::Kind::K_AS)
                    {
                        // mark as kw
                        tk_from_as_comma->ignore = true;

                        hasAlias = true;
                        // Peek alias
                        aliasName = &tokens[index++];
                        // check for identifier
                        if (aliasName->kind != Token::Kind::T_IDENTIFIER)
                        {
                            index--; // Prevent phantom consumption
                            //panic(Problem::FileProblem::get(Problem::Type::ERR_EXPECTED, "<identifier> expected", meta.path.c_str(), {token->location.line, token->location.column}));
                            break;
                        }

                        // mark alias name
                        aliasName->ignore = true;

                        // Peek for from | ,
                        tk_from_as_comma = &tokens[index++];
                    }

                    // Store the symbol (with alias if any)
                    if (hasAlias)
                    {
                        deps.push_back({std::string(symbolName->text, symbolName->length),
                                        std::string(aliasName->text, aliasName->length)});
                    }
                    else
                    {
                        deps.push_back({std::string(symbolName->text, symbolName->length),
                                        std::string(symbolName->text, symbolName->length)});
                    }

                    // Check for , | from
                    if (tk_from_as_comma->kind == Token::Kind::T_COMMA)
                    {
                        // mark comma
                        tk_from_as_comma->ignore = true;

                        // continue to next symbol
                        continue;
                    }
                    else if (tk_from_as_comma->kind == Token::Kind::K_FROM)
                    {
                        // mark from
                        tk_from_as_comma->ignore = true;

                        pathToken = &tokens[index++];

                        if (pathToken->kind == Token::Kind::C_STRING)
                        {
                            // mark path
                            pathToken->ignore = true;

                            std::string importPath(pathToken->text, pathToken->length);
                            // Remove quotes
                            if (importPath.size() >= 2 && importPath.front() == '"' && importPath.back() == '"')
                            {
                                importPath = importPath.substr(1, importPath.size() - 2);
                            }

                            // std::printf("Importing from file: %s\n", importPath.c_str());

                            TextFile::ImportEntry entry( importPath );

                            for (auto &sympair : deps)
                            {
                                entry.namespaces[ sympair.second ] = sympair.first;
                            }

                            imports.push_back(entry);
                        }
                        else
                        {
                            index--; // Prevent phantom consumption
                            //panic(Problem::FileProblem::get(Problem::Type::ERR_EXPECTED, "<string lit> expected", meta.path.c_str(), {token->location.line, token->location.column}));
                            continue;
                        }

                        break;
                    }
                    else
                    {
                        index--; // Prevent phantom consumption
                        //panic(Problem::FileProblem::get(Problem::Type::ERR_EXPECTED, ", | <from> expected", meta.path.c_str(), {token->location.line, token->location.column}));
                        break;
                    }
                }
            }
        }
    }
}