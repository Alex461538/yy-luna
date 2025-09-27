#include <string>
#include <format>

namespace Problem
{
    enum class PanicAction {
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

        operator std::string() const
        {
            return std::format("[{}] {}: {}", (severity == Severity::Error ? "ERROR" : (severity == Severity::Warning ? "WARNING" : "INFO")), name, text);
        }
    };

    template<Type T>
    Problem problemOf(const char* a)
    {
        Problem p;
        p.severity = Severity::Error;
        p.name = "UNKNOWN_PROBLEM";
        p.text = "An unknown problem has occurred.";
        return p;
    }

    template<Type T>
    Problem problemOf(const char* a, const char* b)
    {
        Problem p;
        p.severity = Severity::Error;
        p.name = "UNKNOWN_PROBLEM";
        p.text = "An unknown problem has occurred.";
        return p;
    }

    // Especialización para cada tipo
    template<>
    Problem problemOf<Type::ERR_PROJECT_NO_CONFIG>(const char* project_path) {
        Problem p;
        p.severity = Severity::Error;
        p.name = "ERR_PROJECT_NO_CONFIG";
        p.text = std::format("Project at path {} has no configuration file.", std::string(project_path));
        return p;
    }

    template<>
    Problem problemOf<Type::ERR_PROJECT_INVALID_CONFIG>(const char* project_path, const char* text) {
        Problem p;
        p.severity = Severity::Error;
        p.name = "ERR_PROJECT_INVALID_CONFIG";
        p.text = std::format("Project at path {} has a corrupted configuration file. \n{}", std::string(project_path), std::string(text));
        return p;
    }

    template<>
    Problem problemOf<Type::ERR_FILE_NOT_FOUND>(const char* project_path, const char* text) {
        Problem p;
        p.severity = Severity::Error;
        p.name = "ERR_FILE_NOT_FOUND";
        p.text = std::format("File at path {} not found. \n{}", std::string(project_path), std::string(text));
        return p;
    }
}