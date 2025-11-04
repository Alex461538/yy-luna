#include "yy/yy.hpp"

int main(int argc, const char **argv)
{
    YY::CLI::Command command;
    // Init all required constants
    YY::Constants::init();
    // Clear logfile output
    YY::Debug::clear();
    // Parse command from cmdl
    command.parse(argv, argc);
    // Execute parsed command
    command.execute();
    return 0;
}