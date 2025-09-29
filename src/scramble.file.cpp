#include "scramble.hpp"

namespace YY
{
    namespace Scramble
    {
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
                case Token::Kind::T_OPEN_PAREN: {
                    brackets.push_back({'(', tokens.size() - 1});
                } break;
                case Token::Kind::T_OPEN_CURLY: {
                    brackets.push_back({'{', tokens.size() - 1});
                } break;
                case Token::Kind::T_OPEN_SQUARE: {
                    brackets.push_back({'[', tokens.size() - 1});
                } break;
                case Token::Kind::T_CLOSE_PAREN: { 
                    auto pair = brackets[ brackets.size() - 1 ];
                    tokens[ std::get<1>(pair) ].enclosing = tokens.size() - 1;
                    if ( std::get<0>(pair) != '(' )
                    {
                        panic(Problem::problemOf<Problem::Type::ERR_STRAY_PAREN>(path.c_str(), {token.location.column, token.location.line}));
                    }
                    brackets.pop_back();
                } break;
                case Token::Kind::T_CLOSE_CURLY: {
                    auto pair = brackets[ brackets.size() - 1 ];
                    tokens[ std::get<1>(pair) ].enclosing = tokens.size() - 1;
                    if ( std::get<0>(pair) != '{' )
                    {
                        panic(Problem::problemOf<Problem::Type::ERR_STRAY_PAREN>(path.c_str(), {token.location.column, token.location.line}));
                    }
                    brackets.pop_back();
                } break;
                case Token::Kind::T_CLOSE_SQUARE: {
                    auto pair = brackets[ brackets.size() - 1 ];
                    tokens[ std::get<1>(pair) ].enclosing = tokens.size() - 1;
                    if ( std::get<0>(pair) != '[' )
                    {
                        panic(Problem::problemOf<Problem::Type::ERR_STRAY_PAREN>(path.c_str(), {token.location.column, token.location.line}));
                    }
                    brackets.pop_back();
                } break;
                case Token::Kind::T_INVALID: {
                    panic(Problem::problemOf<Problem::Type::ERR_INVALID_TOKEN>(path.c_str(), {token.location.column, token.location.line}));
                } break;
                default:
                    break;
                }

                token = lexer.next();
            }

            tokens.push_back(token);
        }

        void File::panic(Problem::Problem problem)
        {
            problems.push_back(problem);
        }

        void File::loadFromPath(const std::filesystem::path &path)
        {
            this->path = path;
            relative_path = std::filesystem::relative(path, owner_project->root_path);

            std::stringstream buffer = std::stringstream();
            std::ifstream file( path, std::ios::binary);

            if (!file.fail() && !file.bad())
            {
                buffer << file.rdbuf();
                content = buffer.str();
            }
            else
            {
                panic(Problem::problemOf<Problem::Type::ERR_FILE_NOT_FOUND>(path.c_str(), "This file does not exist or is not readable."));
            }

            lex();
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
                aprobs.push_back(std::string(p));
            }

            for (auto &t : tokens)
            {
                atoks.push_back(std::string(t));
            }

            return data;
        }
    }
}