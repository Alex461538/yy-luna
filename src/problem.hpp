#if !defined(__hpp_yy_problem__)
#define __hpp_yy_problem__

#include <string>
#include <format>
#include <map>
#include <utility>
#include <memory>

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

        enum class Type : int16_t
        {
            ERR_PROJECT_NO_CONFIG = 0,
            ERR_PROJECT_INVALID_CONFIG = 1,
            ERR_FILE_NOT_FOUND = 2,
            ERR_DIR_NOT_FOUND = 3,
            ERR_RESOURCE_UNREACHABLE = 4,
            ERR_STRAY_PAREN = 5,
            ERR_INVALID_TOKEN = 6,
            ERR_EXPECTED = 7
        };

        extern std::map<Type, std::string> TypeNames;

        class Problem
        {
        public:
            std::string source;
            std::string text;
            
            int32_t line = -1;
            int16_t column = -1;
            Type type;
            Severity severity;

            std::string levelToString() const;
            std::string typeToString() const;

            virtual std::string asText() const;

            operator std::string() const;
        };

        class WPProblem : public Problem
        {
        public:
            std::string asText() const;

            static std::shared_ptr<WPProblem> get(Type type, std::string text, std::string source);
        };

        class FileProblem : public Problem
        {
        public:
            std::string asText() const;

            static std::shared_ptr<FileProblem> get(Type type, std::string text, std::string source, std::pair<int16_t, int32_t> location);
        };
    }
}

#endif