semanticAnalyzer: main.o parse.o scan.o analyze.o printST.o
	g++ -o semanticAnalyzer main.o parse.o scan.o analyze.o printST.o

main.o: main.cpp globals.h scan.o analyze.o printST.o
	g++ -c -w main.cpp globals.h scan.o analyze.o printST.o

analyze.o: analyze.cpp globals.h
	g++ -c -w analyze.cpp globals.h

parse.o: parse.cpp globals.h scan.o
	g++ -c -w parse.cpp scan.o globals.h

scan.o: scan.cpp globals.h
	g++ -c -w scan.cpp globals.h
	    
printST.o: printST.cpp globals.h
	g++ -c -w printST.cpp globals.h

clean: rm -f *o semanticAnalyzer
