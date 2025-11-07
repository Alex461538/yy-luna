#include "package.hpp"

namespace YY
{
    bool Package::fromPath(std::filesystem::path path)
    {
        if (!path.is_absolute())
        {
            return false;
        }

        dir = path;

        json yyconf = {};
        std::ifstream file((std::filesystem::path(path) / "yyconf.json").c_str());

        if (file.fail())
        {
            return false;
        }
        else
        {
            try
            {
                yyconf = json::parse(file);
            }
            catch (const std::exception &e)
            {
                return false;
            }
        }

        file.close();

        if (yyconf.is_object())
        {
            if (
                yyconf.contains("name") &&
                yyconf.contains("main") &&
                yyconf.contains("version") && 
                yyconf["version"].is_string() &&
                yyconf["name"].is_string() &&
                yyconf["main"].is_string())
            {
                name = yyconf["name"].get<std::string>();

                main = yyconf["main"].get<std::string>();
                if (main.is_absolute())
                {
                    return false;
                }
                main = path / main;
                if (!std::filesystem::exists(main))
                {
                    return false;
                }
                if (!semver::parse(yyconf["version"].get<std::string>(), version))
                {
                    return false;
                }
            }
            else
            {
                return false;
            }

            if (yyconf.contains("author") && yyconf["author"].is_string())
            {
                author = yyconf["author"].get<std::string>();
            }
            if (yyconf.contains("description") && yyconf["description"].is_string())
            {
                description = yyconf["description"].get<std::string>();
            }

            if (yyconf.contains("keywords") && yyconf["keywords"].is_array())
            {
                for (const auto &keyword : yyconf["keywords"])
                {
                    if (keyword.is_string())
                    {
                        keywords.push_back(keyword.get<std::string>());
                    }
                }
            }

            if (yyconf.contains("packages") && yyconf["packages"].is_array())
            {
                for (const auto &package : yyconf["packages"])
                {
                    if (package.is_object())
                    {
                        YY::PackageEntry pkg;

                        if (package.contains("name") and package["name"].is_string())
                        {
                            pkg.name = package["name"].get<std::string>();
                        }

                        if (package.contains("version") and package["version"].is_string())
                        {
                            std::string v = package["version"].get<std::string>();
                            semver::version sm_v;
                            semver::range_set sm_rgs;
                            if (semver::parse(v, sm_v)) 
                            {
                                pkg.versionSelector = sm_v;
                            }
                            else if (semver::parse(v, sm_rgs))
                            {
                                pkg.versionSelector = sm_rgs;
                            }
                            else
                            {
                                return false;
                            }
                        }

                        packages.push_back(pkg);
                    }
                }
            }
        }
        else
        {
            return false;
        }

        return true;
    }

    bool Package::loadFiles()
    {
        //
    }
}
