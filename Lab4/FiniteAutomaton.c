#include <stdio.h>
#include <string.h>
#include "FiniteAutomaton.h"

#define MAX_LINE 4096

static int fa_find_state(const FiniteAutomaton *fa, const char *name) {
    for (int i = 0; i < fa->state_count; i++)
        if (strcmp(fa->states[i], name) == 0)
            return i;
    return -1;
}

static int fa_find_symbol(const FiniteAutomaton *fa, const char *name) {
    for (int i = 0; i < fa->symbol_count; i++)
        if (strcmp(fa->alphabet[i], name) == 0)
            return i;
    return -1;
}

static int read_names(char *rest, char names[][MAX_NAME], int max, int line_no) {
    int count = 0;
    for (char *tok = strtok(rest, " \t\r\n"); tok; tok = strtok(NULL, " \t\r\n")) {
        if (count == max || strlen(tok) >= MAX_NAME) {
            printf("Line %d: too many elements or name too long\n", line_no);
            return -1;
        }
        strcpy(names[count++], tok);
    }
    return count;
}

static int read_state_list(FiniteAutomaton *fa, char *rest, int *out, int line_no) {
    char names[MAX_STATES][MAX_NAME];
    int count = read_names(rest, names, MAX_STATES, line_no);
    if (count < 0)
        return -1;
    for (int i = 0; i < count; i++) {
        out[i] = fa_find_state(fa, names[i]);
        if (out[i] < 0) {
            printf("Line %d: unknown state '%s'\n", line_no, names[i]);
            return -1;
        }
    }
    return count;
}

static int read_transition(FiniteAutomaton *fa, char *line, int line_no) {
    char from[MAX_NAME], symbol[MAX_NAME], to[MAX_NAME], extra[MAX_NAME];
    int fields = sscanf(line, "%31s %31s %31s %31s", from, symbol, to, extra);
    if (fields != 3) {
        printf("Line %d: a transition must be 'state symbol state'\n", line_no);
        return -1;
    }

    Transition t = { fa_find_state(fa, from), fa_find_symbol(fa, symbol), fa_find_state(fa, to) };
    if (t.from < 0 || t.to < 0) {
        printf("Line %d: unknown state in transition\n", line_no);
        return -1;
    }
    if (t.symbol < 0) {
        printf("Line %d: symbol '%s' is not in the alphabet\n", line_no, symbol);
        return -1;
    }
    if (fa->transition_count == MAX_TRANSITIONS) {
        printf("Line %d: too many transitions\n", line_no);
        return -1;
    }
    fa->transitions[fa->transition_count++] = t;
    return 0;
}

int fa_read(FiniteAutomaton *fa, const char *path) {
    FILE *in = fopen(path, "r");
    if (in == NULL) {
        printf("Cannot open %s\n", path);
        return -1;
    }

    memset(fa, 0, sizeof(*fa));
    fa->initial = -1;

    char line[MAX_LINE];
    int line_no = 0, in_transitions = 0, status = 0;

    while (status == 0 && fgets(line, sizeof(line), in)) {
        line_no++;
        if (strspn(line, " \t\r\n") == strlen(line))
            continue;

        if (in_transitions) {
            status = read_transition(fa, line, line_no);
        } else if (strncmp(line, "states:", 7) == 0) {
            fa->state_count = read_names(line + 7, fa->states, MAX_STATES, line_no);
            status = fa->state_count < 0 ? -1 : 0;
        } else if (strncmp(line, "alphabet:", 9) == 0) {
            fa->symbol_count = read_names(line + 9, fa->alphabet, MAX_SYMBOLS, line_no);
            status = fa->symbol_count < 0 ? -1 : 0;
        } else if (strncmp(line, "initial:", 8) == 0) {
            int initial[MAX_STATES];
            int count = read_state_list(fa, line + 8, initial, line_no);
            if (count != 1) {
                if (count >= 0)
                    printf("Line %d: exactly one initial state is required\n", line_no);
                status = -1;
            } else {
                fa->initial = initial[0];
            }
        } else if (strncmp(line, "final:", 6) == 0) {
            int finals[MAX_STATES];
            int count = read_state_list(fa, line + 6, finals, line_no);
            for (int i = 0; i < count; i++)
                fa->is_final[finals[i]] = 1;
            status = count < 0 ? -1 : 0;
        } else if (strncmp(line, "transitions:", 12) == 0) {
            in_transitions = 1;
        } else {
            printf("Line %d: unexpected line '%s'\n", line_no, strtok(line, "\r\n"));
            status = -1;
        }
    }
    fclose(in);

    if (status == 0 && (fa->state_count == 0 || fa->symbol_count == 0 || fa->initial < 0)) {
        printf("%s: states, alphabet and initial state are required\n", path);
        status = -1;
    }
    return status;
}
