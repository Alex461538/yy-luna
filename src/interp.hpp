#if !defined(__hpp_yy_interp__)
#define __hpp_yy_interp__

#include <vector>
#include <memory>

#include <scramble.hpp>

/*
Should a true lower processing layer grab any upper layer's types?

Just find a counterexample, if iwant to modify the fundamental
structure of tokens, is it cheaper to change all lower layers
functionality or just the adjacente one's transformer?

Just the transformer, thanks! :D

Unless it's tightly coupled like the preprocessor D: i'll review that later

*/

namespace YY
{
    namespace Interp
    {
        struct Workspace;
        struct Project;
        struct File;
        
        namespace SymbolTable
        {
            struct TypeEntry
            {
            };

            struct SymbolEntry
            {
            };
        }

        namespace AST
        {
            enum class Kind
            {
                // Special
                K_ROOT,
                K_BLOCK,

                // Declarations
                K_DECL,
                K_DECL_NAMESPACE,
                K_DECL_FUNCTION,

                // Statements
                K_STATEMENT,
                K_RETURN
            };

            struct Node
            {
                // What is this node about?
                Kind kind = Kind::K_ROOT;
                // What is the decl or identifier type
                SymbolTable::TypeEntry result_type;
            };
        }

        /*
        Que pereza, esto debe ser simple, y quiero cachear luego archivos
        Ley del mínimo esfuerzo, otro workspace
        */
        class Workspace
        {
        public:
            std::vector<std::shared_ptr<Project>> projects;

            void loadFromScramble(YY::Scramble::Workspace &fs_workspace);
        };

        class Project
        {
        public:
            std::vector<std::shared_ptr<File>> projects;
        };

        class File
        {
        public:
            AST::Node root;
        };

        namespace Transformers
        {
            Workspace fromScramble(YY::Scramble::Workspace &fs_workspace);
        }
    }
}

#endif