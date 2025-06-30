#include <iostream>

#include "glin/cli.hpp"

/*
    Nunca olvide papi, debug: shift+f5, run normal: ctrl+shift+f5
*/

// #define _TEST_

int main(int argc, char** argv){
    #ifdef _TEST_
        char *test_argv[] = {"C:/Users/alexg/AppData/Local/glin/out/build/vcpkg/Debug/glin.exe", "build", "C:/Users/alexg/AppData/Local/glin/samples/test/main.lyn"};
        int test_argc = sizeof(test_argv) / sizeof(char*);
        return CLI::exec(test_argc, test_argv);
    #else
        return CLI::exec(argc, argv);
    #endif
}