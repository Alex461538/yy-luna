#if !defined(__hpp_yy_yyconf__)
#define __hpp_yy_yyconf__

#include <string>

namespace YY
{
    namespace YYConf
    {
        struct Package {
            std::string name;
            std::string version;

            operator std::string() const;
        };
    }
}

#endif