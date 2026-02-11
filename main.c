#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"

int main() {
    // 1. Загружаем исходный код из файла в память
    char* source = readFile("test.gh");
    
    // 2. "Заряжаем" сканер нашим кодом
    initScanner(source); 

    printf("--- Ghost Lexer Output ---\n");
    printf("%-10s | %-15s | %s\n", "LINE", "TYPE", "TEXT");
    printf("-------------------------------------------\n");

    // 3. Основной цикл прохода по токенам
    for (;;) {
        // Запрашиваем следующий токен у лексера
        Token t = getToken();

        // Выводим информацию в красивом табличном виде
        // %.*s берет длину из t.length и начало из t.start
        printf("[%4d]     | %-15s | '%.*s'\n", 
               t.line, 
               getTokenName(t.type), 
               t.length, 
               t.start);

        // Если дошли до конца файла — выходим из цикла
        if (t.type == TOKEN_EOF) {
            break;
        }
    }

    // 4. Освобождаем память, выделенную readFile
    free(source);
    
    return 0;
}