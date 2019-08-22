GCCFLAGS = -Ofast -mrecip
CLANGFLAGS = -Ofast
GCC = gcc
CLANG = clang

fdiv-test : divgcc.o main.o divclg.o
	$(CLANG) -o fdiv-test divgcc.o main.o divclg.o

divgcc.o : divgcc.c
	$(GCC) -o divgcc.o $(GCCFLAGS) -c divgcc.c

divclg.o : divclg.c
	$(CLANG) -o divclg.o $(CLANGFLAGS) -c divclg.c

main.o : main.c inputs.h
	$(CLANG) -o main.o $(CLANGFLAGS) -c main.c

clean :
	rm fdiv-test main.o divclg.o divgcc.o
