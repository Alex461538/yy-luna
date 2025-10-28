#include "config.hpp"

namespace YY
{
    namespace Config
    {
        std::string resolveGlobalPackage(ImportQuery &dependency)
        {
            semver::version dep_version;
            bool valid_dep_version = semver::parse(dependency.version, dep_version);

            json yyconf = {};
            std::ifstream file((std::filesystem::path(YY::HOME_PATH) / "yyconf.json").c_str());

            if (file.fail())
            {
                return "";
            }
            else
            {
                try
                {
                    yyconf = json::parse(file);
                }
                catch (const std::exception &e)
                {
                    return "";
                }
            }

            file.close();

            if (yyconf.is_object())
            {
                if (yyconf.contains("packages") && yyconf["packages"].is_array())
                {
                    for (auto &pkg : yyconf["packages"])
                    {
                        std::string name, version, path;

                        if (pkg.contains("name") && pkg["name"].is_string())
                        {
                            name = pkg["name"].get<std::string>();
                        }
                        if (pkg.contains("version") && pkg["version"].is_string())
                        {
                            version = pkg["version"].get<std::string>();
                        }
                        if (pkg.contains("path") && pkg["path"].is_string())
                        {
                            path = pkg["path"].get<std::string>();
                        }

                        semver::version pkg_version;
                        bool valid_pkg_version = semver::parse(version, pkg_version);

                        if (pkg_version == dep_version && name == dependency.name)
                        {
                            auto abs_path = std::filesystem::absolute( HOME_PATH / std::filesystem::path(path) );
                            
                            return abs_path;
                        }
                    }
                }
            }

            return "";
        }

        PackageEntry::operator std::string() const
        {
            return std::format("{} <{}>", name, version_matcher.empty() ? "?" : version_matcher);
        }

        ImportQuery::ImportQuery(std::string &_query)
        {
            version = "";
            name = "";

            std::string::size_type version_sepr = _query.find(':');
            name = _query.substr(0, version_sepr);

            if (version_sepr != std::string::npos)
            {
                version = _query.substr(version_sepr + 1);
            }
        }

        void ImportQuery::attachFile(std::string path)
        {
            this->path = path;
        }

        void ImportQuery::attachProject(std::string path)
        {
            this->path = path;
        }
    }
}