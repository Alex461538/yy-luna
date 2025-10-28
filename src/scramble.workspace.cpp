#include "scramble.hpp"

namespace YY
{
    namespace Scramble
    {
        void Workspace::panic(std::shared_ptr<Problem::Problem> problem)
        {
            problems.push_back(problem);
        }

        void Workspace::loadFromPath(const std::filesystem::path &path)
        {
            root_path = path;
            addProject(path);
        }

        std::shared_ptr<Project> Workspace::addProject(const std::filesystem::path &path)
        {
            if (!path.is_absolute())
            {
                panic(Problem::WPProblem::get(Problem::Type::ERR_RESOURCE_UNREACHABLE, "A project can't be resolved from a relative path", path.c_str()));
            }
            else if (!std::filesystem::is_directory(path) || !std::filesystem::exists(path))
            {
                panic(Problem::WPProblem::get(Problem::Type::ERR_DIR_NOT_FOUND, "This is not a valid project path", path.c_str()));
            }
            else
            {
                auto prev = projects.find(std::string(path));
                if (prev != projects.end())
                {
                    return prev->second;
                }
                else
                {
                    auto project = std::make_shared<Project>();
                    project.get()->owner_workspace = this;
                    projects[path] = project;
                    project.get()->loadFromPath(path);
                    return project;
                }
            }

            return std::make_shared<Project>();
        }

        Workspace::operator json() const
        {
            json data = json::object();

            data["root"] = root_path.c_str();
            data["problems"] = json::array();
            data["projects"] = json::array();

            auto &aprobs = data.at("problems");
            auto &aprojs = data.at("projects");

            for (auto &p : problems)
            {
                aprobs.push_back(std::string(*p));
            }

            for (const auto &[key, value] : projects)
            {
                aprojs.push_back( json(*value.get()) );
            }

            return data;
        }

        void Workspace::resolveProject(ImportDependency *dependency)
        {
            // Necesito ir al yyconfig global y ver si está
            // Todavía no

            if (dependency == nullptr)
            {
                return;
            }

            semver::version dep_version;
            bool valid_dep_version = semver::parse(dependency->version, dep_version);

            json yyconf = {};
            std::ifstream file((std::filesystem::path(YY::HOME_PATH) / "yyconf.json").c_str());

            if (file.fail())
            {
                panic(Problem::WPProblem::get(Problem::Type::ERR_PROJECT_NO_CONFIG, "Language config file not found", YY::HOME_PATH.c_str()));
            }
            else
            {
                try
                {
                    yyconf = json::parse(file);
                }
                catch (const std::exception &e)
                {
                    panic(Problem::WPProblem::get(Problem::Type::ERR_PROJECT_INVALID_CONFIG, e.what(), YY::HOME_PATH.c_str()));
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

                        if (pkg_version == dep_version && name == dependency->path)
                        {
                            auto abs_path = HOME_PATH / std::filesystem::path(path);
                            dependency->attachProject(
                                addProject(abs_path)
                            );
                            break;
                        }
                    }
                }
            }
        }
    }
}