#if !defined(__hpp_yy_global__)
#define __hpp_yy_global__

//  --- Global includes ---
#include <optional>
#include <variant>
#include <filesystem>
#include <fstream>
//  --- Project includes ---
#include <semver.hpp>
#include <json.hpp>
//  --- Local includes ---
#include "constants.hpp"

using json = nlohmann::json;

namespace YY
{
    namespace Global
    {
        typedef semver::version<int, int, int> Version;
        typedef semver::range_set<int, int, int> VersionRange;
        typedef std::variant<Version, VersionRange> VersionSelector;

        std::optional<std::filesystem::path> resolvePackage(std::string name, VersionSelector version_selector);
    }
}

#endif