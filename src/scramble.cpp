#include "scramble.hpp"

namespace YY
{
    namespace Scramble
    {
        Workspace fromPath(const std::filesystem::path &path)
        {
            Workspace workspace;
            workspace.fromPath(path);
            return workspace;
        }
    }
}