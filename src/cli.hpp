#if !defined(__hpp_yy_cli__)
#define __hpp_yy_cli__

#include <filesystem>

#include <argh.h>
#include <json.hpp>

#include "info.hpp"

using json = nlohmann::json;

/*

// Default pipeline, build exe for current target
yy-luna -s samples/test

./build/yy-luna -s samples/test -e obj -os linux -arch x64 -vendor pc

*/

namespace YY {
    namespace CLI {
        json parseArgs(int argc, const char **argv);
    }
}

#endif