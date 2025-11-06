#if !defined(__hpp_yy_out__)
#define __hpp_yy_out__

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
        extern YY::Constants::TransportKind out_place;

        void _clear(YY::Constants::TransportKind out);
        void _log(const char * fmt, va_list &args, YY::Constants::TransportKind out);

        inline void log(const char *fmt, ...)
        {
            va_list args;
            va_start(args, fmt);

            _log(fmt, args, out_place);

            va_end(args);
        }

        void set_out(YY::Constants::TransportKind out);
        void clear();
    }

    namespace Debug
    {
        extern YY::Constants::TransportKind out_place;

        inline void log(const char *fmt, ...)
        {
            #if defined(YY_DEBUG)
            va_list args;
            va_start(args, fmt);

            YY::Out::_log(fmt, args, out_place);

            va_end(args);
            #endif
        }

        void set_out(YY::Constants::TransportKind out);
        void clear();
    }
}

#endif