#include <iostream>
#include <string>

#include "yyl.hpp"

/*

I have a plan, for sure I will not fail this time.

Lets consider a layer architecture with these conditions:

- Any layer can ddefine specific types and operations with them. Also, it can assign transformers for converting data from the adjacent upper layer into those specific types.
- Any layer shoult NOT depend on lower layers nor trigger operations there.

The sketch of this is:

Scramble: Builds a deptree from a given root project path
    - file resolving and preprocessing
    
Interpret: Build AST's and performs analysis to a given deptree, also offers AST operations.
    # Depends on scramble
    - symbol resolving, analysis, AST building - cleaning

Translate: Swappable, from a given AST it can vary in results:
    - Buils IR files
    - Build docs
    - Compiles into assembly (direct custom
    - Do whatever you want with an external program (DSL's for images, sound, music sheets!)

Any lower layer could not be so important as long as it respects the upper provided data for the use case:
    - example 1: Panic if new keyword is used "as is" in platform without heap is normal
    - example 2: Errors because of platform incomplete support are worrying

The compilation process (Layer use) is managed by an external entity via predefined pipelines or manual instructions.

*/

int main( int argc, const char **argv )
{
    YY::init();

    YY::Scramble::Workspace workspace = YY::Scramble::Workspace::fromPath("./samples/test");

    for (const auto& problem : workspace.problems)
    {
        std::cout << std::string(problem) << std::endl;
    }

    std::cout << workspace.problems.size();
    for (const auto& project_problem : workspace.projects[0].problems)
    {
        std::cout << std::string(project_problem) << std::endl;
    }

    return 0;
}