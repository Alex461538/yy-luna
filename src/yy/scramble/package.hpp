#if !defined(__hpp_yy_package__)
#define __hpp_yy_package__

//  --- Global includes ---
#include <optional>
#include <variant>
#include <filesystem>
//  --- Project includes ---
#include <semver.hpp>
#include <json.hpp>
//  --- Local includes ---
#include "../out.hpp"
#include "textfile.hpp"

using json = nlohmann::json;

namespace YY
{
    typedef semver::version<int, int, int> Version;
    typedef semver::range_set<int, int, int> VersionRange;
    typedef std::variant<Version, VersionRange> VersionSelector;

    struct PackageEntry {
        std::string name;
        VersionSelector versionSelector;
    };

    struct Package {
        std::string name;
        std::string description;
        std::string author;

        std::filesystem::path dir;
        std::filesystem::path main;

        semver::version<int> version;

        std::vector<std::string> keywords;
        std::vector<PackageEntry>packages;

        bool fromPath(std::filesystem::path path);
        bool loadFiles();
    };
}

#endif