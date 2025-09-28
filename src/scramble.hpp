#if !defined(__hpp_yy_workspace__)
#define __hpp_yy_workspace__

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <memory>

#include "problem.hpp"

#include <json.hpp>

using json = nlohmann::json;

namespace YY
{
    namespace Scramble
    {
        struct Workspace;
        struct Project;
        struct File;

        struct Workspace
        {
            std::filesystem::path root_path;

            std::vector<std::shared_ptr<Project>> projects;

            std::vector<Problem::Problem> problems;

            void panic(Problem::Problem problem);
            void loadFromPath(const std::filesystem::path &path);

            void addProject(const std::filesystem::path &path);

            operator json() const;
        };

        struct Project
        {
            Workspace *owner_workspace;

            std::filesystem::path root_path;

            std::string name;
            std::string version;
            std::string description;
            std::string author;
            std::string main_path;
            std::vector<std::string> keywords;

            std::vector<std::shared_ptr<File>> files;
            std::vector<Problem::Problem> problems;

            void panic(Problem::Problem problem);
            void loadFromPath(const std::filesystem::path &path);

            operator json() const;
        };

        struct File
        {
            Project* owner_project;

            std::filesystem::path path;
            std::filesystem::path relative_path;

            std::string content;

            std::vector<Problem::Problem> problems;
            std::vector<std::shared_ptr<File>> imports;

            void panic(Problem::Problem problem);
            void loadFromPath(const std::filesystem::path &path);

            operator json() const;
        };

        Workspace pathToWorkspace(const std::filesystem::path &path);
    }
}

/*

Inside a workspace there are different projects and one is marked as root.

Inside a project there are files and one marked as root.

A file can depend of other files.
A file can also depend on a project, this implies either having a project reference or could simply be the project's root file reference.

A porject has a reference to it's workspace.
A file has a reference to it's project

If a file imports another thing, it tells the project to resolve it:
    If the name is a path (.../file.yy) and:
        it's inside the project folder, it tries to resolve the name relative to the requester.
        it's outside the project folder, raises an error, we don't want to get hacked somehow.
    If the name seems to be a package name:
        The project searches the name in its yyconf packages entry and tells the workspace to resolve it.
        For the workspace:
            If it's in their loaded projects, it recycles the reference.
            If it's not, it tells yy-luna to resolve it from the installed packages and loads it.

Resolvers for those operations could be found in the classes, or be in a resolver file

One thing i thought about, for avoiding conflicts and making everything easier, 
is isolating dependencies, for instance, if A->B & B->C then A-/->C, 
unless A explicitly imports it. 
Each file should export namespaces (variables in their global namespace are private), 
and each requester, imports the desired namespaces. 

import Std from "std";
import Truetype from "truetype";
import Truetype as Truetype_legacy from "truetype:0.1.0";
import Sub from "sub/sub.yy";

*/

#endif