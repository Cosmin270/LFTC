#include <stdio.h>
#include <string.h>
#include "FiniteAutomaton.h"
#include "RegularGrammar.h"

static void rg_output_path(const char *fa_path, char *out, size_t size) {
    const char *base = strrchr(fa_path, '/');
    base = base ? base + 1 : fa_path;
    if (strncmp(base, "FA_", 3) == 0)
        base += 3;
    const char *dot = strrchr(base, '.');
    int len = dot ? (int)(dot - base) : (int)strlen(base);
    snprintf(out, size, "RG_%.*s.out", len, base);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <FA file>\n", argv[0]);
        return 1;
    }

    static FiniteAutomaton fa;
    if (fa_read(&fa, argv[1]) != 0)
        return 1;

    static RegularGrammar rg;
    fa_to_rg(&fa, &rg);

    char path[256];
    rg_output_path(argv[1], path, sizeof(path));
    FILE *out = fopen(path, "w");
    if (out == NULL) {
        printf("Cannot write %s\n", path);
        return 1;
    }
    rg_print(&rg, out);
    fclose(out);
    printf("Grammar written to %s\n", path);
    return 0;
}
