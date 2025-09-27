#include <iostream>
#include <string>

#include "workspace.hpp"

int main()
{
    Workspace::Workspace workspace = Workspace::Workspace::fromPath("./samples/test");

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