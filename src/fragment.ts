import * as fs from 'fs';
import * as path from 'path';

import ProjectConfig from "./project-config";
import { tokenize } from './lexer';
import { Token, TokenKind, TokenName } from './token';
import { resolverError, syntaxError } from './errors';

import semver from 'semver';
import { resolveGlobal } from './glyn-y';
import { parse } from './parser';

function isSubpath(childPath: string, parentPath: string): boolean {
    const resolvedChild = path.resolve(childPath);
    const resolvedParent = path.resolve(parentPath);

    return resolvedChild.startsWith(resolvedParent + path.sep);
}

export type FragmentType = "none" | "project" | "source"

export class Fragment {
    type: FragmentType = "none";
    path: string = "";
    preprocessed: boolean = false;
    parsed: boolean = false;
    root: Fragment | undefined;

    preprocess() {
        this.preprocessed = true;
    }

    parse() {
        this.parsed = true;
    }
}

export class Project extends Fragment {
    config: ProjectConfig;
    main: Source | undefined;
    sources: { [key: string]: any } = {}

    constructor() {
        super()
        this.config = new ProjectConfig()
        this.type = "project";
    }

    fromDirectory(dir: string) {
        /* Load config.json */
        this.path = path.resolve(dir);
        this.config.fromDirectory(dir);
        /* Load main from file */
        this.main = new Source();
        this.main.fromFile(this.config.main);
        this.main.project = this;
        this.main.root = this.root || this;

        this.preprocess();
    }

    addFile(file: string) {
        let source: Source | undefined = undefined;
        if (source = this.sources[file]) {
            return source;
        }
        /* Load main from file */
        source = new Source();
        source.fromFile(file);
        this.sources[file] = source;
        source.project = this;
        source.root = this.root || this;
        source.preprocess();
        return source;
    }

    addProject(dir: string) {
        let project: Project | undefined = undefined
        if (project = this.sources[dir]) {
            return project;
        }
        project = new Project();
        project.root = this.root || this;
        project.fromDirectory(dir);
        this.sources[dir] = project;
        project.preprocess();
        return project;
    }

    preprocess() {
        super.preprocess();
        // console.log(".. project ..");
        this.main?.preprocess();
    }

    parse(): void {
        if (this.parsed)
        {
            return;
        }

        super.parse();

        this.main?.parse();
    }
}

export class Source extends Fragment {
    project: Project | undefined;
    name: string = "";
    text: string = "";
    deps: Fragment[] = [];
    tokens: Token[] = [];
    depAliases: { [key: string]: Fragment } = {};

    constructor() {
        super();
        this.type = "source";
    }

    fromFile(file: string) {
        if (fs.existsSync(file)) {
            this.path = path.resolve(file);
            this.name = path.basename(file);
            this.text = fs.readFileSync(file).toString();
            this.tokens = tokenize(this.text); /* Tokenize */
        }
        else {
            console.log("puto no file found");
        }
    }

    resolve(query: string) {
        // console.log(`Resolving ${query} from ${this.path}`);

        if (this.project == undefined || this.root == undefined) {
            resolverError("Wtf why the project that holds this source is null?", this.path);
            return;
        }

        if (!query) {
            resolverError("Expected a project or source name but found an empty query", this.path);
        }

        let tokens = query.split(':');
        let name = tokens[0].trim();
        let version = (tokens[1] ?? "").trim();

        let pathAsSource = path.join(path.dirname(this.path), name)
        let sourceExists = fs.existsSync(pathAsSource);

        /* 
            Even if we are importing a global library pathAsSource should result in a path inside the project (Even if it does not exist at all)
        */
        if (!isSubpath(pathAsSource, this.project.path)) {
            resolverError(`Trying to fetch sources outside of project (${pathAsSource})`, this.path);
        }

        if (sourceExists) {
            /*
                This could and will lead to 
                duplicate entries in the deps but don't worry
                so much. In fact, there are the parsed and 
                preprocessed flags for that.

                NOTA: parchese, en proyectos tu solo 
                importas el main debido al aislamineto,
                en efecto, el único beneficio o drawback 
                de la implementación
            */
            let fragment = this.project.addFile(pathAsSource);
            this.deps.push( fragment );
            return fragment;
        }

        let foundEntry = undefined;

        for (const entry of this.project.config.packages) {
            /*
            nV' + nv
            */
            if (entry.name == name && (version == "" || entry.version == version)) {
                /*
                    Specified version, don't do stupid things.
                    If not, pick the latest
                */
                if (!foundEntry || semver.lt(foundEntry.version, entry.version)) {
                    foundEntry = entry;
                }
                if (version) {
                    break;
                }
            }
        }

        if (!foundEntry)
        {
            resolverError(`Librery not found in config.json for this query "${query}"`, this.path);
            return;
        }

        let fragmentPath = resolveGlobal(foundEntry?.name, foundEntry?.version);

        if (!fragmentPath)
        {
            resolverError(`Librery not found for this query "${query}", you should install it`, this.path);
            return;
        }

        let fragment = (this.root as Project).addProject(fragmentPath);
        this.deps.push( fragment );
        return fragment;
    }

    preprocess() {
        super.preprocess();
        // console.log(".. source ..");

        let currentToken: Token | undefined;
        let newTokens: Token[] = [];
        let cursor = 0;

        let consume = (query: { kind?: TokenKind, code?: number }) => {
            let k = !!query.kind;
            let c = !!query.code;
            let K = currentToken?.kind == query.kind;
            let C = currentToken?.code == query.code;
            /*
                Para los jovenes de sistemas digitales... Pélelo, porque aquí usé un mapa carnoso.
            */
            if (
                (K && C) || (!k && C) || (!c && K)
            ) {
                cursor++;
                let old = currentToken;
                currentToken = this.tokens[cursor];
                return old;
            } else {
                return undefined;
            }
        }

        let expect = (query: { kind?: TokenKind, code?: number }) => {
            let token = consume({ kind: query.kind, code: query.code })
            if (token) {
                return token;
            }
            token = this.tokens[cursor];
            syntaxError("I am waiting for other thing to come here bruh", token.index ?? 0, token.line ?? 0, token.column ?? 0, this.text, this.path);
        }

        while (cursor < this.tokens.length) {
            currentToken = this.tokens[cursor];
            /* Recalculate closing pairs for the new array */
            if (currentToken.paren && currentToken.closingPair != undefined) {
                currentToken.closingPair += newTokens.length - cursor;
            }

            switch (currentToken.code) {
                case TokenName.kw_import: {
                    /* import */
                    expect({ code: TokenName.kw_import })
                    /* "x" */
                    let name = expect({ kind: 'string' });
                    // @ts-ignore
                    let importee: any = this.resolve(name.text);

                    /* ; */
                    if (consume({ code: TokenName.kw_as }))
                    {
                        let alias = expect({ kind: 'identifier' })?.text || "";

                        this.depAliases[alias] = importee;
                    }
                    consume({ code: TokenName.eol });
                } break;
                default:
                    newTokens.push(currentToken);
                    cursor++;
                    break;
            }
        }

        this.tokens = newTokens;
    }

    parse(): void {
        if (this.parsed)
        {
            return;
        }

        super.parse();

        for (const dependency of this.deps)
        {
            dependency.parse();
        }

        console.log("parsing ", this.path, this.type);

        parse(this)
    }
}