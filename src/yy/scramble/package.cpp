#include "package.hpp"

namespace YY
{
    bool Package::fromPath(std::filesystem::path path)
    {
        /* A package can't be resolved from a relative path as is */
        if (!path.is_absolute())
        {
            return false;
        }
        /* Store path and load it's yyconf */
        dir = path;
        json yyconf = {};
        std::ifstream file((std::filesystem::path(path) / "yyconf.json").c_str());
        /* Was it loaded? */
        if (file.fail())
        {
            return false;
        }
        else
        {
            /* Was it valid JSON? */
            try
            {
                yyconf = json::parse(file);
            }
            catch (const std::exception &e)
            {
                return false;
            }
        }
        /* Release the file */
        file.close();
        if (yyconf.is_object())
        {
            /* Has it the required properties? */
            if (
                yyconf.contains("name") &&
                yyconf.contains("main") &&
                yyconf.contains("version") && 
                yyconf["version"].is_string() &&
                yyconf["name"].is_string() &&
                yyconf["main"].is_string())
            {
                name = yyconf["name"].get<std::string>();
                main = yyconf["main"].get<std::string>();
                /* The path for main should be a relative file */
                if (main.is_absolute())
                {
                    return false;
                }
                /* Calculate the final main path and check for existance */
                main = path / main;
                if (!std::filesystem::exists(main))
                {
                    return false;
                }
                /* Check for a valid package version */
                if (!semver::parse(yyconf["version"].get<std::string>(), version))
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
            /* Author property */
            if (yyconf.contains("author") && yyconf["author"].is_string())
            {
                author = yyconf["author"].get<std::string>();
            }
            /* Description property */
            if (yyconf.contains("description") && yyconf["description"].is_string())
            {
                description = yyconf["description"].get<std::string>();
            }
            /* Keywords list */
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
            /* Packages list */
            if (yyconf.contains("packages") && yyconf["packages"].is_array())
            {
                for (const auto &package : yyconf["packages"])
                {
                    if (package.is_object())
                    {
                        YY::Package::PackageEntry pkg;

                        if (package.contains("name") and package["name"].is_string())
                        {
                            pkg.name = package["name"].get<std::string>();
                        }

                        if (package.contains("version") and package["version"].is_string())
                        {
                            std::string v = package["version"].get<std::string>();
                            semver::version sm_v;
                            semver::range_set sm_rgs;
                            if (semver::parse(v, sm_v)) 
                            {
                                pkg.versionSelector = sm_v;
                            }
                            else if (semver::parse(v, sm_rgs))
                            {
                                pkg.versionSelector = sm_rgs;
                            }
                            else
                            {
                                return false;
                            }
                        }
                        packages.push_back(pkg);
                    }
                }
            }
        }
        else
        {
            return false;
        }
        return true;
    }

    std::optional<std::shared_ptr<YY::TextFile>> Package::findFile( std::filesystem::path path )
    {
        /* Search the required package */
        auto f = files.find( path );
        /* Is it saved into the map? */
        if ( f != files.end() )
        {
            return f->second;
        }
        return {};
    }

    std::optional<std::shared_ptr<Package>> Package::resolvePackage(std::string name, std::optional<Global::VersionSelector> version_selector)
    {
        if (version_selector)
        {
            std::shared_ptr<Package> p;
            for ( auto &pkg : packages )
            {
                if (
                    (std::holds_alternative<Global::Version>(*version_selector) && ( 
                        pkg.name == name && 
                        pkg.reference && (
                            (*(pkg.reference))->version == std::get<Global::Version>(*version_selector)) )) ||
                    (std::holds_alternative<Global::VersionRange>(*version_selector) && ( 
                        pkg.name == name && 
                        pkg.reference && 
                        std::get<Global::VersionRange>(*version_selector).contains( (*(pkg.reference))->version ) && (
                            p == nullptr || 
                            (*(pkg.reference))->version > p->version) ))
                     )
                {
                    p = *(pkg.reference);
                }
            }
            if ( p ) {
                return p;
            }
        }
        else
        {
            std::shared_ptr<Package> p;
            for ( auto &pkg : packages )
            {
                if (
                    pkg.name == name && 
                    pkg.reference && (
                    p == nullptr || (*(pkg.reference))->version > p->version) )
                {
                    p = *(pkg.reference);
                }
            }
            if ( p ) {
                return p;
            }
        }

        return {};
    }

    std::optional<std::shared_ptr<YY::TextFile>> Package::addFile(std::filesystem::path path)
    {
        YY::Debug::log("Loading file at: %s\n", path.c_str());
        /* Files can't be resolved from relative paths */
        if (!path.is_absolute())
        {
            return {};
        }
        /* Was this file previously loaded? */
        auto preloaded = findFile( path );
        if (preloaded)
        {
            YY::Debug::log("This file was loaded before\n");
            return *preloaded;
        }
        /* Create a new file */
        auto f = std::make_shared<YY::TextFile>();
        /* Try to initialie a file from the given path */
        if ( !f->fromPath(path) )
        {
            return {};
        }
        /* Save in the registry */
        files[ std::string(path) ] = f;
        /* Lex file, you know, right? */
        f->lex();
        /* Generate any preprocessor information for the file */
        f->preprocess();
        /* Resolve imports */
        for (auto &import : f->imports)
        {
            YY::Debug::log("Resolving for file the entry: %s\n", import.name.c_str());

            std::filesystem::path candidate_file = f->dir / import.name;
            /* itsa fil */
            if ( std::filesystem::exists(candidate_file) )
            {
                /* Add a file */
                import.reference = addFile( candidate_file );
            }
            else
            /* Itsa lib */
            {
                /* Try to resolve an entry from the packages */
                import.reference = resolvePackage( import.name, import.versionSelector );
                /* Was it found? */
                if (import.reference)
                {
                    YY::Debug::log("Got this: %s\n", std::get<std::shared_ptr<Package>>(*import.reference)->dir.c_str());
                }
                else
                {
                    // please panic
                }
            }
        }
        return f;
    }
}
