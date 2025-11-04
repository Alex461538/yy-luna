#if !defined(__hpp_yy_debug__)
#define __hpp_yy_debug__

#define YY_DEBUG

//  --- Global includes --- 
#include <fstream>
#include <stdarg.h>
//  --- Project includes ---
//  --- Local includes ---
#include "constants.hpp"

namespace YY
{
    namespace Out
    {
        YY::Constants::TransportKind out_place = YY::Constants::TransportKind::STDIO;

        void _clear(YY::Constants::TransportKind out)
        {
            switch (out)
            {
            case YY::Constants::TransportKind::STDIO: {
                //
            } break;
            case YY::Constants::TransportKind::LOG_FILE: {
                FILE *log = fopen(YY::Constants::Paths::LOG_FILE.c_str(), "w");
                if (log)
                {
                    fclose(log);
                }
            } break;
            default:
                break;
            }
        }

        void _log(const char *__restrict fmt, va_list &args, YY::Constants::TransportKind out)
        {
            switch (out)
            {
            case YY::Constants::TransportKind::STDIO: {
                vprintf(fmt, args); 
            } break;
            case YY::Constants::TransportKind::LOG_FILE: {
                FILE *log = fopen(YY::Constants::Paths::LOG_FILE.c_str(), "a");
                if (log)
                {
                    std::vfprintf(log, fmt, args);
                    fclose(log);
                }
            } break;
            default:
                break;
            }
        }

        inline void log(const char *fmt, ...)
        {
            va_list args;
            va_start(args, fmt);

            _log(fmt, args, out_place);

            va_end(args);
        }

        void set_out(YY::Constants::TransportKind out)
        {
            out_place = out;
        }

        void clear()
        {
            _clear(out_place);
        }
    }

    namespace Debug
    {
        YY::Constants::TransportKind out_place = YY::Constants::TransportKind::LOG_FILE;

        inline void log(const char *fmt, ...)
        {
            #if defined(YY_DEBUG)
            va_list args;
            va_start(args, fmt);

            YY::Out::_log(fmt, args, out_place);

            va_end(args);
            #endif
        }

        void set_out(YY::Constants::TransportKind out)
        {
            out_place = out;
        }

        void clear()
        {
            YY::Out::_clear(out_place);
        }
    }
}

#endif