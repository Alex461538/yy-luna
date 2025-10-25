#if !defined(__hpp_yy_workspace__)
#define __hpp_yy_workspace__

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <memory>
#include <tuple>
#include <utility>
#include <map>
#include <format>
#include <set>

#include "lexer.hpp"
#include "problem.hpp"
#include "info.hpp"

#include <json.hpp>
#include <semver.hpp>

using json = nlohmann::json;

namespace YY
{
    namespace Scramble
    {
        struct Workspace;
        struct Package;
        struct Project;
        struct File;
        struct ImportDependency;

        struct Workspace
        {
            std::filesystem::path root_path;

            std::map<std::string, std::shared_ptr<Project>> projects;

            std::vector<std::shared_ptr<Problem::Problem>> problems;

            void panic(std::shared_ptr<Problem::Problem> problem);
            void loadFromPath(const std::filesystem::path &path);

            std::shared_ptr<Project> addProject(const std::filesystem::path &path);

            void resolveProject(ImportDependency *dependency);

            operator json() const;
        };

        struct Package {
            std::string name;
            std::string version;

            operator std::string() const;
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

            std::shared_ptr<File> main;

            std::vector<std::string> keywords;
            std::vector<Package>packages;

            std::map<std::string, std::shared_ptr<File>> files;
            std::vector<std::shared_ptr<Problem::Problem>> problems;

            void panic(std::shared_ptr<Problem::Problem> problem);
            void loadFromPath(const std::filesystem::path &path);

            operator json() const;

            void resolveFromFile(ImportDependency *dependency, File *from);

            std::shared_ptr<File> addFile(std::string abs_path);

            Package *searchPackage(ImportDependency *dependency);
        };

        struct File
        {
            Project *owner_project;

            std::filesystem::path path;
            std::filesystem::path relative_path;

            std::string content;

            std::vector<std::shared_ptr<Problem::Problem>> problems;

            std::vector<Token::Token> tokens;

            std::vector<ImportDependency> dependencies;
            std::map<std::string, std::pair<std::string, size_t>> importNamespaces;

            size_t addDependency(std::string query);
            void importNamespace(size_t dependency, std::string name, std::string alias);

            void resolve(size_t dependency);

            void panic(std::shared_ptr<Problem::Problem> problem);
            void loadFromPath(const std::filesystem::path &path);
            void lex();

            void preprocess();

            /*
            Dependencias: Enlaces hacia otros archivos o proyectos

            diccionario de namespaces -> índices de importe
            array de importes -> Información de dependencia local
            */

            /*
            Applies all preprocessing queued operations to the file's token stream.
            This includes:
                - Import resolution
                - Branching (conditional compilation)
            */
            void flatten();

            operator json() const;
        };

        struct ImportDependency
        {
            enum class ImportType {
                IPT_NONE,
                IPT_FILE,
                IPT_PROJ,
                IPT_LINK // Imagine importing binary libs!
            };

            std::string version;
            std::string path;

            ImportType type;
            std::shared_ptr<void> target;

            ImportDependency();
            ImportDependency(std::string _path);

            void attachFile(std::shared_ptr<File> target);
            void attachProject(std::shared_ptr<Project> target);

            operator std::string() const;
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