/*
    I am really tired of this dependency madness, let's build a rock, it's temporary anyway
*/

import * as util from 'util';

import * as fs from 'fs';
import * as path from 'path';
import semver from 'semver';
import { projectError, resolverError, syntaxError } from './errors';
import { resolveGlobal } from '../trash/temp/glyn-y';

export type FragmentType = "none" | "project" | "source"
export type TokenKind = 'none' | 'identifier' | 'keyword' | 'number' | 'string' | 'boolean' | 'operator' | 'other'
export type TokenEntry = { [key: string]: {code?: number, kind?: TokenKind, explanable?: boolean, paren?: boolean, opening?: boolean, alternate?: number} };

export const CONFIG_NAME = "config.json";

export const TokenName = {
    co_false: 0,
    co_true: 1,
    co_infinity: 2,

    eol: 8,
    colon: 9,

    kw_import: 10,
    kw_namespace: 20,
    kw_function: 30,
    kw_class: 40,
    kw_var: 50,
    kw_as: 60,
    kw_return: 60,

    brace_open: 500,
    brace_close: 501,
    paren_open: 502,
    paren_close: 503,
    square_open: 504,
    square_close: 505,

    op_plus: 1000,
    op_plusEqual: 1001,

    op_less: 1010,
    op_lshift: 1011,
    op_lshiftEqual: 1012,
}

export const TOKEN_MAP: TokenEntry = {
    true: {code: TokenName.co_true, kind: 'boolean'},
    false: {code: TokenName.co_false, kind: 'boolean'},
    
    import: {code: TokenName.kw_import, kind: 'keyword'},
    as: {code: TokenName.kw_as, kind: 'keyword'},
    namespace: {code: TokenName.kw_namespace, kind: 'keyword', explanable: true},
    function: {code: TokenName.kw_function, kind: 'keyword', explanable: true},
    class: {code: TokenName.kw_class, kind: 'keyword', explanable: true},
    var: {code: TokenName.kw_var, kind: 'keyword', explanable: true},
    return: {code: TokenName.kw_return, kind: 'keyword', explanable: true},
}

export const OPERATOR_MAP: TokenEntry = {
    "{": {code: TokenName.brace_open, kind: 'other', paren: true, opening: true},
    "}": {code: TokenName.brace_close, kind: 'other', paren: true, opening: false, alternate: TokenName.brace_open},
    "(": {code: TokenName.paren_open, kind: 'other', paren: true, opening: true},
    ")": {code: TokenName.paren_close, kind: 'other', paren: true, opening: false, alternate: TokenName.paren_open},
    "[": {code: TokenName.square_open, kind: 'other', paren: true, opening: true},
    "]": {code: TokenName.square_close, kind: 'other', paren: true, opening: false, alternate: TokenName.square_open},

    ";": {code: TokenName.eol, kind: 'other'},
    ":": {code: TokenName.colon, kind: 'other'},

    "+": {code: TokenName.op_plus, kind: 'operator'},
    "+=": {code: TokenName.op_plusEqual, kind: 'operator'},

    "<": {code: TokenName.op_less, kind: 'operator'},
    "<<": {code: TokenName.op_lshift, kind: 'operator'},
    "<<=": {code: TokenName.op_lshiftEqual, kind: 'operator'},
}

export const LEGAL_EXTRA_CHARS = new Set([ '+', '<', '=', ';', ':', '[', ']', '{', '}', '(', ')' ])

export class Token
{
    kind: TokenKind = 'none';
    code?: number = TokenName.kw_namespace;
    text: string = "";
    paren?: boolean = false;
    closingPair?: number = 0;
    canHaveDocs?: boolean = false;

    line?: number = 0;
    column?: number = 0;
    index?: number = 0;
}

function isSubpath(childPath: string, parentPath: string): boolean {
    const resolvedChild = path.resolve(childPath);
    const resolvedParent = path.resolve(parentPath);

    return resolvedChild.startsWith(resolvedParent + path.sep);
}

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
            this.tokenize();
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

        // console.log("parsing ", this.path, this.type);

        // parse(this)
    }

    tokenize(): void {
        let currentChar = 0;
        let currentColumn = 0;
        let currentLine = 0;
        let docstring = "";
        this.tokens = [];
        let text: string | undefined = "";
    
        let bracketStack: { type: number, index: number }[] = []
    
        let munchTrailing = () => {
            let munchedAnything = false;
            while (currentChar < this.text.length && this.text.charCodeAt(currentChar) <= 32) {
                munchedAnything = true;
                if (this.text[currentChar] == '\n') {
                    currentColumn = 0;
                    currentLine++;
                }
                else {
                    currentColumn++;
                }
                currentChar++;
            }
    
            return munchedAnything;
        }
    
        let munchComment = (): string | undefined => {
            let munchedText = "";
    
            if (!(currentChar < this.text.length - 1 && this.text[currentChar] == '/' && this.text[currentChar + 1] == '*')) {
                return;
            }
    
            currentChar += 2
    
            while (currentChar < this.text.length - 1 /* Sufficient space for munching the cositos */ && !(this.text[currentChar + 1] == '/' && this.text[currentChar] == '*')) {
                munchedText += this.text[currentChar];
    
                if (this.text[currentChar] == '\n') {
                    currentColumn = 0;
                    currentLine++;
                }
                else {
                    currentColumn++;
                }
                currentChar++;
            }
    
            if (!(currentChar < this.text.length - 1)) {
                // unclosed trash
                syntaxError(`Unclosed comment at end of source`, currentChar, currentLine, currentColumn, this.text ?? "", this.path);
            }
    
            currentChar += 2
    
            return munchedText;
        }
    
        let munchNumber = (): string | undefined => {
            let munchedText = "";
    
            let counter = 0;
    
            while (currentChar < this.text.length) {
                if ((this.text.charCodeAt(currentChar) >= 48 && this.text.charCodeAt(currentChar) <= 57) || (counter && ['.', 'x', 'o', 'b'].includes(this.text[currentChar]))) {
                    munchedText += this.text[currentChar];
                    currentChar++;
                    counter++;
                    currentColumn++;
                }
                else {
                    break
                }
            }
    
            return munchedText;
        }
    
        let munchString = (): string | undefined => {
            let munchedText = "";
    
            if (!(currentChar < this.text.length && this.text[currentChar] == '"')) {
                return;
            }
    
            currentChar++;
    
            while (currentChar < this.text.length && !(this.text[currentChar] == '"')) {
                munchedText += this.text[currentChar];
    
                if (this.text[currentChar] == '\n') {
                    currentColumn = 0;
                    currentLine++;
                }
                else {
                    currentColumn++;
                }
                currentChar++;
            }
    
            if (!(currentChar < this.text.length)) {
                // unclosed trash
                syntaxError(`Unclosed string at end of source`, currentChar, currentLine, currentColumn, this.text ?? "", this.path);
            }
    
            currentChar++
    
            return munchedText;
        }
    
        let munchWord = (): string | undefined => {
            let munchedText = "";
    
            let counter = 0;
    
            while (currentChar < this.text.length) {
                if ((this.text.charCodeAt(currentChar) >= 65 && this.text.charCodeAt(currentChar) <= 90) || (this.text.charCodeAt(currentChar) >= 97 && this.text.charCodeAt(currentChar) <= 122) || (counter && (this.text.charCodeAt(currentChar) >= 48 && this.text.charCodeAt(currentChar) <= 57)) || this.text.charAt(currentChar) == '_') {
                    munchedText += this.text[currentChar];
                    currentChar++;
                    counter++;
                    currentColumn++;
                }
                else {
                    break
                }
            }
    
            return munchedText;
        }
    
        while (currentChar < this.text.length) {
            let lastPosition = currentChar;
    
            if (munchTrailing()) {
                // Do Nothing Xd
            }
            else if (text = munchComment()) {
                docstring = text;
                // Useful for docs or something of that wtfness
                // console.log(docstring);
            }
            else if (text = munchNumber()) {
                this.tokens.push({
                    kind: 'number',
                    text: text + "",
                    line: currentLine,
                    column: currentColumn,
                    index: currentChar
                })
            }
            else if (text = munchString()) {
                this.tokens.push({
                    kind: 'string',
                    text: text + "",
                    line: currentLine,
                    column: currentColumn,
                    index: currentChar
                })
            }
            else if (text = munchWord()) {
                let info: any = TOKEN_MAP[text];
    
                this.tokens.push({
                    kind: info?.kind || 'identifier',
                    text: text + "",
                    code: info?.code ?? -1,
                    line: currentLine,
                    column: currentColumn,
                    index: currentChar
                })
            }
            else {
                text = "";
                let info = undefined;
                let tempinfo = undefined;
    
                /**
                 * The green pipe black chained muncher (GPBCM)
                 */
    
                while (currentChar < this.text.length && LEGAL_EXTRA_CHARS.has(this.text[currentChar])) {
                    if ( (tempinfo = OPERATOR_MAP[text + this.text[currentChar]]) )
                    {
                        info = tempinfo;
                        text += this.text[currentChar]
                    }
                    else
                    {
                        break;
                    }
                    currentChar++;
                    currentColumn++;
                }
                
                if (text.length <= 0 || !info)
                {
                    syntaxError(`Illegal character sequence`, currentChar, currentLine, currentColumn, this.text ?? "", this.path);
                    return;
                }
    
                if (info.paren)
                {
                    if (info.opening)
                    {
                        bracketStack.push({ type: info.code ?? -1, index: this.tokens.length })
                    }
                    else
                    {
                        let lastBracket = bracketStack[ bracketStack.length - 1 ];
                        if (info.alternate != lastBracket?.type)
                        {
                            syntaxError(`Bracket closing mismatch`, currentChar, currentLine, currentColumn, this.text ?? "", this.path);
                        }
                        this.tokens[ lastBracket.index ].closingPair = this.tokens.length;
                        bracketStack.pop()
                    }
                }
    
                this.tokens.push({
                    kind: info.kind ?? 'none',
                    text: text + "",
                    code: info.code,
                    paren: !!info.paren,
                    line: currentLine,
                    column: currentColumn,
                    index: currentChar
                })
            }
        }
    }
}

export function build(dir: string)
{
    let project = new Project();

    project.fromDirectory(dir);

    console.log( util.inspect(project, {depth: 10, compact: false, colors: true}) );

    project.parse();
}

export function generate_docs(dir: string)
{
    let project = new Project();

    project.fromDirectory(dir);
}