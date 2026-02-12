#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "lexer.h"

typedef struct {
    Token current;
    Token previous;
    bool hadError;
    bool panicMode;
} Parser;

static Parser parser;

// --- 1. Базовая диагностика и навигация ---

static void errorAt(Token* token, const char* message) {
    if (parser.panicMode) return;
    parser.panicMode = true;

    fprintf(stderr, "[line %d] Error", token->line);
    if (token->type == TOKEN_EOF) {
        fprintf(stderr, " at end");
    } else if (token->type == TOKEN_ERROR) {
  
    } else {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }

    fprintf(stderr, ": %s\n", message);
    parser.hadError = true;
}


static void errorAtCurrent(const char* message) {
    errorAt(&parser.current, message);
}

static void advance() {
    parser.previous = parser.current;
    for (;;) {
        parser.current = getToken();
        if (parser.current.type != TOKEN_ERROR) break;
        errorAtCurrent(parser.current.start);
    }
}

static void consume(TokenType type, const char* message) {
    if (parser.current.type == type) {
        advance();
        return;
    }
    errorAtCurrent(message);
}

static bool check(TokenType type) {
    return parser.current.type == type;
}

static bool match(TokenType type) {
    if (!check(type)) return false;
    advance();
    return true;
}

// --- 2. Низкоуровневые правила (Типы и Выражения) ---

// pаглушка для выражений (нужн чтобы varDeclaration работал)
static void expression() {
    // Пока просто "съедаем" один токен (например, число или строку)
    advance();
}

static void parseType() {
    bool isReference = false;
    if (match(TOKEN_AMPERSAND)) {
        isReference = true;
    }

    if (parser.current.type >= TOKEN_I8 && parser.current.type <= TOKEN_STR) {
        advance();
        // Здесь можно добавить логику сохранения типа
    } else {
        errorAtCurrent("Expect type after ':' or '&'.");
    }
}

// --- 3. Синхронизация (Аварийный сброс) ---

static void synchronize() {
    parser.panicMode = false;

    while (parser.current.type != TOKEN_EOF) {
        if (parser.previous.type == TOKEN_SEMICOLON) return;

        switch (parser.current.type) {
            case TOKEN_LET:
            case TOKEN_FN:
            case TOKEN_MUT:
            case TOKEN_IF:
            case TOKEN_RETURN:
                return;
            default:
                ; // Игнорируем
        }
        advance();
    }
}

// --- 4. Объявления и Инструкции ---

static void statement() {
    expression();
    consume(TOKEN_SEMICOLON, "Expect ';' after expression.");
}

static void varDeclaration() {
    bool isMutable = false;

    // 1. Проверяем наличие 'mut' сразу после того, как в main() нашли 'let'
    if (match(TOKEN_MUT)) {
        isMutable = true;
    }

    // 2. Дальше всё как обычно: имя, двоеточие, тип
    consume(TOKEN_IDENTIFIER, "Expect variable name.");
    Token name = parser.previous;

    consume(TOKEN_COLON, "Expect ':' after variable name.");
    parseType(); 

    consume(TOKEN_EQUAL, "Ghost requires explicit initialization. Use '=' followed by a value or 'Uninit'.");

    bool isUninit = false;
    if (match(TOKEN_UNINITIALIZED)) {
        isUninit = true;
    } else {
        if (check(TOKEN_SEMICOLON) || check(TOKEN_EOF)) {
            errorAtCurrent("Variable value cannot be empty.");
        } else {
            expression(); 
        }
    }

    consume(TOKEN_SEMICOLON, "Expect ';' after variable declaration.");

    // 3. Вывод с учетом мутабельности
    if (!parser.panicMode) {
        const char* mutStr = isMutable ? "[Mutable]" : "[Immutable]";
        if (isUninit) {
            printf("Ghost: %s Variable '%.*s' set to Uninit.\n", mutStr, name.length, name.start);
        } else {
            printf("Ghost: %s Variable '%.*s' initialized.\n", mutStr, name.length, name.start);
        }
    }
}

static void declaration() {
    if (match(TOKEN_LET)) {
        varDeclaration();
    } else {
        statement();
    }

    if (parser.panicMode) synchronize();
}

// --- 5. Вход в Парсер ---

void parse(const char* source) {
    initScanner(source);
    parser.hadError = false;
    parser.panicMode = false;

    advance(); 

    while (!match(TOKEN_EOF)) {
        declaration();
    }
}