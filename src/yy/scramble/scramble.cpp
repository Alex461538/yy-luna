#include "scramble.hpp"

namespace YY
{
    std::optional<std::shared_ptr<YY::Package>> Scramble::findPackage( std::filesystem::path path )
    {
        /* Search the required package */
        auto p = packages.find( path );
        /* Is it saved into the map? */
        if ( p != packages.end() )
        {
            return p->second;
        }
        return {};
    }

    std::optional<std::shared_ptr<YY::Package>> Scramble::addPackageFolder( std::filesystem::path path )
    {
        YY::Debug::log("Loading package at: %s\n", path.c_str());
        /* A relative path can't be resolved as is */
        if (!path.is_absolute())
        {
            return {};
        }
        /* If this package was loaded before, don't do it twice */
        auto preloaded = findPackage( path );
        if (preloaded)
        {
            YY::Debug::log("This package was loaded before\n");
            return *preloaded;
        }
        /* Create a new package */
        auto p = std::make_shared<YY::Package>();
        /* Try to initialie a package from the given path */
        if ( !p->fromPath(path) )
        {
            return {};
        }
        /* Save in the registry */
        packages[ std::string(path) ] = p;
        /* Load the new yyconf's requirements */
        for (auto &package : p->packages)
        {
            YY::Debug::log("Resolving yyconf package entry: %s\n", package.name.c_str());
            /* Try to resolve a package from a name and version selector */
            auto import_path = YY::Global::resolvePackage(package.name, package.versionSelector);
            /* Was it resolved successfully? */
            if (import_path)
            {
                YY::Debug::log("Found at: %s!\n", (*import_path).c_str());
                /* Add import recursively */
                auto dep = addPackageFolder(*import_path);
                if (dep)
                {
                    package.reference = *dep;
                }
            }
            else
            {
                YY::Debug::log("Not found\n");
            }
        }
        /* Better the yyconf things are loaded first */
        p->addFile( p->main );
        return p;
    }
}