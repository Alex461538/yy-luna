#include "problem.hpp"

namespace YY
{
    namespace Problem
    {
        Problem::operator std::string() const
        {
            return std::format("[{}] {}: {}", (severity == Severity::Error ? "ERROR" : (severity == Severity::Warning ? "WARNING" : "INFO")), name, text);
        }

        template <Type T>
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
            p.text = std::format("Invalid token at {}, line: {} - col: {}.", std::string(path), std::to_string(std::get<0>(location)), std::to_string(std::get<1>(location)));
            return p;
        }
    }
}