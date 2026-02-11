#ifndef GHOST_COMMON_H
#define GHOST_COMMON_H

#include <stdbool.h>

// Максимальная длина идентификатора (если потребуется буферизация)
#define MAX_IDENTIFIER_LEN 31

// 1. Перечисление типов токенов
// Используем префикс TOKEN_ для ВСЕХ элементов, чтобы избежать конфликтов имен
typedef enum {
    // Ключевые слова
    TOKEN_LET,      // let
    TOKEN_MUT,      // mut
    TOKEN_FN,       // fn
    TOKEN_IF,       // if
    TOKEN_ELSE,     // else
    TOKEN_RETURN,   // return

    // Операторы
    TOKEN_PLUS,     // +
    TOKEN_MINUS,    // -
    TOKEN_STAR,     // *
    TOKEN_SLASH,    // /
    TOKEN_PERCENT,  // %
    TOKEN_EQUAL,    // =
    
    // Сравнение
    TOKEN_EQ_EQ,         // ==
    TOKEN_BANG_EQ,       // !=
    TOKEN_BANG,          // !
    TOKEN_GREATER,       // >
    TOKEN_GREATER_EQUAL, // >=
    TOKEN_LESS,          // <
    TOKEN_LESS_EQUAL,    // <=

    // Другое
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_SEMICOLON, // ;
    TOKEN_ERROR,     // Для лексических ошибок
    TOKEN_EOF        // Конец файла
} TokenType;

// 2. Структура Токена
typedef struct {
    TokenType type;
    const char* start;  // Обязательно const! Мы только читаем из буфера
    int length;
    int line;

    // Литеральные значения для констант
    union {
        int as_int;
        double as_float;
    } literal;
} Token;

// 3. Состояние Сканера (Инкапсуляция)
typedef struct {
    const char* start;   // Начало текущего токена
    const char* current; // Текущая позиция "курсора"
    int line;            // Текущий номер строки
} Scanner;

#endif // GHOST_COMMON_H