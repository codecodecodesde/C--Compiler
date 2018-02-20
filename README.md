## C-- Compiler
C-- Compiler is a compiler for a subset of C language implemented by C++. It includes 4 components:
* Lexical Analyzer
* Parser
* Semantic Analysis
* Code Generation

Each component is able to be compiled and ran independently. Each of them contains code in previous compiler construction phases.
```
make lexicalAnalyzer
make parser
make semanticAnalyzer
make codeGeneration
```

#### Lexical Analyzer
Lexical Analyzer can recognize tokens in defined language and ignore white spaces and comments(enclosed inside `/*...*/`). The principle function is `getToken()` that returns the next token. The `main` function contains a loop that calls `getToken()`. This program will print out token class returned and the string value.

TODO: attach DFA graph

#### Parser

Built followed by revised LL(1) grammar(LL(1) grammar file is under Parser folder). Given an input, Parser either reports an appropriate error message if it finds a syntax issue , or constructs an abstract syntax tree and prints it out in preorder.

#### Semantic Analysis
Semantic Analysis can detect following semantic error:
1. undeclared variables and functions
2. multiple occurrences of the same variable or function name in the same scope
3. parameters error: wrong number or wrong type
4. break statements must occur in a loop

For each error, it will print an appropriate message and exit. If the program has no semantic issue, it will print out necessary information contained in the symbol table for each scope including the names and their associated semantic information.

#### Code Generation
Code Generation produce instructions following IBM rules. You can run and test your output instructions in modified "Tiny Machine" from Kenneth C. Louden(`ibm.c`). Some future work can be done, like tail recursion optimization.















