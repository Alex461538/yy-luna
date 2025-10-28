#include "scramble.hpp"

namespace YY
{
    namespace Scramble
    {
        void File::panic(std::shared_ptr<Problem::Problem> problem)
        {
            problems.push_back(problem);
        }

        void File::lex()
        {
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
                        panic(Problem::FileProblem::get(Problem::Type::ERR_STRAY_PAREN, ") expected", meta.path.c_str(), {token.location.line, token.location.column}));
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
                        panic(Problem::FileProblem::get(Problem::Type::ERR_STRAY_PAREN, "} expected", meta.path.c_str(), {token.location.line, token.location.column}));
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
                        panic(Problem::FileProblem::get(Problem::Type::ERR_STRAY_PAREN, "] expected", meta.path.c_str(), {token.location.line, token.location.column}));
                    }
                    brackets.pop_back();
                }
                break;
                case Token::Kind::T_INVALID:
                {
                    panic(Problem::FileProblem::get(Problem::Type::ERR_INVALID_TOKEN, "Invalid token at lexing: " + std::string(token.text, token.length), meta.path.c_str(), {token.location.line, token.location.column}));
                }
                break;
                default:
                    break;
                }

                token = lexer.next();
            }

            tokens.push_back(token);
        }

        void File::fromPath(const std::filesystem::path &path)
        {
            this->meta.path = path;
            meta.relative_path = std::filesystem::relative(path, owner_project->meta.root_path);

            std::stringstream buffer = std::stringstream();
            std::ifstream file(path, std::ios::binary);

            if (!file.fail() && !file.bad())
            {
                buffer << file.rdbuf();
                content = buffer.str();
            }
            else
            {
                panic(Problem::WPProblem::get(Problem::Type::ERR_FILE_NOT_FOUND, "This file does not exist or is not readable", path.c_str()));
                return;
            }

            lex();

            preprocess();

            flatten();
        }

        File::operator json() const
        {
            json data = json::object();

            data["path"] = meta.path.c_str();
            data["relative_path"] = meta.relative_path.c_str();

            data["problems"] = json::array();
            data["deps"] = json::array();
            // data["tokens"] = json::array();

            auto &aprobs = data.at("problems");
            auto &adeps = data.at("deps");
            // auto &atoks = data.at("tokens");

            for (auto &p : problems)
            {
                aprobs.push_back(std::string(*p));
            }

            for (auto &d : dependencies)
            {
                adeps.push_back(std::string(d));
            }

            /* for (auto &t : tokens)
            {
                atoks.push_back(std::string(t));
            } */

            return data;
        }

        ImportDependency::ImportDependency(){}

        ImportDependency::ImportDependency(std::string _path)
        {
            std::string::size_type version_sepr = _path.find(':');
            path = _path.substr(0, version_sepr);
            if (version_sepr != std::string::npos)
            {
                version = _path.substr(version_sepr + 1);
            }
        }

        ImportDependency::operator std::string() const
        {
            // This WILL cause a lot of problems
            std::string abs_path;
            if (type == ImportType::IPT_FILE)
            {
                abs_path = ((File*)target.get())->meta.path;
            }
            else if (type == ImportType::IPT_PROJ)
            {
                abs_path = ((Project*)target.get())->meta.root_path;
            }
            return std::format("{} <{}> {} {}", path, version.empty() ? "?" : version, std::to_string((int)type), abs_path);
        }

        size_t File::addDependency(std::string query)
        {
            // Shoul i care too much of duplicate deps here?
            // Oh, FIX DUP NAMESPACES FOR NOW
            // PaThS LaTeR
            dependencies.push_back(ImportDependency(query));
            return dependencies.size() - 1;
        }

        void File::importNamespace(size_t dependency, std::string name, std::string alias = "")
        {
            if (!alias.empty())
            {
                importNamespaces[alias] = {name, dependency};
            }
            else
            {
                importNamespaces[name] = {name, dependency};
            }
        }

        void File::preprocess()
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
                            panic(Problem::FileProblem::get(Problem::Type::ERR_EXPECTED, "<identifier> expected", meta.path.c_str(), {token->location.line, token->location.column}));
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
                                panic(Problem::FileProblem::get(Problem::Type::ERR_EXPECTED, "<identifier> expected", meta.path.c_str(), {token->location.line, token->location.column}));
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
                                            ""});
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

                                size_t dependency = addDependency(importPath);

                                for (auto &sympair : deps)
                                {
                                    importNamespace(dependency, sympair.first, sympair.second);
                                }
                            }
                            else
                            {
                                index--; // Prevent phantom consumption
                                panic(Problem::FileProblem::get(Problem::Type::ERR_EXPECTED, "<string lit> expected", meta.path.c_str(), {token->location.line, token->location.column}));
                                continue;
                            }

                            break;
                        }
                        else
                        {
                            index--; // Prevent phantom consumption
                            panic(Problem::FileProblem::get(Problem::Type::ERR_EXPECTED, ", | <from> expected", meta.path.c_str(), {token->location.line, token->location.column}));
                            break;
                        }
                    }
                }
            }
        }

        void ImportDependency::attachFile(std::shared_ptr<File> target) 
        {
            //
            type = ImportType::IPT_FILE;
            this->target = target;
        }

        void ImportDependency::attachProject(std::shared_ptr<Project> target) 
        {
            //
            type = ImportType::IPT_PROJ;
            this->target = target;
        }

        void File::resolve(size_t dependency_index)
        {
            auto &dependency = dependencies[dependency_index];

            if (owner_project != nullptr)
            {
                owner_project->resolveFromFile(&dependency, this);
            }
        }

        void File::flatten()
        {
            for (int i = 0; i < dependencies.size(); i++)
            {
                resolve(i);
            }
        }
    }
}