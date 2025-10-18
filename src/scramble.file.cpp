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
                        panic(Problem::FileProblem::get(Problem::Type::ERR_STRAY_PAREN, ") expected", path.c_str(), {token.location.line, token.location.column}));
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
                        panic(Problem::FileProblem::get(Problem::Type::ERR_STRAY_PAREN, "} expected", path.c_str(), {token.location.line, token.location.column}));
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
                        panic(Problem::FileProblem::get(Problem::Type::ERR_STRAY_PAREN, "] expected", path.c_str(), {token.location.line, token.location.column}));
                    }
                    brackets.pop_back();
                }
                break;
                case Token::Kind::T_INVALID:
                {
                    panic(Problem::FileProblem::get(Problem::Type::ERR_INVALID_TOKEN, "Invalid token at lexing: " + std::string(token.text, token.length), path.c_str(), {token.location.line, token.location.column}));
                }
                break;
                default:
                    break;
                }

                token = lexer.next();
            }

            tokens.push_back(token);
        }

        void File::loadFromPath(const std::filesystem::path &path)
        {
            this->path = path;
            relative_path = std::filesystem::relative(path, owner_project->root_path);

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

            data["path"] = path.c_str();
            data["relative_path"] = relative_path.c_str();

            data["problems"] = json::array();
            data["tokens"] = json::array();

            auto &aprobs = data.at("problems");
            auto &atoks = data.at("tokens");

            for (auto &p : problems)
            {
                aprobs.push_back(std::string(*p));
            }

            for (auto &t : tokens)
            {
                atoks.push_back(std::string(t));
            }

            return data;
        }

        void File::preprocess()
        {
            for (int index = 0; index < tokens.size();)
            {
                Token::Token &token = tokens[index++];

                // Verbose output for debugging
                // std::printf("Processing token: %s\n", std::string(token).c_str());

                // handle import statements
                if (token.kind == Token::Kind::K_IMPORT)
                {
                    Token::Token symbolName;
                    Token::Token tk_from_as_comma;
                    Token::Token aliasName;
                    Token::Token pathToken;

                    std::vector<std::pair<std::string, std::string>> importSymbols; // symbol, alias

                    // Consume if at least there can be a import <symbol> from "path"
                    while (index + 3 < tokens.size())
                    {
                        // Peek a symbol
                        symbolName = tokens[index++];
                        // Check for identifier
                        if (symbolName.kind != Token::Kind::T_IDENTIFIER)
                        {
                            index--; // Prevent phantom consumption
                            panic(Problem::FileProblem::get(Problem::Type::ERR_EXPECTED, "<identifier> expected", path.c_str(), {token.location.line, token.location.column}));
                            break;
                        }

                        // Peek for as | from
                        tk_from_as_comma = tokens[index++];

                        bool hasAlias = false;

                        // Check for an alias
                        if (tk_from_as_comma.kind == Token::Kind::K_AS)
                        {
                            hasAlias = true;
                            // Peek alias
                            aliasName = tokens[index++];
                            // check for identifier
                            if (aliasName.kind != Token::Kind::T_IDENTIFIER)
                            {
                                index--; // Prevent phantom consumption
                                panic(Problem::FileProblem::get(Problem::Type::ERR_EXPECTED, "<identifier> expected", path.c_str(), {token.location.line, token.location.column}));
                                break;
                            }
                            // Peek for from | ,
                            tk_from_as_comma = tokens[index++];
                        }

                        // Store the symbol (with alias if any)
                        if (hasAlias)
                        {
                            importSymbols.push_back({std::string(symbolName.text, symbolName.length), std::string(aliasName.text, aliasName.length)});
                        }
                        else
                        {
                            importSymbols.push_back({std::string(symbolName.text, symbolName.length), ""});
                        }

                        // Check for , | from
                        if (tk_from_as_comma.kind == Token::Kind::T_COMMA)
                        {
                            // continue to next symbol
                            continue;
                        }
                        else if (tk_from_as_comma.kind == Token::Kind::K_FROM)
                        {
                            pathToken = tokens[index++];

                            if (pathToken.kind == Token::Kind::C_STRING)
                            {
                                std::string importPath(pathToken.text, pathToken.length);
                                // Remove quotes
                                if (importPath.size() >= 2 && importPath.front() == '"' && importPath.back() == '"')
                                {
                                    importPath = importPath.substr(1, importPath.size() - 2);
                                }

                                std::printf("Importing from file: %s\n", importPath.c_str());

                                for (auto &sympair : importSymbols)
                                {
                                    std::printf("  symbol: %s", sympair.first.c_str());
                                    if (!sympair.second.empty())
                                    {
                                        std::printf(" as %s", sympair.second.c_str());
                                    }
                                    std::printf("\n");
                                }
                            }
                            else
                            {
                                index--; // Prevent phantom consumption
                                panic(Problem::FileProblem::get(Problem::Type::ERR_EXPECTED, "<string lit> expected", path.c_str(), {token.location.line, token.location.column}));
                                continue;
                            }

                            break;
                        }
                        else
                        {
                            index--; // Prevent phantom consumption
                            panic(Problem::FileProblem::get(Problem::Type::ERR_EXPECTED, ", | <from> expected", path.c_str(), {token.location.line, token.location.column}));
                            break;
                        }
                    }
                }
            }
        }

        void File::flatten()
        {
            //
        }
    }
}