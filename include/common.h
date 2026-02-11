#ifndef GHOST_COMMON_H
#define GHOST_COMMON_H

#include <stdbool.h>

// Максимальная длина идентификатора (если потребуется буферизация)
#define MAX_IDENTIFIER_LEN 31

// 1. Перечисление типов токенов
// Используем префикс TOKEN_ для ВСЕХ элементов, чтобы избежать конфликтов имен
typedef enum {
    // Ключевые слова
    TOKEN_LET,              // let
    TOKEN_MUT,              // mut
    TOKEN_FN,               // fn
    TOKEN_IF,               // if
    TOKEN_ELSE,             // else
    TOKEN_RETURN,           // return

    // Операторы
    TOKEN_PLUS,             // +
    TOKEN_MINUS,            // -
    TOKEN_STAR,             // *
    TOKEN_SLASH,            // /
    TOKEN_PERCENT,          // %
    TOKEN_EQUAL,            // =
    
    // Сравнение
    TOKEN_EQ_EQ,            // ==
    TOKEN_BANG_EQ,          // !=
    TOKEN_BANG,             // !
    TOKEN_GREATER,          // >
    TOKEN_GREATER_EQUAL,    // >=
    TOKEN_LESS,             // <
    TOKEN_LESS_EQUAL,       // <=

    // Типы данных
    TOKEN_I8, TOKEN_I16, TOKEN_I32,
    TOKEN_F8, TOKEN_F16, TOKEN_F32,
    
    TOKEN_BOOL,             // bool
    TOKEN_STR,              // string
    

    // Другое
    TOKEN_IDENTIFIER,       // переменная
    TOKEN_NUMBER,           // число
    TOKEN_SEMICOLON,        // ;
    TOKEN_ERROR,            // Для лексических ошибок
    TOKEN_EOF,              // Конец файла
    TOKEN_COLON,            // :
    TOKEN_AMPERSAND,        // &
    TOKEN_STRING_LITERAL,   // "текст"

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