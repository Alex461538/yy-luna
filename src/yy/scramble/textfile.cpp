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
                    //panic(Problem::FileProblem::get(Problem::Type::ERR_STRAY_PAREN, ") expected", meta.path.c_str(), {token.location.line, token.location.column}));
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
                    //panic(Problem::FileProblem::get(Problem::Type::ERR_STRAY_PAREN, "} expected", meta.path.c_str(), {token.location.line, token.location.column}));
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
                    //panic(Problem::FileProblem::get(Problem::Type::ERR_STRAY_PAREN, "] expected", meta.path.c_str(), {token.location.line, token.location.column}));
                    successful = false;
                }
                brackets.pop_back();
            }
            break;
            case Token::Kind::T_INVALID:
            {
                //panic(Problem::FileProblem::get(Problem::Type::ERR_INVALID_TOKEN, "Invalid token at lexing: " + std::string(token.text, token.length), meta.path.c_str(), {token.location.line, token.location.column}));
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
}