CC = gcc
CFLAGS = -Wall -Wextra -g
FLEX = win_flex

all: minilang

minilang: parser.o lex.yy.o ast.o main.o
	$(CC) $(CFLAGS) -o minilang parser.o lex.yy.o ast.o main.o

parser.c parser.h: parser.y
	win_bison -d -o parser.c parser.y

parser.o: parser.c parser.h
	$(CC) $(CFLAGS) -c parser.c

lex.yy.c: lexer.l parser.h
	$(FLEX) lexer.l

lex.yy.o: lex.yy.c
	$(CC) $(CFLAGS) -c lex.yy.c

ast.o: ast.c ast.h
	$(CC) $(CFLAGS) -c ast.c

main.o: main.c ast.h
	$(CC) $(CFLAGS) -c main.c

clean:
	del minilang parser.c parser.h lex.yy.c *.o
