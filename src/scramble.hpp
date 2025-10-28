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

#include "config.hpp"

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
        struct Project;
        struct File;
        struct ImportDependency;

        struct Workspace
        {
            std::shared_ptr<Project> root;

            std::map<std::string, std::shared_ptr<Project>> registry;
            std::vector<std::shared_ptr<Problem::Problem>> problems;

            void fromPath(const std::filesystem::path &path);
            void resolveProject(Config::ImportQuery &dependency);
            
            std::shared_ptr<Project> addProject(const std::filesystem::path &path);
            
            void panic(std::shared_ptr<Problem::Problem> problem);

            operator json() const;
        };

        struct Project
        {
            struct Meta {
                std::string name;
                std::string version;
                std::string description;
                std::string author;
                std::string main_path;

                std::filesystem::path root_path;

                std::vector<std::string> keywords;
                std::vector<Config::PackageEntry>packages;
            } meta;

            Workspace *owner_workspace;

            std::shared_ptr<File> main;
            std::map<std::string, std::shared_ptr<File>> registry;

            std::vector<std::shared_ptr<Problem::Problem>> problems;

            void panic(std::shared_ptr<Problem::Problem> problem);
            void fromPath(const std::filesystem::path &path);

            void resolveFromFile(Config::ImportQuery &dependency, File *from);
            
            Config::PackageEntry *searchPackage(Config::ImportQuery &dependency);
            
            std::shared_ptr<File> addFile(std::string abs_path);

            operator json() const;
        };

        struct File
        {
            enum class ImportType {
                IPT_NONE,
                IPT_FILE,
                IPT_PROJ
            };

            struct Meta {
                std::filesystem::path path;
                std::filesystem::path relative_path;
            } meta;

            Project *owner_project;

            /* firstly, an unknown type and the import query, then, after flattened the actual type and absoulte path */
            std::vector<std::pair<ImportType, std::string>> dependencies;
            std::map<std::string, std::pair<std::string, size_t>> importNamespaces;

            std::string content;
            std::vector<Token::Token> tokens;

            std::vector<std::shared_ptr<Problem::Problem>> problems;
            
            void importNamespace(size_t dependency, std::string name, std::string alias);
            void resolve(size_t dependency);
            void panic(std::shared_ptr<Problem::Problem> problem);
            void fromPath(const std::filesystem::path &path);
            
            void lex();
            void preprocess();
            void flatten();
            
            size_t addDependency(std::string query);

            operator json() const;
        };

        Workspace fromPath(const std::filesystem::path &path);
    }
}

#endif