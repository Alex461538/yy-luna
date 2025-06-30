#ifndef _GLIN_PATHS_HPP_
#define _GLIN_PATHS_HPP_

#include <string>

namespace Paths {
    std::string getGlinHome();
    std::string getTrueCallerPath(std::string directory);
}

#endif