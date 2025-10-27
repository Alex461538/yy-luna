#include <iostream>
#include <string>

#include "yyl.hpp"

int main( int argc, const char **argv )
{
    YY::init();

    json args = YY::CLI::parseArgs(argc, argv);

    std::filesystem::path source = args["source"];

    if (source.empty())
    {
        return 1;
    }

    // Load a filesystem workspace repr from a path
    YY::Scramble::Workspace fs_workspace = YY::Scramble::Transformers::fromPath(source);
    // Create a more specific repr with trees
    YY::Interp::Workspace ast_workspace = YY::Interp::Transformers::fromScramble(fs_workspace);

    // Debug of scramble
    std::cout << ((json)fs_workspace).dump(4) << std::endl;

    return 0;
}