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
#include "yyconf.hpp"

#include <json.hpp>
#include <semver.hpp>

using json = nlohmann::json;

namespace YY
{
    namespace Scramble
    {
        struct Tree;
        struct File;

        struct ImportQuery;

        struct Workspace
        {
            std::filesystem::path root_path;

            std::map<std::string, std::shared_ptr<Tree>> projects;

            std::vector<std::shared_ptr<Problem::Problem>> problems;

            void panic(std::shared_ptr<Problem::Problem> problem);
            void loadFromPath(const std::filesystem::path &path);

            std::shared_ptr<Tree> addProject(const std::filesystem::path &path);

            void resolveProject(ImportQuery *dependency);

            operator json() const;
        };

        struct Tree
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
            std::vector<YYConf::Package>packages;

            std::map<std::string, std::shared_ptr<File>> files;
            std::vector<std::shared_ptr<Problem::Problem>> problems;

            void panic(std::shared_ptr<Problem::Problem> problem);
            void loadFromPath(const std::filesystem::path &path);

            operator json() const;

            void resolveFromFile(ImportQuery *dependency, File *from);

            std::shared_ptr<File> addFile(std::string abs_path);

            YYConf::Package *searchPackage(ImportQuery *dependency);
        };

        struct File
        {
            Tree *owner_project;

            std::filesystem::path path;
            std::filesystem::path relative_path;

            std::string content;

            std::vector<std::shared_ptr<Problem::Problem>> problems;

            std::vector<Token::Token> tokens;

            std::vector<ImportQuery> dependencies;
            std::map<std::string, std::pair<std::string, size_t>> importNamespaces;

            size_t addDependency(std::string query);
            void importNamespace(size_t dependency, std::string name, std::string alias);

            void resolve(size_t dependency);

            void panic(std::shared_ptr<Problem::Problem> problem);
            void loadFromPath(const std::filesystem::path &path);
            void lex();

            void preprocess();

            void flatten();

            operator json() const;
        };

        struct ImportQuery
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

            ImportQuery();
            ImportQuery(std::string _path);

            void attachFile(std::shared_ptr<File> target);
            void attachProject(std::shared_ptr<Tree> target);

            operator std::string() const;
        };

        namespace Transformers
        {
            Workspace fromPath(const std::filesystem::path &path);
        }
    }
}

#endif