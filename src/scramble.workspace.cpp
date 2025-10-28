#include "scramble.hpp"

namespace YY
{
    namespace Scramble
    {
        void Workspace::panic(std::shared_ptr<Problem::Problem> problem)
        {
            problems.push_back(problem);
        }

        void Workspace::fromPath(const std::filesystem::path &path)
        {
            root = addProject(path);
        }

        std::shared_ptr<Project> Workspace::addProject(const std::filesystem::path &path)
        {
            auto project = std::make_shared<Project>();

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
                auto prev = registry.find(std::string(path));
                if (prev != registry.end())
                {
                    return prev->second;
                }
                else
                {
                    project.get()->owner_workspace = this;
                    registry[path] = project;
                    project.get()->fromPath(path);
                    return project;
                }
            }

            return project;
        }

        Workspace::operator json() const
        {
            json data = json::object();

            data["problems"] = json::array();
            data["projects"] = json::array();

            auto &aprobs = data.at("problems");
            auto &aprojs = data.at("projects");

            for (auto &p : problems)
            {
                aprobs.push_back(std::string(*p));
            }

            for (const auto &[key, value] : registry)
            {
                aprojs.push_back( json(*value.get()) );
            }

            return data;
        }

        void Workspace::resolveProject(Config::ImportQuery &dependency)
        {
            // Necesito ir al yyconfig global y ver si está
            // Todavía no

            std::string abs_path = Config::resolveGlobalPackage(dependency);

            if (!abs_path.empty())
            {
                dependency.attachProject( addProject(abs_path).get()->meta.root_path );
            }
            else
            {}
        }
    }
}