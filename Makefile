
CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -g


TARGET = bin/ghost
SRCS = main.c src/lexer.c src/parser.c
OBJS = $(SRCS:.c=.o)


all: $(TARGET)


$(TARGET): $(OBJS)
	@mkdir -p bin
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)


%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


run: $(TARGET)
	./$(TARGET)


clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf bin