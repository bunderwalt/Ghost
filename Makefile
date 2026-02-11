CC = gcc
# Флаг -Iinclude позволяет писать #include "lexer.h" без папки
CFLAGS = -Iinclude -Wall -Wextra 
TARGET = bin/ghost

SRCS = main.c src/lexer.c
# Превращаем список .c файлов в список .o в папке obj
OBJS = obj/main.o obj/lexer.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

# Правило для компиляции .c в .o
obj/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

obj/lexer.o: src/lexer.c
	$(CC) $(CFLAGS) -c src/lexer.c -o obj/lexer.o

clean:
	rm -rf obj/*.o bin/ghost