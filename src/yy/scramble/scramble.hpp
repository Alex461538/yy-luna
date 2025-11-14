#if !defined(__hpp_yy_scramble__)
#define __hpp_yy_scramble__

//  --- Global includes ---
#include <optional>
#include <variant>
#include <filesystem>
#include <memory>
//  --- Project includes ---
//  --- Local includes ---
#include "../global.hpp"
#include "../out.hpp"

#include "package.hpp"

namespace YY
{
    struct Scramble {
        /* 
         This can generate problems if two 
         yyconf entries with different versions 
         are used and the both point to the same path (overwrite)
        */
        std::map<std::string, std::shared_ptr<YY::Package>> packages;

        std::optional<std::shared_ptr<YY::Package>> findPackage( std::filesystem::path path );
        std::optional<std::shared_ptr<YY::Package>> addPackageFolder( std::filesystem::path path );
    };
}

#endif