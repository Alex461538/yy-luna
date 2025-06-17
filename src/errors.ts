import { styleText } from 'node:util';
import { stderr } from 'node:process';

export function projectError(message: string) {
    const errorMessage = styleText(
        ['magenta', 'bold'],
        `Project error:  ${message}`,
        // Validate if process.stderr has TTY
        { stream: stderr },
    );
    console.error(errorMessage);
    throw "exit";
}

export function syntaxError(message: string, currentChar: number, currentLine: number, currentColumn: number, code: string, sourceFile: string = "<unknown>") {
    const WINDOW_SIZE = 30;
    const text = code.substring(currentChar - WINDOW_SIZE, currentChar + WINDOW_SIZE / 4);

    console.error(styleText(['magenta', 'bold'], `Syntax error:  ${message}`));

    console.error(text)
    console.error(styleText(["yellow", "bold"], "^^^".padStart(currentColumn, " ")))

    console.error(styleText(['magenta', 'bold'], `At: ${sourceFile} -- line: ${currentLine+1} column: ${currentColumn+1}`));

    throw "exit";
}

export function resolverError(message: string, sourceFile: string = "<unknown>") {
    console.error(styleText(['magenta', 'bold'], `Resolver error:  ${message}`));
    console.error(styleText(['magenta', 'bold'], `From: ${sourceFile}`));

    throw "exit";
}

export function parserError(message: string, currentChar: number, currentLine: number, currentColumn: number, code: string, sourceFile: string = "<unknown>") {
    const WINDOW_SIZE = 30;
    const text = code.substring(currentChar - WINDOW_SIZE, currentChar + WINDOW_SIZE / 4);

    console.error(styleText(['magenta', 'bold'], `Parser error:  ${message}`));

    console.error(text)
    console.error(styleText(["yellow", "bold"], "^^^".padStart(currentColumn, " ")))

    console.error(styleText(['magenta', 'bold'], `At: ${sourceFile} -- line: ${currentLine+1} column: ${currentColumn+1}`));

    throw "exit";
}