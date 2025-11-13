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
        std::map<std::string, std::shared_ptr<YY::Package>> packages;

        std::optional<std::shared_ptr<YY::Package>> findPackage( std::filesystem::path path );
        std::optional<std::shared_ptr<YY::Package>> addPackageFolder( std::filesystem::path path );
    };
}

#endif