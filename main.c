#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "lexer.h"
#include "parser.h"


static void run(const char* source) {
    parse(source);
}

// Режим запуска из файла
static void runFile(const char* path) {
    char* source = readFile(path); 
    if (source == NULL) return; 

    run(source);
    free(source);
}

// Интерактивный режим (REPL)
static void repl() {
    char line[1024];
    printf("Ghost Language [Version 0.0.1]\n");
    printf("Type 'exit' to quit or 'test' to run test.gh\n");

    for (;;) {
        printf("ghost > ");
        fflush(stdout); // Чтобы приглашение появлялось мгновенно

        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }

    
        line[strcspn(line, "\n")] = 0;

        // Команда выхода
        if (strcmp(line, "exit") == 0) break;

        // Команда для быстрого теста файла test.gh
        if (strcmp(line, "test") == 0) {
            printf("--- Running test.gh ---\n");
            runFile("test.gh");
            printf("--- Done ---\n");
            continue;
        }


        if (strlen(line) == 0) continue;

        run(line);
    }
}

int main(int argc, const char* argv[]) {
    // Если аргументов нет — запускаем консоль
    if (argc == 1) {
        repl();
    } 
    // Если один аргумент — считаем его путем к файлу
    else if (argc == 2) {
        // Удобство: 'ghost test' тоже запустит test.gh
        if (strcmp(argv[1], "test") == 0) {
            runFile("test.gh");
        } else {
            runFile(argv[1]);
        }
    } 
    else {
        fprintf(stderr, "Usage: ghost [path]\n");
        exit(64);
    }

    return 0;
}