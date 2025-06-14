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
    process.exit(1);
}

export function lexerError(message: string, currentChar: number, currentLine: number, currentColumn: number, code: string, sourceFile: string = "<unknown>") {
    const WINDOW_SIZE = 20;
    const text = code.substring(currentChar - WINDOW_SIZE, currentChar + WINDOW_SIZE / 2);

    console.error(styleText(['magenta', 'bold'], `Syntax error:  ${message}`));

    console.error(text)
    console.error(styleText(["yellow", "bold"], "^^^".padStart(currentColumn, " ")))

    console.error(styleText(['magenta', 'bold'], `At: ${sourceFile} -- line: ${currentLine+1} column: ${currentColumn+1}`));

    process.exit(1);
}