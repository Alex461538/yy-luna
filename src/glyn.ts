import * as path from 'path';

import { Project, Source } from "./fragment";

export function build(dir: string)
{
    let project = new Project();
    project.fromDirectory(dir);

    console.log(project.main);
    
}