#include <stdio.h>
#include <stdlib.h>
#include "FiniteAutomaton.h"
#include "Scanner.h"
#include "SymbolTable.h"
#include "Pif.h"

#define FA_IDENTIFIER "FA_identifier.in"
#define FA_INTEGER "FA_integer_constant.in"
#define FA_REAL "FA_double_constant.in"

static char *read_file(const char *path) {
    FILE *in = fopen(path, "rb");
    if (in == NULL)
        return NULL;
    fseek(in, 0, SEEK_END);
    long size = ftell(in);
    fseek(in, 0, SEEK_SET);

    char *text = malloc((size_t)size + 1);
    if (text != NULL) {
        size_t read = fread(text, 1, (size_t)size, in);
        text[read] = '\0';
    }
    fclose(in);
    return text;
}

static int load_fa(FiniteAutomaton *fa, const char *path) {
    if (fa_read(fa, path) != 0)
        return -1;
    return fa_build_table(fa, path);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <source file>\n", argv[0]);
        return 1;
    }

    static FiniteAutomaton identifier, integer_constant, real_constant;
    if (load_fa(&identifier, FA_IDENTIFIER) != 0 || load_fa(&integer_constant, FA_INTEGER) != 0 || load_fa(&real_constant, FA_REAL) != 0)
        return 1;

    char *source = read_file(argv[1]);
    if (source == NULL) {
        printf("Cannot open %s\n", argv[1]);
        return 1;
    }

    ScannerAutomata automata = { &identifier, &integer_constant, &real_constant };
    int error_count = scan(source, &automata);
    free(source);

    FILE *pif_out = fopen("PIF.out", "w");
    FILE *st_out = fopen("ST.out", "w");
    if (pif_out == NULL || st_out == NULL) {
        printf("Cannot write PIF.out / ST.out\n");
        return 1;
    }
    pif_print(pif_out);
    st_print(st_out);
    fclose(pif_out);
    fclose(st_out);

    if (error_count == 0)
        printf("Lexically correct\n");
    else
        printf("%d lexical error(s) found\n", error_count);

    pif_free();
    st_free();
    return 0;
}
