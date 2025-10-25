#include "scramble.hpp"

namespace YY
{
    namespace Scramble
    {
        void Project::panic(std::shared_ptr<Problem::Problem> problem)
        {
            problems.push_back(problem);
        }

        void Project::loadFromPath(const std::filesystem::path &path)
        {
            root_path = path;

            //std::printf("Loading workspace from path: %s\n", path.c_str());

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
                    name = yyconf["name"].get<std::string>();
                }
                else
                {
                    name = path.filename().string();
                    panic(Problem::WPProblem::get(Problem::Type::ERR_PROJECT_INVALID_CONFIG, "Project name is missing or not a string", path.c_str()));
                }

                if (yyconf.contains("main") && yyconf["main"].is_string())
                {
                    main_path = yyconf["main"].get<std::string>();

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
                    author = yyconf["author"].get<std::string>();
                }

                if (yyconf.contains("description") && yyconf["description"].is_string())
                {
                    description = yyconf["description"].get<std::string>();
                }

                if (yyconf.contains("version") && yyconf["version"].is_string())
                {
                    version = yyconf["version"].get<std::string>();
                }
                else
                {
                    version = "0.0.1";
                    panic(Problem::WPProblem::get(Problem::Type::ERR_PROJECT_INVALID_CONFIG, "Version is missing or not a string", path.c_str()));
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
                            Package pkg;

                            if (package.contains("name") and package["name"].is_string())
                            {
                                pkg.name = package["name"];
                            }

                            if (package.contains("version") and package["version"].is_string())
                            {
                                pkg.version = package["version"];
                            }

                            packages.push_back(pkg);
                        }
                    }
                }

                if (!main_path.empty())
                {
                    auto abs_path = std::filesystem::path(main_path);

                    if (!abs_path.is_absolute())
                    {
                        abs_path = root_path / main_path;
                    }

                    addFile(abs_path.c_str());
                }
            }
            else
            {
                panic(Problem::WPProblem::get(Problem::Type::ERR_PROJECT_INVALID_CONFIG, "Configuration file is empty", path.c_str()));
            }
        }

        std::shared_ptr<File> Project::addFile(std::string abs_path)
        {
            // TODO: PLEASE PLEASE CHECK IF IT IS ALREADY LOADED FIRST
            std::printf("Adding file at: %s\n", abs_path.c_str());
            auto file = std::make_shared<File>();
            file.get()->owner_project = this;
            files.push_back(file);
            file.get()->loadFromPath(abs_path);
            return file;
        }

        Package::operator std::string() const
        {
            return std::format("{} <{}>", name, version.empty() ? "?" : version);
        }

        Project::operator json() const
        {
            json data = json::object();

            data["name"] = name.c_str();
            data["version"] = version.c_str();
            data["description"] = description.c_str();
            data["author"] = author.c_str();
            data["main"] = main_path.c_str();
            data["root"] = root_path.c_str();

            data["problems"] = json::array();
            data["keywords"] = json::array();
            data["files"] = json::array();
            data["packages"] = json::array();

            auto &aprobs = data.at("problems");
            auto &akeys = data.at("keywords");
            auto &afiles = data.at("files");
            auto &apacks = data.at("packages");

            for (auto &p : packages)
            {
                apacks.push_back(std::string(p));
            }

            for (auto &k : keywords)
            {
                akeys.push_back(k);
            }

            for (auto &p : problems)
            {
                aprobs.push_back(std::string(*p));
            }

            for (auto &f : files)
            {
                afiles.push_back( json(*f.get()) );
            }

            return data;
        }
    }
}