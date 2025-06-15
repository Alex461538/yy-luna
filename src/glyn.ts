import * as path from 'path';

import { Project, Source } from "./fragment";

export function build(dir: string)
{
    let project = new Project();

    project.fromDirectory(dir);

    project.parse();

    // console.log(project);
}

export function generate_docs(dir: string)
{
    let project = new Project();

    project.fromDirectory(dir);

    // project.parse();
}