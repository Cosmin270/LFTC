CC = gcc
CFLAGS = -Wall -Wextra -std=c99

fa: main.c FiniteAutomaton.c RegularGrammar.c FiniteAutomaton.h RegularGrammar.h
	$(CC) $(CFLAGS) -o fa main.c FiniteAutomaton.c RegularGrammar.c

clean:
	rm -f fa
