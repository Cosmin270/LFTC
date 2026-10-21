#include <stdlib.h>
#include <string.h>
#include "SymbolTable.h"

typedef struct Node {
    char *lexeme;
    int position;
    struct Node *next;
} Node;

static Node *table[ST_SIZE];  
static int next_position = 0;

static unsigned int hash(const char *s) {
    unsigned int h = 0;
    while (*s)
        h = h * 31 + (unsigned char)*s++;
    return h % ST_SIZE;
}

int st_insert(const char *lexeme) {
    unsigned int index = hash(lexeme);
    Node *current = table[index];

    while (current) {
        if (strcmp(current->lexeme, lexeme) == 0) {
            return current->position;  
        }
        current = current->next;
    }

    Node *new_node = (Node *)malloc(sizeof(Node));
    new_node->lexeme = strdup(lexeme);
    new_node->position = next_position++;
    new_node->next = table[index];
    table[index] = new_node;

    return new_node->position;
}

void st_print(FILE *out) {
    for (int i = 0; i < ST_SIZE; i++) {
        Node *current = table[i];
        while (current) {
            fprintf(out, "%s <--> %d\n", current->lexeme, current->position);
            current = current->next;
        }
    }
}

void st_free(void) {
    for (int i = 0; i < ST_SIZE; i++) {
        Node *current = table[i];
        while (current) {
            Node *temp = current;
            current = current->next;
            free(temp->lexeme);
            free(temp);
        }
        table[i] = NULL;
    }
    next_position = 0;
}