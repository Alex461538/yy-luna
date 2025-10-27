#include "scramble.hpp"

namespace YY
{
    namespace Scramble
    {
        namespace Transformers
        {
            Workspace fromPath(const std::filesystem::path &path)
            {
                Workspace workspace;
                workspace.loadFromPath(path);
                return workspace;
            }
        }
    }
}