#if !defined(__hpp_yy_problem__)
#define __hpp_yy_problem__

#include <string>
#include <format>

namespace YY
{
    namespace Problem
    {
        enum class PanicAction
        {
            Halt,
            Continue
        };

        enum class Severity
        {
            Info = 0,
            Warning = 1,
            Error = 2
        };

        enum class Type
        {
            ERR_PROJECT_NO_CONFIG = 0,
            ERR_PROJECT_INVALID_CONFIG = 1,
            ERR_FILE_NOT_FOUND = 2,
        };

        struct Problem
        {
            Severity severity;
            std::string name;
            std::string text;

            operator std::string() const;
        };

        template <Type T>
        Problem problemOf(const char *a);

        template <Type T>
        Problem problemOf(const char *a, const char *b);

        // Especialización para cada tipo
        template <>
        Problem problemOf<Type::ERR_PROJECT_NO_CONFIG>(const char *project_path);

        template <>
        Problem problemOf<Type::ERR_PROJECT_INVALID_CONFIG>(const char *project_path, const char *text);

        template <>
        Problem problemOf<Type::ERR_FILE_NOT_FOUND>(const char *project_path, const char *text);
    }
}

#endif