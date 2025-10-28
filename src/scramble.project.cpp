#include "scramble.hpp"

namespace YY
{
    namespace Scramble
    {
        void Project::panic(std::shared_ptr<Problem::Problem> problem)
        {
            problems.push_back(problem);
        }

        void Project::fromPath(const std::filesystem::path &path)
        {
            meta.root_path = path;

            json yyconf = {};
            std::ifstream file((std::filesystem::path(path) / "yyconf.json").c_str());

            if (file.fail())
            {
                panic(Problem::WPProblem::get(Problem::Type::ERR_PROJECT_NO_CONFIG, "Config file not found", path.c_str()));
            }
            else
            {
                try
                {
                    yyconf = json::parse(file);
                }
                catch (const std::exception &e)
                {
                    panic(Problem::WPProblem::get(Problem::Type::ERR_PROJECT_INVALID_CONFIG, e.what(), path.c_str()));
                }
            }

            file.close();

            if (!yyconf.is_null())
            {
                if (yyconf.contains("name") && yyconf["name"].is_string())
                {
                    meta.name = yyconf["name"].get<std::string>();
                }
                else
                {
                    meta.name = path.filename().string();
                    panic(Problem::WPProblem::get(Problem::Type::ERR_PROJECT_INVALID_CONFIG, "Project name is missing or not a string", path.c_str()));
                }

                if (yyconf.contains("main") && yyconf["main"].is_string())
                {
                    meta.main_path = yyconf["main"].get<std::string>();

                    std::filesystem::path main_path = path / yyconf["main"].get<std::string>();
                    if (!std::filesystem::exists(main_path))
                    {
                        main_path = "";
                        panic(Problem::WPProblem::get(Problem::Type::ERR_FILE_NOT_FOUND, "Main file specified in configuration does not exist", path.c_str()));
                    }
                }
                else
                {
                    panic(Problem::WPProblem::get(Problem::Type::ERR_PROJECT_INVALID_CONFIG, "Main file is missing or not a string", path.c_str()));
                }

                if (yyconf.contains("author") && yyconf["author"].is_string())
                {
                    meta.author = yyconf["author"].get<std::string>();
                }

                if (yyconf.contains("description") && yyconf["description"].is_string())
                {
                    meta.description = yyconf["description"].get<std::string>();
                }

                if (yyconf.contains("version") && yyconf["version"].is_string())
                {
                    meta.version = yyconf["version"].get<std::string>();
                }
                else
                {
                    meta.version = "0.0.1";
                    panic(Problem::WPProblem::get(Problem::Type::ERR_PROJECT_INVALID_CONFIG, "Version is missing or not a string", path.c_str()));
                }

                if (yyconf.contains("keywords") && yyconf["keywords"].is_array())
                {
                    for (const auto &keyword : yyconf["keywords"])
                    {
                        if (keyword.is_string())
                        {
                            meta.keywords.push_back(keyword.get<std::string>());
                        }
                    }
                }

                if (yyconf.contains("packages") && yyconf["packages"].is_array())
                {
                    for (const auto &package : yyconf["packages"])
                    {
                        if (package.is_object())
                        {
                            Config::PackageEntry pkg;

                            if (package.contains("name") and package["name"].is_string())
                            {
                                pkg.name = package["name"];
                            }

                            if (package.contains("version") and package["version"].is_string())
                            {
                                pkg.version_matcher = package["version"];
                            }

                            meta.packages.push_back(pkg);
                        }
                    }
                }

                if (!meta.main_path.empty())
                {
                    auto abs_path = std::filesystem::path(meta.main_path);

                    if (!abs_path.is_absolute())
                    {
                        abs_path = meta.root_path / meta.main_path;
                    }

                    main = addFile(abs_path.c_str());
                }
            }
            else
            {
                panic(Problem::WPProblem::get(Problem::Type::ERR_PROJECT_INVALID_CONFIG, "Configuration file is empty", path.c_str()));
            }
        }

        Config::PackageEntry *Project::searchPackage(Config::ImportQuery &dependency)
        {
            // TODO OPTIONAL: Support semver ranges

            semver::version version;
            bool valid_version = semver::parse(dependency.version, version);

            semver::version candidate_version;
            bool valid_can_ver = false;

            semver::version temp_ver;
            bool valid_temp_ver = false;

            Config::PackageEntry *candidate_package = nullptr;

            for (auto &pkg : meta.packages)
            {
                if (pkg.name == dependency.name)
                {
                    if (!valid_version)
                    {
                        valid_temp_ver = semver::parse(pkg.version_matcher, temp_ver);
                        // Grab the newest
                        if (candidate_package == nullptr || (candidate_package && valid_can_ver && valid_temp_ver && candidate_version < temp_ver))
                        {
                            candidate_package = &pkg;
                            valid_can_ver = semver::parse(candidate_package->version_matcher, candidate_version);
                        }
                    }
                    else if (pkg.version_matcher == dependency.version)
                    {
                        // Grab this only
                        candidate_package = &pkg;
                        break;
                    }
                }
            }

            return candidate_package;
        }

        void Project::resolveFromFile(Config::ImportQuery &dependency, File *from = nullptr)
        {
            if (from == nullptr)
            {
                return;
            }

            Config::PackageEntry *candidate_package = searchPackage(dependency);

            if (candidate_package)
            {
                dependency.version = candidate_package->version_matcher;

                if (owner_workspace)
                {
                    owner_workspace->resolveProject(dependency);
                }
            }
            else
            {
                // TODO check if a path is only inside the project
                auto abs_path = from->meta.path.parent_path() / std::filesystem::path(dependency.name);

                dependency.attachFile( addFile(abs_path.c_str()).get()->meta.path );
            }
        }

        std::shared_ptr<File> Project::addFile(std::string abs_path)
        {
            // Thanks
            auto prev = registry.find(std::string(abs_path));
            if (prev != registry.end())
            {
                return prev->second;
            }
            else
            {
                auto file = std::make_shared<File>();
                file.get()->owner_project = this;
                registry[abs_path] = file;
                file.get()->fromPath(abs_path);
                return file;
            }
        }

        Project::operator json() const
        {
            json data = json::object();

            data["name"] = meta.name.c_str();
            data["version"] = meta.version.c_str();
            data["description"] = meta.description.c_str();
            data["author"] = meta.author.c_str();
            data["main"] = meta.main_path.c_str();
            data["root"] = meta.root_path.c_str();

            data["problems"] = json::array();
            data["keywords"] = json::array();
            data["files"] = json::array();
            data["packages"] = json::array();

            auto &aprobs = data.at("problems");
            auto &akeys = data.at("keywords");
            auto &afiles = data.at("files");
            auto &apacks = data.at("packages");

            for (auto &p : meta.packages)
            {
                apacks.push_back(std::string(p));
            }

            for (auto &k : meta.keywords)
            {
                akeys.push_back(k);
            }

            for (auto &p : problems)
            {
                aprobs.push_back(std::string(*p));
            }

            for (const auto &[key, value] : registry)
            {
                afiles.push_back( json(*value.get()) );
            }

            return data;
        }
    }
}