RECIP_FLAGS = -Ofast
NORECIP_FLAGS = -O3 -DNO_WARN_X86_INTRINSICS
CC = clang

tests : compiling algorithm compiling-set

compiling : divgcc.o compiling_test.o divclg.o
	$(CC) -o compiling-test divgcc.o compiling_test.o divclg.o

algorithm : algorithm_test.c
	$(CC) -o algorithm-test $(NORECIP_FLAGS) algorithm_test.c

compiling-set : divgcc.o compiling_test_set.o divclg.o
	$(CC) -o compiling-test-set divgcc.o compiling_test_set.o divclg.o

divgcc.o : divgcc.c
	$(CC) -o divgcc.o $(NORECIP_FLAGS) -c divgcc.c

divclg.o : divclg.c
	$(CC) -o divclg.o $(RECIP_FLAGS) -c divclg.c

compiling_test.o : algorithm_test.c
	$(CC) -o compiling_test.o $(NO_RECIP_FLAGS) -c compiling_test.c

compiling_test_set.o : compiling_test_set.c
	$(CC) -o compiling_test_set.o $(NO_RECIP_FLAGS) -c compiling_test_set.c

.PHONY : clean
clean :
	rm -f compiling-test algorithm-test compiling-test-set compiling_test.o \
	  compiling_test_set.o divclg.o divgcc.o
