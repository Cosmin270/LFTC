#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Scanner.h"
#include "SymbolTable.h"
#include "Pif.h"
#define COUNT(a) ((int)(sizeof(a) / sizeof((a)[0])))

static const char *KEYWORDS[] = {
    "type", "record", "end", "var", "int", "real", "bool", "string",
    "input", "read", "send", "log", "wait", "if", "else", "while", "for", "in",
    "when", "true", "false", "traction", "direction", "forward", "reverse",
    "neutral", "cruise", "brake", "direct", "primary", "release", "running",
    "service1", "service2", "service3", "emergency", "emergency_stop", "regen",
    "sand", "pantograph", "up", "down", "breaker", "open", "close",
    "compressor", "start", "stop", "horn", "high", "low", "lights", "back",
    "off", "alarm", "DSV", "PZB", "Fire", "WheelSlip", "U", "O", "M"
};

static const char *OPERATORS[] = {
    "==", "!=", "<=", ">=", "||", "&&", "..", "=", "<", ">", "!", "+", "-", "*", "/", "%"
};

static const char *SEPARATORS[] = { "(", ")", "[", "]", ":", ";", ",", "." };

static int is_keyword(const char *word) {
    for (int i = 0; i < COUNT(KEYWORDS); i++)
        if (strcmp(KEYWORDS[i], word) == 0)
            return 1;
    return 0;
}

static int match_symbol(const char *s, const char **list, int count) {
    for (int i = 0; i < count; i++) {
        size_t len = strlen(list[i]);
        if (strncmp(s, list[i], len) == 0)
            return (int)len;
    }
    return 0;
}

static int is_word_char(char c) {
    return isalnum((unsigned char)c) || c == '_';
}

static int is_string_char(char c) {
    return isalnum((unsigned char)c) || (c != '\0' && strchr("_-+.,:;!?()/%#=<> ", c) != NULL);
}

static int is_glued(const char *s) {
    return is_word_char(s[0]) || (s[0] == '.' && isdigit((unsigned char)s[1]));
}


static int bad_word_length(const char *s) {
    int len = 0;
    while (is_word_char(s[len]) || (s[len] == '.' && is_word_char(s[len + 1])))
        len++;
    return len;
}

static char *copy_lexeme(const char *s, int len) {
    char *lexeme = malloc((size_t)len + 1);
    if (lexeme == NULL) {
        fprintf(stderr, "Out of memory in scanner\n");
        exit(1);
    }
    memcpy(lexeme, s, (size_t)len);
    lexeme[len] = '\0';
    return lexeme;
}

static int scan_string(const char *s, int line, int *errors) {
    int len = 1, bad = 0;
    while (s[len] != '\0' && s[len] != '"' && s[len] != '\n') {
        if (!is_string_char(s[len]))
            bad = 1;
        len++;
    }

    if (s[len] != '"') {
        printf("Lexical error at line %d: unterminated string %.*s\n", line, len, s);
        *errors = 1;
        return len;
    }

    len++;
    char *lexeme = copy_lexeme(s, len);
    if (bad) {
        printf("Lexical error at line %d: invalid character in string %s\n", line, lexeme);
        *errors = 1;
    } 
    else {
        add_pif("CONSTANT", st_insert(lexeme));
        *errors = 0;
    }
    free(lexeme);
    return len;
}

int scan(const char *src, const ScannerAutomata *fa) {
    int line = 1, errors = 0;
    int after_operand = 0;
    size_t i = 0;

    while (src[i] != '\0') {
        const char *s = src + i;

        if (*s == '\n') {
            line++;
            i++;
            continue;
        }
        if (isspace((unsigned char)*s)) {
            i++;
            continue;
        }
        if (*s == '#') {                       
            while (src[i] != '\0' && src[i] != '\n')
                i++;
            continue;
        }
        if (*s == '"') {
            int bad;
            i += (size_t)scan_string(s, line, &bad);
            errors += bad;
            after_operand = 1;
            continue;
        }

        
        int id_len = fa_longest_prefix(fa->identifier, s);
        int num_len = -1;
        if (!((*s == '+' || *s == '-') && after_operand)) {
            int int_len = fa_longest_prefix(fa->integer_constant, s);
            int real_len = fa_longest_prefix(fa->real_constant, s);
            num_len = int_len > real_len ? int_len : real_len;
        }

        if (id_len > 0 && id_len >= num_len) {
            char *lexeme = copy_lexeme(s, id_len);
            if (is_keyword(lexeme)) {
                add_pif(lexeme, -1);
                after_operand = strcmp(lexeme, "true") == 0 || strcmp(lexeme, "false") == 0;
            } else {
                add_pif("IDENTIFIER", st_insert(lexeme));
                after_operand = 1;
            }
            free(lexeme);
            i += (size_t)id_len;
            continue;
        }

        if (num_len > 0) {
            if (is_glued(s + num_len)) {
                int len = num_len + bad_word_length(s + num_len);
                int has_letter = 0;
                for (int k = 0; k < len; k++)
                    if (isalpha((unsigned char)s[k]) || s[k] == '_')
                        has_letter = 1;
                printf("Lexical error at line %d: invalid %s '%.*s'\n",
                       line, has_letter ? "identifier" : "constant", len, s);
                errors++;
                i += (size_t)len;
            } else {
                char *lexeme = copy_lexeme(s, num_len);
                add_pif("CONSTANT", st_insert(lexeme));
                free(lexeme);
                i += (size_t)num_len;
            }
            after_operand = 1;
            continue;
        }

        int len = match_symbol(s, OPERATORS, COUNT(OPERATORS));
        if (len == 0)
            len = match_symbol(s, SEPARATORS, COUNT(SEPARATORS));
        if (len > 0) {
            char *lexeme = copy_lexeme(s, len);
            add_pif(lexeme, -1);
            after_operand = strcmp(lexeme, ")") == 0 || strcmp(lexeme, "]") == 0;
            free(lexeme);
            i += (size_t)len;
            continue;
        }

        if (is_word_char(*s)) {        
            len = bad_word_length(s);
            printf("Lexical error at line %d: invalid identifier '%.*s'\n", line, len, s);
        } else {
            len = 1;
            printf("Lexical error at line %d: illegal character '%c'\n", line, *s);
        }
        errors++;
        after_operand = 0;
        i += (size_t)len;
    }
    return errors;
}
