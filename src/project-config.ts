import path from "path";
import fs from "fs";
import { projectError } from "./errors";

const CONFIG_NAME = "config.json";

export default class ProjectConfig {
    name: string = "";
    version: string = "";
    description: string = "";
    keywords: string[] = [];
    license: string = "";
    author: string = "";
    main: string = "";
    packages: { name: string, version: string }[] = [];
    build: any;

    fromDirectory(dir: string) {
        const fullPath = path.join(dir, CONFIG_NAME);

        if (fs.existsSync(fullPath)) {
            const content = fs.readFileSync(fullPath).toString();

            try {
                const data = JSON.parse(content);

                this.name = data.name
                this.version = data.version
                this.description = data.description;
                this.keywords = data.keywords;
                this.license = data.license;
                this.author = data.author;
                this.main = data.main;
                this.packages = data.packages;
                this.build = data.build ?? {};

                this.main = path.join(dir, this.main); /* Resolve path for main */
            } catch (error) {
                projectError(`Configuration files are corrupt: ${fullPath}`)
            }
        }
        else {
            projectError(`Configuration not found for this project: ${fullPath}`)
        }
    }
}