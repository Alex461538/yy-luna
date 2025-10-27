#include "interp.hpp"

namespace YY
{
    namespace Interp
    {
        void Workspace::loadFromScramble(YY::Scramble::Workspace &fs_workspace)
        {
            // please load everything in pos-order
            // this guarantees that any required symbol is available
            for (auto &proj : fs_workspace.projects)
            {
                //
            }
        }

        namespace Transformers
        {
            Workspace fromScramble(YY::Scramble::Workspace &fs_workspace)
            {
                Workspace workspace;
                workspace.loadFromScramble(fs_workspace);
                return workspace;
            }
        }
    }
}