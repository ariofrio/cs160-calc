YACC	= bison -d -y 
LEX	= flex
CC	= gcc
CPP	= g++
CPPFLAGS=-std=gnu++11
DEFTARGET = calc_def

all: calc_def calc  
test_def: calc_def
	./calc_def < test.good.calc > test.good.defoutput
	./calc_def < test.bad.calc > test.bad.defoutput
test_scan: calc 
	./calc -s < test.good.calc > test.good.tokens
	./calc -s < test.bad.calc > test.bad.tokens
test_parse: calc
	./calc < test.good.calc > test.good.dot
	dot -Tpng test.good.dot -o test.good.png
	./calc < test.bad.calc > test.bad.dot
###############################################
# This part makes your parse tree generator
#

calc: calc.o 
	    $(CPP) $(CPPFLAGS) calc.o -o calc
calc.o: calc.cpp 
	    $(CPP) $(CPPFLAGS) -c calc.cpp

################################################
# This part makes the parsing definition
#

$(DEFTARGET): y.tab.o lex.yy.o
	    $(CC) lex.yy.o y.tab.o -o $(DEFTARGET)
y.tab.o: y.tab.c
	    $(CC) -c y.tab.c
y.tab.c: $(DEFTARGET).y
	    $(YACC) $(DEFTARGET).y
y.tab.h: $(DEFTARGET).y
	    $(YACC) $(DEFTARGET).y
lex.yy.o: lex.yy.c y.tab.h
	    $(CC) -c lex.yy.c
lex.yy.c: $(DEFTARGET).l
	    $(LEX) $(DEFTARGET).l

clean:
	rm -f calc calc.o $(DEFTARGET) y.tab.o y.tab.c y.tab.h lex.yy.o lex.yy.c
	rm -f test.good.defoutput test.bad.defoutput test.good.dot test.bad.dot test.good.png test.good.tokens test.bad.tokens

submit: calc
	turnin proj1@cs160 .
