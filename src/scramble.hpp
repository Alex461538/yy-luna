#if !defined(__hpp_yy_workspace__)
#define __hpp_yy_workspace__

#include <fstream>
#include <filesystem>
#include <string>
#include <vector>

#include "problem.hpp"

#include <json.hpp>

using json = nlohmann::json;

namespace YY
{
    namespace Scramble
    {
        struct Project
        {
            std::filesystem::path path;
            std::string name;
            std::string main_file;
            std::string author;
            std::string description;
            std::string version;
            std::vector<std::string> keywords;

            std::vector<Problem::Problem> problems;

            void panic(Problem::Problem problem);

            static Project fromPath(const std::filesystem::path &path);
        };

        struct File
        {
            std::filesystem::path path;
            std::filesystem::path relative_to_project_path;

            std::string content;
        };

        struct Workspace
        {
            std::filesystem::path root_path;

            std::vector<Project> projects;
            std::vector<File> files;

            std::vector<Problem::Problem> problems;

            void panic(Problem::Problem problem);

            static Workspace fromPath(const std::filesystem::path &path);
        };
    }
}

#endif