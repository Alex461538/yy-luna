#include "problem.hpp"

namespace YY
{
    namespace Problem
    {
        std::map<Type, std::string> TypeNames = {
            {Type::ERR_PROJECT_NO_CONFIG, "ERR_PROJECT_NO_CONFIG"},
            {Type::ERR_PROJECT_INVALID_CONFIG, "ERR_PROJECT_INVALID_CONFIG"},
            {Type::ERR_FILE_NOT_FOUND, "ERR_FILE_NOT_FOUND"},
            {Type::ERR_DIR_NOT_FOUND, "ERR_DIR_NOT_FOUND"},
            {Type::ERR_RESOURCE_UNREACHABLE, "ERR_RESOURCE_UNREACHABLE"},
            {Type::ERR_STRAY_PAREN, "ERR_STRAY_PAREN"},
            {Type::ERR_INVALID_TOKEN, "ERR_INVALID_TOKEN"},
            {Type::ERR_EXPECTED, "ERR_EXPECTED"}
        };

        std::string Problem::levelToString() const
        {
            switch (severity)
            {
            case Severity::Info:
                return "INFO";
            case Severity::Warning:
                return "WARNING";
            case Severity::Error:
                return "ERROR";
            default:
                return "UNKNOWN";
            }
        }

        std::string Problem::typeToString() const
        {
            auto nameIt = TypeNames.find(type);
            return nameIt != TypeNames.end() ? nameIt->second : "JUST CRY";
        }

        std::shared_ptr<WPProblem> WPProblem::get(Type type, std::string text, std::string source)
        {
            auto p = std::make_shared<WPProblem>();
            p->type = type;
            p->text = text;
            p->source = source;
            return p;
        }

        std::shared_ptr<FileProblem> FileProblem::get(Type type, std::string text, std::string source, std::pair<int16_t, int32_t> location)
        {
            auto p = std::make_shared<FileProblem>();
            p->type = type;
            p->text = text;
            p->source = source;
            p->line = location.first;
            p->column = location.second;
            return p;
        }

        Problem::operator std::string() const
        {
            return asText();
        }

        std::string Problem::asText() const
        {
            return std::format("[{}] {}: {}", typeToString(), levelToString(), text);
        }

        std::string WPProblem::asText() const
        {
            return std::format("[{}] {}: {}\n\tAt: {}", typeToString(), levelToString(), text, source);
        }

        std::string FileProblem::asText() const
        {
            return std::format("[{}] {}: {}\n\tAt: {} - line: {} col: {}", typeToString(), levelToString(), text, source, line, column);
        }

        /* template <Type T>
        Problem problemOf(const char *a)
        {
            Problem p;
            p.severity = Severity::Error;
            p.name = "UNKNOWN_PROBLEM";
            p.text = "An unknown problem has occurred.";
            return p;
        }

        template <Type T>
        Problem problemOf(const char *a, const char *b)
        {
            return problemOf<T>(a);
        }

        // Especialización para cada tipo
        template <>
        Problem problemOf<Type::ERR_PROJECT_NO_CONFIG>(const char *path)
        {
            Problem p;
            p.severity = Severity::Error;
            p.name = "ERR_PROJECT_NO_CONFIG";
            p.text = std::format("Project at path {} has no configuration file.", std::string(path));
            return p;
        }

        template <>
        Problem problemOf<Type::ERR_EXPECTED>(const char *message, std::tuple<int32_t, int32_t> location)
        {
            Problem p;
            p.severity = Severity::Error;
            p.name = "ERR_PROJECT_NO_CONFIG";
            p.text = std::format("A different thing was expected here: {}. line: {} - col: {}.", std::string(message), std::to_string(std::get<0>(location)), std::to_string(std::get<1>(location)));
            return p;
        }

        template <>
        Problem problemOf<Type::ERR_PROJECT_INVALID_CONFIG>(const char *path, const char *text)
        {
            Problem p;
            p.severity = Severity::Error;
            p.name = "ERR_PROJECT_INVALID_CONFIG";
            p.text = std::format("Project at path {} has a corrupted configuration file. \n{}", std::string(path), std::string(text));
            return p;
        }

        template <>
        Problem problemOf<Type::ERR_FILE_NOT_FOUND>(const char *path, const char *text)
        {
            Problem p;
            p.severity = Severity::Error;
            p.name = "ERR_FILE_NOT_FOUND";
            p.text = std::format("File at path {} not found. \n{}", std::string(path), std::string(text));
            return p;
        }

        template <>
        Problem problemOf<Type::ERR_DIR_NOT_FOUND>(const char *path, const char *text)
        {
            Problem p;
            p.severity = Severity::Error;
            p.name = "ERR_FILE_NOT_FOUND";
            p.text = std::format("Directory at path {} not found. \n{}", std::string(path), std::string(text));
            return p;
        }

        template <>
        Problem problemOf<Type::ERR_RESOURCE_UNREACHABLE>(const char *path, const char *text)
        {
            Problem p;
            p.severity = Severity::Error;
            p.name = "ERR_RESOURCE_UNREACHABLE";
            p.text = std::format("Resource at path {} can't be resolved. \n{}", std::string(path), std::string(text));
            return p;
        }

        template <>
        Problem problemOf<Type::ERR_STRAY_PAREN>(const char *path, std::tuple<int32_t, int32_t> location)
        {
            Problem p;
            p.severity = Severity::Error;
            p.name = "ERR_STRAY_PAREN";
            p.text = std::format("Unclosed token at {}, line: {} - col: {}.", std::string(path), std::to_string(std::get<0>(location)), std::to_string(std::get<1>(location)));
            return p;
        }

        template <>
        Problem problemOf<Type::ERR_INVALID_TOKEN>(const char *path, std::tuple<int32_t, int32_t> location)
        {
            Problem p;
            p.severity = Severity::Error;
            p.name = "ERR_INVALID_TOKEN";
            p.text = std::format("Invalid token at {}", std::string(path));
            return p;
        } */
    }
}