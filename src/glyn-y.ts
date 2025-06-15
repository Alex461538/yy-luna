import * as fs from 'fs';
import * as path from 'path';

import semver from 'semver';

const GLYN_NAME = "glyn-y.json"
const GLYN_HOME = process.env.GLYN_HOME;

export function resolveGlobal(name: string, version: string)
{
    if (!GLYN_HOME)
    {
        throw "GLYN_HOME is not defined";
    }

    const data = JSON.parse(fs.readFileSync( path.join(GLYN_HOME, GLYN_NAME) ).toString())

    for (const entry of data.packages) {
        if (entry.name == name && semver.satisfies(version, entry.version))
        {
            let absPath = path.isAbsolute(entry.path) ? entry.path : path.join(GLYN_HOME, entry.path);
            return absPath;
        }
    }
}