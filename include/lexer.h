#ifndef GHOST_LEXER_H
#define GHOST_LEXER_H

#include "common.h"

// 1. Утилита для чтения файлов (нужна в main.c)
// Возвращает динамически выделенную строку, которую нужно будет free()
char* readFile(const char* path);

// 2. Инициализация лексера
// Настраивает внутренний Scanner на работу с исходным кодом
void initScanner(const char* source);

// 3. Основная рабочая функция
// Вызывается парсером для получения следующего токена
Token getToken();

// 4. Инструмент для отладки
// Превращает TokenType (число) в понятную строку (например, "TOKEN_LET")
const char* getTokenName(TokenType type);

#endif // GHOST_LEXER_H