#include "global.hpp"

namespace YY
{
    namespace Global
    {
        struct PackageEntry
        {
            std::string name;
            std::filesystem::path path;
            Version version;
        };

        std::optional<std::filesystem::path> resolvePackage(std::string name, VersionSelector version_selector)
        {
            std::vector<PackageEntry> packages;

            /* Open the global yyconf (dry on this parsing later) */
            json yyconf = {};
            std::ifstream file((std::filesystem::path(YY::Constants::Paths::HOME) / "yyconf.json").c_str());
            /* Does this file exist? */
            if (file.fail())
            {
                return {};
            }
            else
            {
                try
                {
                    /* Was this valid JSON? */
                    yyconf = json::parse(file);
                }
                catch (const std::exception &e)
                {
                    return {};
                }
            }
            /* Release the file */
            file.close();
            /* Parse */
            if (yyconf.is_object())
            {
                /* Does it had the packages list? */
                if (
                    yyconf.contains("packages") && yyconf["packages"].is_array()
                )
                {
                    for (auto &pkg : yyconf["packages"])
                    {
                        PackageEntry p;

                        if (
                            pkg.contains("name") && 
                            pkg.contains("path") && 
                            pkg.contains("version") && 
                            pkg["name"].is_string() &&
                            pkg["path"].is_string() &&
                            pkg["version"].is_string()
                        )
                        {
                            p.name = pkg["name"].get<std::string>();
                            p.path = pkg["path"].get<std::string>();
                            if ( semver::parse( pkg["version"].get<std::string>(), p.version) )
                            {
                                packages.push_back(p);
                            }
                        }
                    }
                }
            }

            /* Search a package using a range and pick the newest */
            if ( std::holds_alternative< VersionRange >(version_selector) )
            {
                auto &range = std::get<VersionRange>(version_selector);
                /* Initialize a null candidate */
                PackageEntry *p = nullptr;
                for (auto pkg : packages)
                {
                    /* There is a matching package, and there is none selected or the new version is greater? */
                    if ( range.contains(pkg.version) && name == pkg.name && ( p == nullptr || pkg.version > p->version ) )
                    {
                        p = &pkg;
                    }
                }
                /* Was a match found? */
                if ( p != nullptr )
                {
                    return YY::Constants::Paths::HOME / p->path;
                }
            }
            /* Search a package by exact version */
            else if ( std::holds_alternative< Version >(version_selector) )
            {
                auto &version = std::get<Version>(version_selector);
                for (auto pkg : packages)
                {
                    if ( name == pkg.name && pkg.version == version )
                    {
                        return YY::Constants::Paths::HOME / pkg.path;
                    }
                }
            }

            return {};
        }
    }
}