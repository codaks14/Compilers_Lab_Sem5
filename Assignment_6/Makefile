all : 
	flex prog.l
	yacc -d prog.y
	g++ -o codegen codegen.cpp

run: all
	./codegen <sample.txt

clean : 
	rm -f lex.yy.c y.tab.h y.tab.c codegen intcode.txt target.txt