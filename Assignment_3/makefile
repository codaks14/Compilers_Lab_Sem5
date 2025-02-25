all:
	yacc -d expr.y
	lex as3.l
	gcc as3.c -lm
	./a.out < sample.txt

clean:
	-rm -f a.out y.tab.h y.tab.c lex.yy.c
