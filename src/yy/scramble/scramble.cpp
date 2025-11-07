#include "scramble.hpp"

namespace YY
{
    std::shared_ptr<YY::Package> Scramble::addPackageFolder( std::filesystem::path path )
    {
        YY::Debug::log("A mi me gusta la empanadaaaaaaa %s \n", path.c_str());

        auto p = std::make_shared<YY::Package>();

        p->fromPath(path);
        p->loadFiles();

        packages[ std::string(path) ] = p;
        return p;
    }
}