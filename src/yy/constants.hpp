#if !defined(__hpp_yy_constants__)
#define __hpp_yy_constants__

//  --- Global includes --- 
#include <string>
#include <filesystem>
#if defined(YY_OSAPI_UNIX)
#include <unistd.h>
#elif defined(YY_OSAPI_WIN32)
#include <windows>
#endif
//  --- Project includes ---
//  --- Local includes ---

namespace YY
{
    namespace Constants
    {
        enum class TransportKind
        {
            NONE,
            STDIO,
            LOG_FILE
        };

        enum class OsAPI
        {
            NONE,
            LINUX,
            WINDOWS
        };

        enum class ISA
        {
            NONE,
            X86_32,
            X86_64,
        };

        enum class Packaging
        {
            NONE,
            OBJ,
            EXE,
            ELF
        };

        namespace Paths
        {
            extern std::filesystem::path RUNNING_AT;
            extern std::filesystem::path HOME;

            extern std::filesystem::path LOG_FILE;
            extern std::filesystem::path GLOBAL_YYCONF;
        }
        
        extern const char* OSAPI_NAME;

        extern const char *ISA_NAME;
        extern const char* VERSION;
        extern const char* CLI_BANNER;

        void init();
    }
}

#endif