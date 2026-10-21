#include "SymbolTable.h"

int main(void) {
    printf("%d\n", st_insert("real_speed"));  
    printf("%d\n", st_insert("0"));           
    printf("%d\n", st_insert("\"none\""));    
    printf("%d\n", st_insert("real_speed"));  
    st_print(stdout);
    st_free();
    return 0;
}