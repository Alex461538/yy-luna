#if !defined(__hpp_yy_config__)
#define __hpp_yy_config__

#include <string>

namespace YY
{
    namespace Config
    {
        std::string resolveGlobalPackage(PackageEntry query)
        {
            //
            return "";
        }

        struct PackageEntry {
            std::string name;
            std::string version_matcher;
        };
    }
}

#endif