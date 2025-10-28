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
            std::shared_ptr<Project> root;

            std::map<std::string, std::shared_ptr<Project>> registry;
            std::vector<std::shared_ptr<Problem::Problem>> problems;

            void fromPath(const std::filesystem::path &path);
            void resolveProject(ImportDependency *dependency);
            
            std::shared_ptr<Project> addProject(const std::filesystem::path &path);
            
            void panic(std::shared_ptr<Problem::Problem> problem);

            operator json() const;
        };

        struct Package {
            std::string name;
            std::string version;

            operator std::string() const;
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
                std::vector<Package>packages;
            } meta;

            Workspace *owner_workspace;

            std::shared_ptr<File> main;
            std::map<std::string, std::shared_ptr<File>> registry;

            std::vector<std::shared_ptr<Problem::Problem>> problems;

            void panic(std::shared_ptr<Problem::Problem> problem);
            void fromPath(const std::filesystem::path &path);
            void resolveFromFile(ImportDependency *dependency, File *from);
            
            Package *searchPackage(ImportDependency *dependency);
            
            std::shared_ptr<File> addFile(std::string abs_path);

            operator json() const;
        };

        struct File
        {
            struct Meta {
                std::filesystem::path path;
                std::filesystem::path relative_path;
            } meta;

            Project *owner_project;

            std::vector<ImportDependency> dependencies;
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

        Workspace fromPath(const std::filesystem::path &path);
    }
}

#endif