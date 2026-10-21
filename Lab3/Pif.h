#ifndef PIF_H
#define PIF_H
#include <stdio.h>

void add_pif(const char *token, int code); 
void pif_print(FILE *out);
void pif_free(void);

#endif