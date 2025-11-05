#include "out.hpp"

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