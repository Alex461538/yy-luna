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

        void Workspace::addProject(const std::filesystem::path &path)
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
                auto project = std::make_shared<Project>();
                project.get()->owner_workspace = this;
                projects.push_back(project);
                project.get()->loadFromPath(path);
            }
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

            for (auto &p : projects)
            {
                aprojs.push_back( json(*p.get()) );
            }

            return data;
        }
    }
}