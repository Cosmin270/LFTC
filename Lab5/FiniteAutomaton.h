#ifndef FINITE_AUTOMATON_H
#define FINITE_AUTOMATON_H

#define MAX_STATES 64
#define MAX_SYMBOLS 128
#define MAX_TRANSITIONS 1024
#define MAX_NAME 32

typedef struct {
    int from;
    int symbol;
    int to;
} Transition;

typedef struct {
    char states[MAX_STATES][MAX_NAME];
    int state_count;
    char alphabet[MAX_SYMBOLS][MAX_NAME];
    int symbol_count;
    int initial;
    int is_final[MAX_STATES];
    Transition transitions[MAX_TRANSITIONS];
    int transition_count;
    int delta[MAX_STATES][256];   
} FiniteAutomaton;


int fa_read(FiniteAutomaton *fa, const char *path);

int fa_build_table(FiniteAutomaton *fa, const char *path);

int fa_longest_prefix(const FiniteAutomaton *fa, const char *s);

int fa_accepts(const FiniteAutomaton *fa, const char *word);

#endif
