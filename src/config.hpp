#if !defined(__hpp_yy_config__)
#define __hpp_yy_config__

#include <string>
#include <format>

namespace YY
{
    namespace Config
    {
        struct PackageEntry {
            std::string name;
            std::string version_matcher;

            operator std::string() const;
        };

        struct ImportQuery {
            std::string name;
            std::string version;

            std::string path;

            void attachFile(std::string path);
            void attachProject(std::string path);

            ImportQuery(std::string &_query);
        };

        std::string resolveGlobalPackage(PackageEntry query);
    }
}

#endif