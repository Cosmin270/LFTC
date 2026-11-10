#ifndef SCANNER_H
#define SCANNER_H

#include "FiniteAutomaton.h"

typedef struct {
    const FiniteAutomaton *identifier;
    const FiniteAutomaton *integer_constant;
    const FiniteAutomaton *real_constant;
} ScannerAutomata;


int scan(const char *source, const ScannerAutomata *fa);

#endif
