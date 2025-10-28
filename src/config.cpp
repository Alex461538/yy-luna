#include "config.hpp"

namespace YY
{
    namespace Config
    {
        std::string resolveGlobalPackage(PackageEntry query)
        {
            //
            return "";
        }

        PackageEntry::operator std::string() const
        {
            return std::format("{} <{}>", name, version_matcher.empty() ? "?" : version_matcher);
        }

        ImportQuery::ImportQuery(std::string &_query)
        {
            version = "";
            name = "";

            std::string::size_type version_sepr = _query.find(':');
            name = _query.substr(0, version_sepr);

            if (version_sepr != std::string::npos)
            {
                version = _query.substr(version_sepr + 1);
            }
        }

        void ImportQuery::attachFile(std::string path)
        {
            this->path = path;
        }

        void ImportQuery::attachProject(std::string path)
        {
            this->path = path;
        }
    }
}