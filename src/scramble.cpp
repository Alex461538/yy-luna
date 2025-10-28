#include "scramble.hpp"

namespace YY
{
    namespace Scramble
    {
        Workspace pathToWorkspace(const std::filesystem::path &path)
        {
            Workspace workspace;
            workspace.loadFromPath(path);
            return workspace;
        }
    }
}