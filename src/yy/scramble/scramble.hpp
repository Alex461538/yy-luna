#if !defined(__hpp_yy_scramble__)
#define __hpp_yy_scramble__

//  --- Global includes ---
#include <optional>
//  --- Project includes ---
//  --- Local includes ---
#include "../out.hpp"
#include "../lsp/lsp-message.hpp"

namespace YY
{
    struct Scramble {
        void addPackageFolder( YY::LSP::WorkspaceFolder folder );
    };
}

#endif