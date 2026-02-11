#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

// Внутреннее состояние лексера (инкапсуляция)

static Scanner ghostScanner; 


static bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

static bool isAtEnd() {
    return *ghostScanner.current == '\0';
}

// Сдвигает курсор и возвращает символ
static char advance() {
    char c = *ghostScanner.current;
    ghostScanner.current++;
    if (c == '\n') ghostScanner.line++;
    return c;
}

// Смотрит на текущий символ, не сдвигаясь
static char peek() {
    return *ghostScanner.current;
}

// Смотрит на следующий символ
static char peekNext() {
    if (isAtEnd()) return '\0';
    return ghostScanner.current[1];
}

// --- 2. Логика сканирования ---

void initScanner(const char* source) {
    ghostScanner.start = source;
    ghostScanner.current = source;
    ghostScanner.line = 1;
}

void skipWhiteSpace() {
    for (;;) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
            case '\n':
                advance();
                break;
            case '/':
                if (peekNext() == '/') {
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

static Token makeToken(TokenType type) {
    Token t;
    t.type = type;
    t.start = ghostScanner.start;
    t.length = (int)(ghostScanner.current - ghostScanner.start);
    t.line = ghostScanner.line;
    t.literal.as_int = 0;
    return t;
}

static TokenType checkKeyword(int start, int length, const char* rest, TokenType type) {
    if (ghostScanner.current - ghostScanner.start == start + length &&
        memcmp(ghostScanner.start + start, rest, length) == 0) {
        return type;
    }
    return TOKEN_IDENTIFIER;
}

static TokenType identifierType() {
    switch (ghostScanner.start[0]) {
        case 'f': return checkKeyword(1, 1, "n", TOKEN_FN);
        case 'l': return checkKeyword(1, 2, "et", TOKEN_LET);
        case 'm': return checkKeyword(1, 2, "ut", TOKEN_MUT);
        default:  return TOKEN_IDENTIFIER;
    }
}

static bool match(char expected) {
    if (*ghostScanner.current != expected) return false;
    ghostScanner.current++; 
    return true;
}

Token getToken() {
    skipWhiteSpace();
    ghostScanner.start = ghostScanner.current;

    if (isAtEnd()) return makeToken(TOKEN_EOF);

    char c = advance();


    if (isDigit(c)) {
        int value = c - '0';
        while (isDigit(peek())) {
            value = value * 10 + (advance() - '0');
        }
        Token t = makeToken(TOKEN_NUMBER);
        t.literal.as_int = value;
        return t;
    }

    // Идентификаторы
    if (isAlpha(c)) {
        while (isAlpha(peek()) || isDigit(peek())) advance();
        return makeToken(identifierType());
    }

    // Операторы
    switch (c) {
        case '+': return makeToken(TOKEN_PLUS);
        case '-': return makeToken(TOKEN_MINUS);
        case '*': return makeToken(TOKEN_STAR);
        case '/': return makeToken(TOKEN_SLASH);
        case '%': return makeToken(TOKEN_PERCENT);
        case ';': return makeToken(TOKEN_SEMICOLON);
        case '>': return makeToken(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        case '<': return makeToken(match('=') ? TOKEN_LESS_EQUAL    : TOKEN_LESS);
        case '=': return makeToken(match('=') ? TOKEN_EQ_EQ         : TOKEN_EQUAL);
        case '!': return makeToken(match('=') ? TOKEN_BANG_EQ       : TOKEN_BANG);
    }

    return makeToken(TOKEN_ERROR);
}

// --- 3. Утилиты ---

char* readFile(const char* path) {
    FILE* file = fopen(path, "rb");
    if (!file) {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(74);
    }
    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);
    char* buffer = (char*)malloc(fileSize + 1);
    fread(buffer, sizeof(char), fileSize, file);
    buffer[fileSize] = '\0';
    fclose(file);
    return buffer;
}

const char* getTokenName(TokenType type) {
    switch (type) {
        case TOKEN_LET: return "LET";
        case TOKEN_MUT: return "MUT";
        case TOKEN_FN:  return "FN";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_NUMBER:     return "NUMBER";
        case TOKEN_PLUS:       return "PLUS";
        case TOKEN_EQUAL:      return "EQUAL";
        case TOKEN_SEMICOLON:  return "SEMICOLON";
        case TOKEN_EOF:        return "EOF";
        default:               return "OTHER";
    }
}