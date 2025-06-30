#include "glin/cli.hpp"

#include <iostream>
#include <fstream>

#include <argh.h>

#include "glin/paths.hpp"
#include "glin/lexer.hpp"
#include "glin/token.hpp"

namespace CLI
{
    void printBanner()
    {
        std::cout <<"     ..  ##     \n" \
                    "  ...      ###  \n" \
                    " ...        ### \n" \
                    "...          ###\n" \
                    ":::          +++\n" \
                    " ::-        -++ \n" \
                    "  ---      ---  \n" \
                    "     --  --     \n";
        std::cout << "Thanks for using the luna compiler - Your most adrade resource for making simple things." << std::endl;
        std::cout << "v0.0: If you have an interesting idea for improving this, or want to see other projects check out these links!" << std::endl;
        std::cout << " - GitHub: https://github.com/Alex461538/glin" << std::endl;
        std::cout << " - Yt: https://youtube.com/@pianitas38 " << std::endl;
    }

    void printSyntaxError(std::string message)
    {
        std::cout << message + "\nTry glin help for more details.";
    }

    int exec(int argc, char** argv) {
        std::string GLIN_HOME = Paths::getGlinHome();

        if (!GLIN_HOME.length())
        {
            std::cout << "GLIN_HOME env variable not set.\n";
            return 1;
        }

        auto args = argh::parser(argc, argv);

        const std::string action = args(1, "help").str();

        if (action == "help")
        {
            printBanner();
        }
        else if (action == "build")
        {
            std::string path = args[2];

            if (!path.length()) 
            {
                printSyntaxError("Please provide a path.");
                return 1;
            }

            std::stringstream buffer;
            std::ifstream file(Paths::getTrueCallerPath(path), std::ios::binary);
            if (!file.fail())
            {
                buffer << file.rdbuf();
                file.close();

                std::string content = buffer.str();

                Lexer::Lexer lexer = Lexer::Lexer();
                lexer.content = content.data();
                lexer.length = content.size();
                Token::Token token = lexer.next();

                std::cout << content;
                
                while (token.kind != Token::Kind::T_EOF)
                {
                    std::cout << "Token: " << (std::string)token << std::endl;
                    token = lexer.next();
                }
            }
        }
        else if (action == "doc")
        {
            std::string path = args[2];
            if (!path.length())
            {
                printSyntaxError("Please provide a path.");
                return 1;
            }
        }
        else
        {
            std::cout << "Invalid action provided.\n";
            return 1;
        }

        return 0;
    }
}