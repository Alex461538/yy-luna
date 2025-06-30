#ifndef _GLIN_CLI_HPP_
#define _GLIN_CLI_HPP_

#include <string>

namespace CLI
{
    void printBanner();
    void printSyntaxError(std::string message);
    int exec(int argc, char** argv);
}

#endif