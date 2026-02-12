#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "lexer.h"
#include "parser.h"


static void run(const char* source) {
    
    parse(source);
}


static void repl() {
    char line[1024];
    printf("Ghost Language [Version 0.0.1]\n");
    printf("Type 'exit' to quit.\n");

    for (;;) {
        printf("ghost > ");

        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }

        if (strncmp(line, "exit", 4) == 0) break;

        run(line);
    }
}

// Режим запуска из файла
static void runFile(const char* path) {
    char* source = readFile(path); 
    run(source);
    free(source);
}

int main(int argc, const char* argv[]) {

    if (argc == 1) {
        repl();
    } 

    else if (argc == 2) {
        runFile(argv[1]);
    } 
    else {
        fprintf(stderr, "Usage: ghost [path]\n");
        exit(64);
    }

    return 0;
}