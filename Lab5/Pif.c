#include <stdlib.h>
#include <string.h>
#include "Pif.h"

typedef struct {
    char *token;
    int code;
} PifEntry;

static PifEntry *entries = NULL;
static int count = 0;      
static int capacity = 0;   

void add_pif(const char *token, int code) {
    if (count == capacity) {
        int new_capacity = (capacity == 0) ? 8 : capacity * 2;
        PifEntry *grown = realloc(entries, new_capacity * sizeof(PifEntry));
        if (grown == NULL) {
            fprintf(stderr, "Out of memory in PIF\n");
            exit(1);
        }
        entries = grown;
        capacity = new_capacity;
    }
    entries[count].token = strdup(token);
    entries[count].code = code;
    count++;
}

void pif_print(FILE *out) {
    for (int i = 0; i < count; i++)
        fprintf(out, "%-12s %d\n", entries[i].token, entries[i].code);
}

void pif_free(void) {
    for (int i = 0; i < count; i++)
        free(entries[i].token);
    free(entries);
    entries = NULL;
    count = capacity = 0;
}