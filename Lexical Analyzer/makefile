lexicalAnalyzer: main.o printToken.o
				 g++ -o lexicalAnalyzer main.o printToken.o

main.o: main.cpp globals.h
		g++ -c -w main.cpp

printToken.o: printToken.cpp globals.h
	    g++ -c -w printToken.cpp