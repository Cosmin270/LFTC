CC = gcc
CFLAGS = -Wall -Wextra -std=c99
SRC = main.c Scanner.c FiniteAutomaton.c SymbolTable.c Pif.c

scanner: $(SRC) Scanner.h FiniteAutomaton.h SymbolTable.h Pif.h
	$(CC) $(CFLAGS) -o scanner $(SRC)

clean:
	rm -f scanner PIF.out ST.out
