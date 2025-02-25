all : 
		yacc -d poly.y
		flex poly.l
		gcc polyutils.c

run : all
		./a.out < sample.txt > output.txt

clean : 
		rm -f lex.yy.c y.tab.c y.tab.h a.out output.txt