#if !defined(__hpp_yy_config__)
#define __hpp_yy_config__

#include <string>
#include <format>
#include <fstream>

#include <json.hpp>
#include <semver.hpp>

#include "problem.hpp"
#include "info.hpp"

using json = nlohmann::json;

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

        std::string resolveGlobalPackage(ImportQuery &query);
    }
}

#endif