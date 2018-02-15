parser: main.o parse.o scan.o printAST.o
	g++ -o parser main.o parse.o scan.o printAST.o

main.o: main.cpp globals.h scan.o printAST.o
	g++ -c -w main.cpp globals.h scan.o printAST.o

parse.o: parse.cpp globals.h scan.o
	g++ -c -w parse.cpp scan.o 

scan.o: scan.cpp globals.h
	g++ -c -w scan.cpp globals.h
	    
printAST.o: printAST.cpp globals.h
	g++ -c -w printAST.cpp

clean: rm -f *o parser
