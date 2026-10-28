#ifndef REGULAR_GRAMMAR_H
#define REGULAR_GRAMMAR_H

#include <stdio.h>
#include "FiniteAutomaton.h"

#define MAX_NONTERMINALS (MAX_STATES + 1)
#define MAX_PRODUCTIONS (2 * MAX_TRANSITIONS + MAX_STATES + 1)

typedef struct {
    int lhs;
    int terminal;
    int nonterminal;
} Production;

typedef struct {
    char nonterminals[MAX_NONTERMINALS][MAX_NAME];
    int nonterminal_count;
    char terminals[MAX_SYMBOLS][MAX_NAME];
    int terminal_count;
    int start;
    Production productions[MAX_PRODUCTIONS];
    int production_count;
} RegularGrammar;

void fa_to_rg(const FiniteAutomaton *fa, RegularGrammar *rg);

void rg_print(const RegularGrammar *rg, FILE *out);

#endif
