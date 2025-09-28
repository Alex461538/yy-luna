#include "scramble.hpp"

namespace YY
{
    namespace Scramble
    {
        void Project::panic(Problem::Problem problem)
        {
            problems.push_back(problem);
        }

        Project Project::fromPath(const std::filesystem::path &path)
        {
            Project project;
            project.path = path;

            std::printf("Loading workspace from path: %s\n", path.c_str());

            json yyconf = {};
            std::ifstream file((std::filesystem::path(path) / "yyconf.json").c_str());

            if (file.fail())
            {
                project.panic(Problem::problemOf<Problem::Type::ERR_PROJECT_NO_CONFIG>(path.c_str()));
            }
            else
            {
                try
                {
                    yyconf = json::parse(file);
                }
                catch (const std::exception &e)
                {
                    project.panic(Problem::problemOf<Problem::Type::ERR_PROJECT_INVALID_CONFIG>(path.c_str(), e.what()));
                }
            }

            file.close();

            if (!yyconf.is_null())
            {
                if (yyconf.contains("name") && yyconf["name"].is_string())
                {
                    project.name = yyconf["name"].get<std::string>();
                }
                else
                {
                    project.name = path.filename().string();
                    project.panic(Problem::problemOf<Problem::Type::ERR_PROJECT_INVALID_CONFIG>(path.c_str(), "Project name is missing or not a string."));
                }

                if (yyconf.contains("main") && yyconf["main"].is_string())
                {
                    project.main_file = yyconf["main"].get<std::string>();

                    std::filesystem::path main_path = path / yyconf["main"].get<std::string>();
                    if (!std::filesystem::exists(main_path))
                    {
                        project.main_file = "";
                        project.panic(Problem::problemOf<Problem::Type::ERR_FILE_NOT_FOUND>(main_path.c_str(), "Main file specified in configuration does not exist."));
                    }
                }
                else
                {
                    project.panic(Problem::problemOf<Problem::Type::ERR_PROJECT_INVALID_CONFIG>(path.c_str(), "Main file is missing or not a string."));
                }

                if (yyconf.contains("author") && yyconf["author"].is_string())
                {
                    project.author = yyconf["author"].get<std::string>();
                }

                if (yyconf.contains("description") && yyconf["description"].is_string())
                {
                    project.description = yyconf["description"].get<std::string>();
                }

                if (yyconf.contains("version") && yyconf["version"].is_string())
                {
                    project.version = yyconf["version"].get<std::string>();
                }
                else
                {
                    project.version = "0.1.0";
                    project.panic(Problem::problemOf<Problem::Type::ERR_PROJECT_INVALID_CONFIG>(path.c_str(), "Project version is missing or not a string."));
                }

                if (yyconf.contains("keywords") && yyconf["keywords"].is_array())
                {
                    for (const auto &keyword : yyconf["keywords"])
                    {
                        if (keyword.is_string())
                        {
                            project.keywords.push_back(keyword.get<std::string>());
                        }
                    }
                }
            }
            else
            {
                project.panic(Problem::problemOf<Problem::Type::ERR_PROJECT_INVALID_CONFIG>(path.c_str(), "Configuration file is empty."));
            }

            return project;
        }

        void Workspace::panic(Problem::Problem problem)
        {
            problems.push_back(problem);
        }

        Workspace Workspace::fromPath(const std::filesystem::path &path)
        {
            Workspace workspace;
            workspace.root_path = path;

            workspace.projects.push_back(Project::fromPath(path));

            return workspace;
        }
    }
}