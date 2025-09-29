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
            ERR_DIR_NOT_FOUND = 3,
            ERR_RESOURCE_UNREACHABLE = 4,
            ERR_STRAY_PAREN = 5,
            ERR_INVALID_TOKEN = 6,
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

        template <Type T>
        Problem problemOf(const char *a, std::tuple<int32_t, int32_t> location);

        // Especialización para cada tipo
        template <>
        Problem problemOf<Type::ERR_PROJECT_NO_CONFIG>(const char *path);

        template <>
        Problem problemOf<Type::ERR_PROJECT_INVALID_CONFIG>(const char *path, const char *text);

        template <>
        Problem problemOf<Type::ERR_FILE_NOT_FOUND>(const char *path, const char *text);

        template <>
        Problem problemOf<Type::ERR_DIR_NOT_FOUND>(const char *path, const char *text);

        template <>
        Problem problemOf<Type::ERR_RESOURCE_UNREACHABLE>(const char *path, const char *text);

        template <>
        Problem problemOf<Type::ERR_STRAY_PAREN>(const char *path, std::tuple<int32_t, int32_t> location);

        template <>
        Problem problemOf<Type::ERR_INVALID_TOKEN>(const char *path, std::tuple<int32_t, int32_t> location);
    }
}

#endif