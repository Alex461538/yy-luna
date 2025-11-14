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
#include "../global.hpp"
#include "../out.hpp"
#include "textfile.hpp"

using json = nlohmann::json;

namespace YY
{
    struct Package
    {
        struct PackageEntry
        {
            std::string name;
            Global::VersionSelector versionSelector;
            std::optional<std::shared_ptr<Package>> reference;
        };

        std::map<std::string, std::shared_ptr<YY::TextFile>> files;

        std::string name;
        std::string description;
        std::string author;

        std::filesystem::path dir;
        std::filesystem::path main;

        semver::version<int> version;

        std::vector<std::string> keywords;
        std::vector<Package::PackageEntry> packages;

        bool fromPath(std::filesystem::path path);
        std::optional<std::shared_ptr<YY::TextFile>> findFile(std::filesystem::path path);
        std::optional<std::shared_ptr<YY::TextFile>> addFile(std::filesystem::path path);

        std::optional<std::shared_ptr<Package>> resolvePackage(std::string name, std::optional<Global::VersionSelector> version_selector);
    };
}

#endif