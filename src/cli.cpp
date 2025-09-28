#include "cli.hpp"

namespace YY {
    namespace CLI {
        json parseArgs(int argc, const char **argv)
        {
            argh::parser cmdl;
            cmdl.add_params({"-s", "-os", "-arch", "-vendor", "-e", "-do"});
            cmdl.parse(argv);

            json payload = json::object();

            payload["source"] = std::filesystem::current_path() / cmdl("-s", "").str();
            payload["os"] = cmdl("-os", Info::OS_NAME).str();
            payload["arch"] = cmdl("-arch", Info::ARCH_NAME).str();
            payload["vendor"] = cmdl("-vendor", "unknown").str();
            payload["emit"] = cmdl("-e", "obj").str();
            payload["action"] = cmdl("-do", "build").str();

            return payload;
        }
    }
}