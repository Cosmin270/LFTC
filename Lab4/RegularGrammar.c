#include <string.h>
#include "RegularGrammar.h"

static void add_production(RegularGrammar *rg, int lhs, int terminal, int nonterminal) {
    Production p = { lhs, terminal, nonterminal };
    rg->productions[rg->production_count++] = p;
}

static int is_reached(const FiniteAutomaton *fa, int state) {
    for (int i = 0; i < fa->transition_count; i++)
        if (fa->transitions[i].to == state)
            return 1;
    return 0;
}
    
void fa_to_rg(const FiniteAutomaton *fa, RegularGrammar *rg) {
    memset(rg, 0, sizeof(*rg));

    for (int i = 0; i < fa->state_count; i++)
        strcpy(rg->nonterminals[i], fa->states[i]);
    rg->nonterminal_count = fa->state_count;
    for (int i = 0; i < fa->symbol_count; i++)
        strcpy(rg->terminals[i], fa->alphabet[i]);
    rg->terminal_count = fa->symbol_count;
    rg->start = fa->initial;

    if (fa->is_final[fa->initial] && is_reached(fa, fa->initial)) {
        rg->start = rg->nonterminal_count++;
        snprintf(rg->nonterminals[rg->start], MAX_NAME, "%.*s'", MAX_NAME - 2, fa->states[fa->initial]);
    }

    for (int i = 0; i < fa->transition_count; i++) {
        const Transition *t = &fa->transitions[i];
        add_production(rg, t->from, t->symbol, t->to);
        if (fa->is_final[t->to])
            add_production(rg, t->from, t->symbol, -1);
    }

    if (rg->start != fa->initial) {
        int count = rg->production_count;
        for (int i = 0; i < count; i++)
            if (rg->productions[i].lhs == fa->initial)
                add_production(rg, rg->start, rg->productions[i].terminal, rg->productions[i].nonterminal);
    }
    if (fa->is_final[fa->initial])
        add_production(rg, rg->start, -1, -1);
}

static void print_rhs(const RegularGrammar *rg, const Production *p, FILE *out) {
    if (p->terminal < 0)
        fprintf(out, "epsilon");
    else if (p->nonterminal < 0)
        fprintf(out, "%s", rg->terminals[p->terminal]);
    else
        fprintf(out, "%s %s", rg->terminals[p->terminal], rg->nonterminals[p->nonterminal]);
}

void rg_print(const RegularGrammar *rg, FILE *out) {
    fprintf(out, "N = {");
    for (int i = 0; i < rg->nonterminal_count; i++)
        fprintf(out, "%s%s", i ? ", " : " ", rg->nonterminals[i]);
    fprintf(out, " }\nE = {");
    for (int i = 0; i < rg->terminal_count; i++)
        fprintf(out, "%s%s", i ? ", " : " ", rg->terminals[i]);
    fprintf(out, " }\nS = %s\nP:\n", rg->nonterminals[rg->start]);

    for (int k = 0; k < rg->nonterminal_count; k++) {
        int lhs = k == 0 ? rg->start : (k <= rg->start ? k - 1 : k);
        int first = 1;
        for (int i = 0; i < rg->production_count; i++) {
            const Production *p = &rg->productions[i];
            if (p->lhs != lhs)
                continue;
            fprintf(out, first ? "  %s -> " : " | ", rg->nonterminals[lhs]);
            print_rhs(rg, p, out);
            first = 0;
        }
        if (!first)
            fprintf(out, "\n");
    }
}
