#include "compiler.h"

#include "common.h"
#include "scanner.h"

void compile(const char* source) {
    initScanner(source);
    int line = -1;
    for (;;) {
        Token token = scanToken();
        if (token.line != line) {
            printf("%04d ", token.line);
            line = token.line;
        } else {
            printf("  |  ");
        }
        printf("%-19s '%.*s'\n", getTokenTypeName(token.type), token.length,
               token.start);

        if (token.type == TOKEN_EOF) break;
    }
}