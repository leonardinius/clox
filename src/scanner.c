
#include "scanner.h"

#include <string.h>

#include "common.h"

typedef struct {
    const char* start;
    const char* current;
    int line;
} Scanner;

Scanner scanner;

void initScanner(const char* source) {
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

static bool isDigit(char c) { return c >= '0' && c <= '9'; }

static bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static Token makeToken(TokenType type) {
    Token token;
    token.type = type;
    token.start = scanner.start;
    token.length = (int)(scanner.current - scanner.start);
    token.line = scanner.line;
    return token;
}

static Token errorToken(const char* message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = scanner.line;
    return token;
}

static char advance() {
    scanner.current++;
    return scanner.current[-1];
}

static char peek() { return *scanner.current; }

static bool isAtEnd() { return *scanner.current == '\0'; }

static char peekNext() {
    if (isAtEnd()) return '\0';
    return scanner.current[1];
}

static void skipWhitespace() {
    for (;;) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;

            case '\n':
                scanner.line++;
                advance();
                break;

            case '/':
                if (peekNext() == '/') {
                    // A comment goes until the end of the line.
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else {
                    return;
                }
                break;

            default:
                return;
        }
    }
}

static bool match(char expected) {
    if (isAtEnd()) return false;
    if (*scanner.current != expected) return false;

    scanner.current++;
    return true;
}

static Token string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') scanner.line++;
        advance();
    }

    if (isAtEnd()) return errorToken("Unterminated string.");

    // The closing quote.
    advance();
    return makeToken(TOKEN_STRING);
}

static Token number() {
    while (isDigit(peek())) {
        advance();
    }

    if (peek() == '.' && isDigit(peekNext())) {
        advance();
        while (isDigit(peek())) {
            advance();
        }
    }

    return makeToken(TOKEN_NUMBER);
}

static TokenType checkKeyword(int start, int length, const char* rest,
                              TokenType type) {
    if (scanner.current - scanner.start == start + length &&
        memcmp(scanner.start + start, rest, length) == 0) {
        return type;
    }

    return TOKEN_IDENTIFIER;
}

static TokenType identifierType() {
    switch (scanner.start[0]) {
        case 'a':
            return checkKeyword(1, 2, "nd", TOKEN_AND);
        case 'c':
            return checkKeyword(1, 4, "lass", TOKEN_CLASS);
        case 'e':
            return checkKeyword(1, 3, "lse", TOKEN_ELSE);
        case 'f':
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 'a':
                        return checkKeyword(2, 3, "lse", TOKEN_FALSE);
                    case 'o':
                        return checkKeyword(2, 1, "r", TOKEN_FOR);
                    case 'u':
                        return checkKeyword(2, 1, "n", TOKEN_FUN);
                }
            }
            break;
        case 't':
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 'h':
                        return checkKeyword(2, 2, "is", TOKEN_THIS);
                    case 'r':
                        return checkKeyword(2, 2, "ue", TOKEN_TRUE);
                }
            }
            break;
        case 'i':
            return checkKeyword(1, 1, "f", TOKEN_IF);
        case 'n':
            return checkKeyword(1, 2, "il", TOKEN_NIL);
        case 'o':
            return checkKeyword(1, 1, "r", TOKEN_OR);
        case 'p':
            return checkKeyword(1, 4, "rint", TOKEN_PRINT);
        case 'r':
            return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
        case 's':
            return checkKeyword(1, 4, "uper", TOKEN_SUPER);
        case 'v':
            return checkKeyword(1, 2, "ar", TOKEN_VAR);
        case 'w':
            return checkKeyword(1, 4, "hile", TOKEN_WHILE);
    }

    return TOKEN_IDENTIFIER;
}

static Token identifier() {
    while (isAlpha(peek()) || isDigit(peek())) {
        advance();
    }

    return makeToken(identifierType());
}

Token scanToken() {
    skipWhitespace();
    scanner.start = scanner.current;

    if (isAtEnd()) return makeToken(TOKEN_EOF);

    char c = advance();

    if (isAlpha(c)) return identifier();
    if (isDigit(c)) return number();

    switch (c) {
        case '(':
            return makeToken(TOKEN_LEFT_PAREN);
        case ')':
            return makeToken(TOKEN_RIGHT_PAREN);
        case '{':
            return makeToken(TOKEN_LEFT_BRACE);
        case '}':
            return makeToken(TOKEN_RIGHT_BRACE);
        case ';':
            return makeToken(TOKEN_SEMICOLON);
        case ',':
            return makeToken(TOKEN_COMMA);
        case '.':
            return makeToken(TOKEN_DOT);
        case '-':
            return makeToken(TOKEN_MINUS);
        case '+':
            return makeToken(TOKEN_PLUS);
        case '/':
            return makeToken(TOKEN_SLASH);
        case '*':
            return makeToken(TOKEN_STAR);
        case '!':
            return makeToken(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '=':
            return makeToken(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        case '<':
            return makeToken(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        case '>':
            return makeToken(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        case '"':
            return string();
    }

    return errorToken("Unexpected character.");
}

const char* getTokenTypeName(TokenType tokenType) {
    const char* name = NULL;
    switch (tokenType) {
            // Single-character tokens.
        case TOKEN_LEFT_PAREN:
            name = "TOKEN_LEFT_PAREN";
            break;
        case TOKEN_RIGHT_PAREN:
            name = "TOKEN_RIGHT_PAREN";
            break;
        case TOKEN_LEFT_BRACE:
            name = "TOKEN_LEFT_BRACE";
            break;
        case TOKEN_RIGHT_BRACE:
            name = "TOKEN_RIGHT_BRACE";
            break;
        case TOKEN_COMMA:
            name = "TOKEN_COMMA";
            break;
        case TOKEN_DOT:
            name = "TOKEN_DOT";
            break;
        case TOKEN_MINUS:
            name = "TOKEN_MINUS";
            break;
        case TOKEN_PLUS:
            name = "TOKEN_PLUS";
            break;
        case TOKEN_SEMICOLON:
            name = "TOKEN_SEMICOLON";
            break;
        case TOKEN_SLASH:
            name = "TOKEN_SLASH";
            break;
        case TOKEN_STAR:
            name = "TOKEN_STAR";
            break;
            // One or two character tokens.
        case TOKEN_BANG:
            name = "TOKEN_BANG";
            break;
        case TOKEN_BANG_EQUAL:
            name = "TOKEN_BANG_EQUAL";
            break;
        case TOKEN_EQUAL:
            name = "TOKEN_EQUAL";
            break;
        case TOKEN_EQUAL_EQUAL:
            name = "TOKEN_EQUAL_EQUAL";
            break;
        case TOKEN_GREATER:
            name = "TOKEN_GREATER";
            break;
        case TOKEN_GREATER_EQUAL:
            name = "TOKEN_GREATER_EQUAL";
            break;
        case TOKEN_LESS:
            name = "TOKEN_LESS";
            break;
        case TOKEN_LESS_EQUAL:
            name = "TOKEN_LESS_EQUAL";
            break;
            // Literals.
        case TOKEN_IDENTIFIER:
            name = "TOKEN_IDENTIFIER";
            break;
        case TOKEN_STRING:
            name = "TOKEN_STRING";
            break;
        case TOKEN_NUMBER:
            name = "TOKEN_NUMBER";
            break;
            // Keywords.
        case TOKEN_AND:
            name = "TOKEN_AND";
            break;
        case TOKEN_CLASS:
            name = "TOKEN_CLASS";
            break;
        case TOKEN_ELSE:
            name = "TOKEN_ELSE";
            break;
        case TOKEN_FALSE:
            name = "TOKEN_FALSE";
            break;
        case TOKEN_FOR:
            name = "TOKEN_FOR";
            break;
        case TOKEN_FUN:
            name = "TOKEN_FUN";
            break;
        case TOKEN_IF:
            name = "TOKEN_IF";
            break;
        case TOKEN_NIL:
            name = "TOKEN_NIL";
            break;
        case TOKEN_OR:
            name = "TOKEN_OR";
            break;
        case TOKEN_PRINT:
            name = "TOKEN_PRINT";
            break;
        case TOKEN_RETURN:
            name = "TOKEN_RETURN";
            break;
        case TOKEN_SUPER:
            name = "TOKEN_SUPER";
            break;
        case TOKEN_THIS:
            name = "TOKEN_THIS";
            break;
        case TOKEN_TRUE:
            name = "TOKEN_TRUE";
            break;
        case TOKEN_VAR:
            name = "TOKEN_VAR";
            break;
        case TOKEN_WHILE:
            name = "TOKEN_WHILE";
            break;
            // End of scan
        case TOKEN_ERROR:
            name = "TOKEN_ERROR";
            break;
        case TOKEN_EOF:
            name = "TOKEN_EOF";
            break;

        default:
            name = "TOKEN_UNKNOWN";
            break;
    }

    return name;
}