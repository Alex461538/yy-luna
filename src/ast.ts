export const NodeType = {
    none: -1,
    root: 0,
    namespace: 1,
    function: 2,
    class: 3
}

// export const SEARCH_MAP: { [key: number]: Set<number> } = {}
// SEARCH_MAP[NodeType.namespace] = new Set<number>([ NodeType.function, NodeType.class ]);

export class AstNode {
    type: number = NodeType.none;
    name?: string = "";
    completeMember?: boolean;
    parameters?: AstNode[];
    returnType?: any;

    children: AstNode[];

    constructor() {
        this.children = [];
    }

    addChild(node: AstNode) {
        this.children.push(node);
    }
}