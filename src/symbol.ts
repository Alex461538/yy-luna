import { parserError } from "./errors";

export const SymbolType = {
    none: -1,
    root: 0,
    namespace: 1,
    function: 2,
    class: 3
}

export function isCompletable(type: number) {
    return type == SymbolType.class || type == SymbolType.function;
}

export class Symbol {
    type: number = SymbolType.none;
    name?: string = "";
    completeMember?: boolean;
    parameters?: Symbol[];
    returnType?: any;

    fields: Record<string, Symbol> = {};
    children: Symbol[] = [];

    parent?: Symbol;

    index?: number = 0;
    line?: number = 0;
    column?: number = 0;

    define(name: string, node: Symbol) {
        console.log("defining ", name);

        let entry = this.fields[name];

        if (entry) {
            if (isCompletable(entry.type) && !entry.completeMember) {
                entry.completeMember = true;
            }
            else {
                parserError(`Redefinition of ${entry.name}`, this.index!, this.line!, this.column!, "", "");
            }
        }

        this.fields[name] = node;
    }

    find(name: string): Symbol | null {
        if (this.fields[name])
        {
            return this.fields[name];
        }
        return this.parent?.find(name) || null;
    }

    findFromPath(path: string[]): Symbol | null {
        let current: Symbol = this;
        for (const part of path) {
            current = current.fields[part];
            if (!current) return null;
        }
        return current;
    }
}

// // export const SEARCH_MAP: { [key: number]: Set<number> } = {}
// // SEARCH_MAP[NodeType.namespace] = new Set<number>([ NodeType.function, NodeType.class ]);

// export class AstNode {


//     children: AstNode[];

//     constructor() {
//         this.children = [];
//     }

//     addChild(node: AstNode) {
//         this.children.push(node);
//     }
// }