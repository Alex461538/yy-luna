#include <iostream>
#include <string>

#include "yyl.hpp"

int main( int argc, const char **argv )
{
    YY::init();

    json args = YY::CLI::parseArgs(argc, argv);

    std::filesystem::path source = args["source"];

    if (source.empty() || !source.is_absolute())
    {
        return 1;
    }

    YY::Scramble::Workspace workspace = YY::Scramble::fromPath(source);

    std::cout << ((json)workspace).dump(4) << std::endl;

    return 0;
}