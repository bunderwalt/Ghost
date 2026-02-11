#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

// --- 1. Состояние и базовые проверки ---

static Scanner ghostScanner; 

static bool isAtEnd() {
    return *ghostScanner.current == '\0';
}

static bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

// --- 2. Навигация по исходному коду ---

static char peek() {
    return *ghostScanner.current;
}

static char peekNext() {
    if (isAtEnd()) return '\0';
    return ghostScanner.current[1];
}

static char advance() {
    char c = *ghostScanner.current;
    ghostScanner.current++;
    if (c == '\n') ghostScanner.line++;
    return c;
}

static bool match(char expected) {
    if (isAtEnd()) return false;
    if (*ghostScanner.current != expected) return false;
    ghostScanner.current++;
    return true;
}

// --- 3. Создание токенов (Сначала ошибки, потом обычные) ---

// Теперь errorToken стоит ВЫШЕ всех, кто его использует
static Token errorToken(const char* message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = ghostScanner.line;
    token.literal.as_int = 0;
    return token;
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

// --- 4. Логика распознавания слов и строк ---

static TokenType checkKeyword(int start, int length, const char* rest, TokenType type) {
    if (ghostScanner.current - ghostScanner.start == start + length &&
        memcmp(ghostScanner.start + start, rest, length) == 0) {
        return type;
    }
    return TOKEN_IDENTIFIER;
}

static TokenType identifierType() {
    int length = (int)(ghostScanner.current - ghostScanner.start);
    
    switch (ghostScanner.start[0]) {
        case 'b': return checkKeyword(1, 3, "ool", TOKEN_BOOL);
        case 'l': return checkKeyword(1, 2, "et", TOKEN_LET);
        case 'm': return checkKeyword(1, 2, "ut", TOKEN_MUT);
        case 's': return checkKeyword(1, 2, "tr", TOKEN_STR);
        case 'f': 
            if (length > 1) {
                switch (ghostScanner.start[1]) {
                    case 'n': return TOKEN_FN;
                    case '8': return (length == 2) ? TOKEN_F8 : TOKEN_IDENTIFIER;
                    case '1': return checkKeyword(2, 1, "6", TOKEN_F16);
                    case '3': return checkKeyword(2, 1, "2", TOKEN_F32);
                }
            }
            break;
        case 'i':
            if (length > 1) {
                switch (ghostScanner.start[1]) {
                    case '8': return (length == 2) ? TOKEN_I8 : TOKEN_IDENTIFIER;
                    case '1': return checkKeyword(2, 1, "6", TOKEN_I16);
                    case '3': return checkKeyword(2, 1, "2", TOKEN_I32);
                }
            }
            break;
    }
    return TOKEN_IDENTIFIER;
}

static Token string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') ghostScanner.line++;
        advance();
    }

    if (isAtEnd()) return errorToken("Unterminated string.");

    advance(); // Закрывающая кавычка
    return makeToken(TOKEN_STRING_LITERAL);
}

// --- 5. Основной цикл ---

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

Token getToken() {
    skipWhiteSpace();
    ghostScanner.start = ghostScanner.current;

    if (isAtEnd()) return makeToken(TOKEN_EOF);

    char c = advance();

    if (isDigit(c)) {
        while (isDigit(peek())) advance();
        return makeToken(TOKEN_NUMBER);
    }

    if (isAlpha(c)) {
        while (isAlpha(peek()) || isDigit(peek())) advance();
        return makeToken(identifierType());
    }

    switch (c) {
        case '+': return makeToken(TOKEN_PLUS);
        case '-': return makeToken(TOKEN_MINUS);
        case '*': return makeToken(TOKEN_STAR);
        case '/': return makeToken(TOKEN_SLASH);
        case '%': return makeToken(TOKEN_PERCENT);
        case ';': return makeToken(TOKEN_SEMICOLON);
        case ':': return makeToken(TOKEN_COLON);
        case '&': return makeToken(TOKEN_AMPERSAND);
        case '"': return string();
        
        case '>': return makeToken(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        case '<': return makeToken(match('=') ? TOKEN_LESS_EQUAL    : TOKEN_LESS);
        case '=': return makeToken(match('=') ? TOKEN_EQ_EQ         : TOKEN_EQUAL);
        case '!': return makeToken(match('=') ? TOKEN_BANG_EQ       : TOKEN_BANG);
    }

    return errorToken("Unexpected character.");
}

// --- 6. Утилиты (readFile и getTokenName) ---

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
        case TOKEN_I8: return "I8";
        case TOKEN_I16: return "I16";
        case TOKEN_I32: return "I32";
        case TOKEN_F8: return "F8";
        case TOKEN_F16: return "F16";
        case TOKEN_F32: return "F32";
        case TOKEN_STR: return "STR";
        case TOKEN_COLON: return "COLON";
        case TOKEN_AMPERSAND: return "AMPERSAND";
        case TOKEN_STRING_LITERAL: return "STRING_LITERAL";
        case TOKEN_LET: return "LET";
        case TOKEN_MUT: return "MUT";
        case TOKEN_FN:  return "FN";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_NUMBER:     return "NUMBER";
        case TOKEN_PLUS:       return "PLUS";
        case TOKEN_EQUAL:      return "EQUAL";
        case TOKEN_SEMICOLON:  return "SEMICOLON";
        case TOKEN_EOF:        return "EOF";
        case TOKEN_ERROR:      return "ERROR";
        default:               return "OTHER";
    }
}