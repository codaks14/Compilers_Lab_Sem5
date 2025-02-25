all:
	flex expr.l 
	yacc -d expr.y 
	gcc -o icgen intcodegen.c
run: all
	./icgen <sample.txt
	gcc -o ic intcode.c
	./ic >output.txt
clean:
	rm -f lex.yy.c y.tab.c y.tab.h icgen ic intcode.c output.txt
	