import * as fs from 'fs';
import * as path from 'path';
import { fileURLToPath } from 'url';

import * as glyn from "./glyn"

import yargs from 'yargs';
import { hideBin } from 'yargs/helpers';

const FILENAME = fileURLToPath(import.meta.url);
const DIRNAME = path.dirname(FILENAME);
const GLYN_HOME = process.env.GLYN_HOME;

const args: any = yargs(hideBin(process.argv))
    .command("build", "Builds a given project to an intermediate form")
    .option('dir', {
        alias: 'd',
        type: 'string',
        describe: 'Directorio de entrada',
        demandOption: true
    })
    // .option('emit-ast', {
    //     alias: 'a',
    //     type: 'boolean',
    //     describe: 'Mostrar AST intermedio'
    // })
    .argv;

function print_banner() {
    console.log("🌙 [bold yellow] Thanks for using the luna compiler [/bold yellow] - [bold magenta] Your most adrade resource for making simple things. [/bold magenta]🌙\n")
    console.log("[[bold purple]🧪 v0.0 [/bold purple]] If you have an interesting idea for improving this, or want to see other projects check out this! \n")
    console.log("[yellow]GitHub:[/yellow] https://github.com/Alex461538/glyn-python ")
    console.log("[bold red]Yt:[/bold red] https://youtube.com/@pianitas38 ")
    console.log("")
}

if (args._.includes("build")) {
    const TRUE_PROJECT_DIRECTORY = path.join(process.cwd(), args.dir);

    glyn.build(TRUE_PROJECT_DIRECTORY);
}