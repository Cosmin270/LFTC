#ifndef ST_H
#define ST_H
#include <stdio.h>

#define ST_SIZE 31

int  st_insert(const char *lexeme); 
void st_print(FILE *out);
void st_free(void);

#endif