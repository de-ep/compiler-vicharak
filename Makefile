CC = gcc
CFLAGS = -O2 -Wall -Wextra -g
TARGET = compiler
OBJS = main.o tokenize.o parser.o codegen.o utils.o 

all: $(TARGET)

$(TARGET): $(OBJS)
		$(CC) $(OBJS) -o $(TARGET)

main.o: src/main.c
		$(CC) -c src/main.c $(CFLAGS)

tokenize.o: src/tokenize.c 
		$(CC) -c src/tokenize.c $(CFLAGS)

parser.o: src/parser.c
		$(CC) -c src/parser.c $(CFLAGS)

codegen.o: src/parser.c
		$(CC) -c src/codegen.c $(CFLAGS)

utils.o: src/utils.c 
		$(CC) -c src/utils.c $(CFLAGS)

clean: 
		rm $(TARGET) $(OBJS)
