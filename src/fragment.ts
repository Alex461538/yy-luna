import * as fs from 'fs';
import * as path from 'path';

import ProjectConfig from "./project-config";
import { tokenize } from './lexer';

export type FragmentType = "none" | "project" | "source"

export class Fragment {
    type: FragmentType = "none";
    path: string = "";
    preprocessed: boolean = false;
    parsed: boolean = false;
    root: Fragment | undefined;
}

export class Project extends Fragment {
    config: ProjectConfig;
    main: Source | undefined;

    constructor() {
        super()
        this.config = new ProjectConfig()
        this.type = "project";
    }

    fromDirectory(dir: string) {
        this.config.fromDirectory(dir);
        this.config.main = path.join(dir, this.config.main);

        this.main = new Source();
        this.main.fromFile(this.config.main);

        console.log(tokenize(this.main.text, this.config.main));
        
    }
}

export class Source extends Fragment {
    project: Project | undefined;
    name: string = "";
    text: string = "";
    deps: Fragment[] = [];

    constructor() {
        super();
        this.type = "source";
    }

    fromFile(path: string) {
        if (fs.existsSync(path)) {
            this.text = fs.readFileSync(path).toString()
        }
        else {
            console.log("puto no file found");

        }
    }
}