#include "scramble.hpp"

namespace YY
{
    namespace Scramble
    {
        void File::panic(std::shared_ptr<Problem::Problem> problem)
        {
            problems.push_back(problem);
        }

        void File::lex()
        {
            tokens.clear();

            std::vector<std::tuple<char, size_t>> brackets;

            Lexer::Lexer lexer = Lexer::Lexer();

            lexer.content = content.data();
            lexer.length = content.size();

            Token::Token token = lexer.next();

            while (token.kind != Token::Kind::T_EOF)
            {
                tokens.push_back(token);

                switch (token.kind)
                {
                case Token::Kind::T_OPEN_PAREN:
                {
                    brackets.push_back({'(', tokens.size() - 1});
                }
                break;
                case Token::Kind::T_OPEN_CURLY:
                {
                    brackets.push_back({'{', tokens.size() - 1});
                }
                break;
                case Token::Kind::T_OPEN_SQUARE:
                {
                    brackets.push_back({'[', tokens.size() - 1});
                }
                break;
                case Token::Kind::T_CLOSE_PAREN:
                {
                    auto pair = brackets[brackets.size() - 1];
                    tokens[std::get<1>(pair)].enclosing = tokens.size() - 1;
                    if (std::get<0>(pair) != '(')
                    {
                        panic(Problem::FileProblem::get(Problem::Type::ERR_STRAY_PAREN, ") expected", path.c_str(), {token.location.line, token.location.column}));
                    }
                    brackets.pop_back();
                }
                break;
                case Token::Kind::T_CLOSE_CURLY:
                {
                    auto pair = brackets[brackets.size() - 1];
                    tokens[std::get<1>(pair)].enclosing = tokens.size() - 1;
                    if (std::get<0>(pair) != '{')
                    {
                        panic(Problem::FileProblem::get(Problem::Type::ERR_STRAY_PAREN, "} expected", path.c_str(), {token.location.line, token.location.column}));
                    }
                    brackets.pop_back();
                }
                break;
                case Token::Kind::T_CLOSE_SQUARE:
                {
                    auto pair = brackets[brackets.size() - 1];
                    tokens[std::get<1>(pair)].enclosing = tokens.size() - 1;
                    if (std::get<0>(pair) != '[')
                    {
                        panic(Problem::FileProblem::get(Problem::Type::ERR_STRAY_PAREN, "] expected", path.c_str(), {token.location.line, token.location.column}));
                    }
                    brackets.pop_back();
                }
                break;
                case Token::Kind::T_INVALID:
                {
                    panic(Problem::FileProblem::get(Problem::Type::ERR_INVALID_TOKEN, "Invalid token at lexing: " + std::string(token.text, token.length), path.c_str(), {token.location.line, token.location.column}));
                }
                break;
                default:
                    break;
                }

                token = lexer.next();
            }

            tokens.push_back(token);
        }

        void File::loadFromPath(const std::filesystem::path &path)
        {
            this->path = path;
            relative_path = std::filesystem::relative(path, owner_project->root_path);

            std::stringstream buffer = std::stringstream();
            std::ifstream file(path, std::ios::binary);

            if (!file.fail() && !file.bad())
            {
                buffer << file.rdbuf();
                content = buffer.str();
            }
            else
            {
                panic(Problem::WPProblem::get(Problem::Type::ERR_FILE_NOT_FOUND, "This file does not exist or is not readable", path.c_str()));
                return;
            }

            lex();

            preprocess();

            flatten();
        }

        File::operator json() const
        {
            json data = json::object();

            data["path"] = path.c_str();
            data["relative_path"] = relative_path.c_str();

            data["problems"] = json::array();
            // data["tokens"] = json::array();

            auto &aprobs = data.at("problems");
            // auto &atoks = data.at("tokens");

            for (auto &p : problems)
            {
                aprobs.push_back(std::string(*p));
            }

            /* for (auto &t : tokens)
            {
                atoks.push_back(std::string(t));
            } */

            return data;
        }

        ImportDependency::ImportDependency()
        {
            is_project = false;
        }

        ImportDependency::ImportDependency(std::string _path)
        {
            std::string::size_type version_sepr = _path.find(':');
            path = _path.substr(0, version_sepr);
            if (version_sepr != std::string::npos)
            {
                version = _path.substr(version_sepr + 1);
            }
        }

        size_t File::addDependency(std::string query)
        {
            dependencies.push_back(ImportDependency(query));
            return dependencies.size() - 1;
        }

        void File::importNamespace(size_t dependency, std::string name, std::string alias = "")
        {
            if (!alias.empty())
            {
                importNamespaces[alias] = {name, dependency};
            }
            else
            {
                importNamespaces[name] = {name, dependency};
            }
        }

        void File::preprocess()
        {
            for (int index = 0; index < tokens.size();)
            {
                Token::Token *token = &tokens[index++];

                // Verbose output for debugging
                // std::printf("Processing token: %s\n", std::string(token).c_str());

                // handle import statements
                if (token->kind == Token::Kind::K_IMPORT)
                {
                    // mark import
                    token->ignore = true;

                    Token::Token *symbolName = nullptr;
                    Token::Token *tk_from_as_comma = nullptr;
                    Token::Token *aliasName = nullptr;
                    Token::Token *pathToken = nullptr;

                    std::vector<std::pair<std::string, std::string>> deps;

                    // Consume if at least there can be a import <symbol> from "path"
                    while (index + 3 < tokens.size())
                    {
                        // Peek a symbol
                        symbolName = &tokens[index++];
                        // Check for identifier
                        if (symbolName->kind != Token::Kind::T_IDENTIFIER)
                        {
                            index--; // Prevent phantom consumption
                            panic(Problem::FileProblem::get(Problem::Type::ERR_EXPECTED, "<identifier> expected", path.c_str(), {token->location.line, token->location.column}));
                            break;
                        }

                        // mark symbol name
                        symbolName->ignore = true;

                        // Peek for as | from
                        tk_from_as_comma = &tokens[index++];

                        bool hasAlias = false;

                        // Check for an alias
                        if (tk_from_as_comma->kind == Token::Kind::K_AS)
                        {
                            // mark as kw
                            tk_from_as_comma->ignore = true;

                            hasAlias = true;
                            // Peek alias
                            aliasName = &tokens[index++];
                            // check for identifier
                            if (aliasName->kind != Token::Kind::T_IDENTIFIER)
                            {
                                index--; // Prevent phantom consumption
                                panic(Problem::FileProblem::get(Problem::Type::ERR_EXPECTED, "<identifier> expected", path.c_str(), {token->location.line, token->location.column}));
                                break;
                            }

                            // mark alias name
                            aliasName->ignore = true;

                            // Peek for from | ,
                            tk_from_as_comma = &tokens[index++];
                        }

                        // Store the symbol (with alias if any)
                        if (hasAlias)
                        {
                            deps.push_back({std::string(symbolName->text, symbolName->length),
                                            std::string(aliasName->text, aliasName->length)});
                        }
                        else
                        {
                            deps.push_back({std::string(symbolName->text, symbolName->length),
                                            ""});
                        }

                        // Check for , | from
                        if (tk_from_as_comma->kind == Token::Kind::T_COMMA)
                        {
                            // mark comma
                            tk_from_as_comma->ignore = true;

                            // continue to next symbol
                            continue;
                        }
                        else if (tk_from_as_comma->kind == Token::Kind::K_FROM)
                        {
                            // mark from
                            tk_from_as_comma->ignore = true;

                            pathToken = &tokens[index++];

                            if (pathToken->kind == Token::Kind::C_STRING)
                            {
                                // mark path
                                pathToken->ignore = true;

                                std::string importPath(pathToken->text, pathToken->length);
                                // Remove quotes
                                if (importPath.size() >= 2 && importPath.front() == '"' && importPath.back() == '"')
                                {
                                    importPath = importPath.substr(1, importPath.size() - 2);
                                }

                                // std::printf("Importing from file: %s\n", importPath.c_str());

                                size_t dependency = addDependency(importPath);

                                for (auto &sympair : deps)
                                {
                                    importNamespace(dependency, sympair.first, sympair.second);
                                }
                            }
                            else
                            {
                                index--; // Prevent phantom consumption
                                panic(Problem::FileProblem::get(Problem::Type::ERR_EXPECTED, "<string lit> expected", path.c_str(), {token->location.line, token->location.column}));
                                continue;
                            }

                            break;
                        }
                        else
                        {
                            index--; // Prevent phantom consumption
                            panic(Problem::FileProblem::get(Problem::Type::ERR_EXPECTED, ", | <from> expected", path.c_str(), {token->location.line, token->location.column}));
                            break;
                        }
                    }
                }
            }
        }

        void ImportDependency::attachFile(std::shared_ptr<File> target) {}
        void ImportDependency::attachProject(std::shared_ptr<Project> target) {}

        void Workspace::resolveProject(ImportDependency *dependency)
        {
            // Necesito ir al yyconfig global y ver si está
            // Todavía no

            if (dependency == nullptr)
            {
                return;
            }

            semver::version dep_version;
            bool valid_dep_version = semver::parse(dependency->version, dep_version);

            json yyconf = {};
            std::ifstream file((std::filesystem::path(YY::HOME_PATH) / "yyconf.json").c_str());

            if (file.fail())
            {
                panic(Problem::WPProblem::get(Problem::Type::ERR_PROJECT_NO_CONFIG, "Language config file not found", YY::HOME_PATH.c_str()));
            }
            else
            {
                try
                {
                    yyconf = json::parse(file);
                }
                catch (const std::exception &e)
                {
                    panic(Problem::WPProblem::get(Problem::Type::ERR_PROJECT_INVALID_CONFIG, e.what(), YY::HOME_PATH.c_str()));
                }
            }

            file.close();

            if (yyconf.is_object())
            {
                if (yyconf.contains("packages") && yyconf["packages"].is_array())
                {
                    for (auto &pkg : yyconf["packages"])
                    {
                        std::string name, version, path;
                        if (pkg.contains("name") && pkg["name"].is_string())
                        {
                            name = pkg["name"].get<std::string>();
                        }
                        if (pkg.contains("version") && pkg["version"].is_string())
                        {
                            version = pkg["version"].get<std::string>();
                        }
                        if (pkg.contains("path") && pkg["path"].is_string())
                        {
                            path = pkg["path"].get<std::string>();
                        }

                        semver::version pkg_version;
                        bool valid_pkg_version = semver::parse(version, pkg_version);

                        if (pkg_version == dep_version && name == dependency->path)
                        {
                            auto abs_path = HOME_PATH / std::filesystem::path(path);

                            addProject(abs_path);
                        }
                    }
                }
            }
        }

        Package *Project::searchPackage(ImportDependency *dependency)
        {
            // TODO OPTIONAL: Support semver ranges

            if (dependency == nullptr)
            {
                return nullptr;
            }

            semver::version version;
            bool valid_version = semver::parse(dependency->version, version);

            semver::version candidate_version;
            bool valid_can_ver = false;

            semver::version temp_ver;
            bool valid_temp_ver = false;

            Package *candidate_package = nullptr;

            for (auto &pkg : packages)
            {
                if (pkg.name == dependency->path)
                {
                    if (!valid_version)
                    {
                        valid_temp_ver = semver::parse(pkg.version, temp_ver);
                        // Grab the newest
                        if (candidate_package == nullptr || (candidate_package && valid_can_ver && valid_temp_ver && candidate_version < temp_ver))
                        {
                            candidate_package = &pkg;
                            valid_can_ver = semver::parse(candidate_package->version, candidate_version);
                        }
                    }
                    else if (pkg.version == dependency->version)
                    {
                        // Grab this only
                        candidate_package = &pkg;
                        break;
                    }
                }
            }

            return candidate_package;
        }

        void Project::resolveFromFile(ImportDependency *dependency, File *from = nullptr)
        {
            if (dependency == nullptr || from == nullptr)
            {
                return;
            }

            Package *candidate_package = searchPackage(dependency);

            if (candidate_package)
            {
                dependency->version = candidate_package->version;

                std::printf("Package: %s <%s>\n", candidate_package->name.c_str(), candidate_package->version.c_str());
                if (owner_workspace)
                {
                    owner_workspace->resolveProject(dependency);
                }
            }
            else
            {
                // TODO check if a path is only inside the project
                auto abs_path = from->path.parent_path() / std::filesystem::path(dependency->path);

                dependency->attachFile(
                    addFile(abs_path.c_str()));
            }
        }

        void File::resolve(size_t dependency_index)
        {
            auto &dependency = dependencies[dependency_index];

            if (owner_project != nullptr)
            {
                owner_project->resolveFromFile(&dependency, this);
            }
        }

        void File::flatten()
        {
            for (int i = 0; i < dependencies.size(); i++)
            {
                resolve(i);
            }
        }
    }
}