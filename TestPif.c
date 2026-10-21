#include "Pif.h"
#include "SymbolTable.h"

int main(void) {
    add_pif("var", -1);
    add_pif("IDENTIFIER", st_insert("real_speed"));
    add_pif(":", -1);
    add_pif("int", -1);
    add_pif("=", -1);
    add_pif("CONSTANT", st_insert("0"));
    add_pif(";", -1);

    pif_print(stdout);
    pif_free();
    st_free();
    return 0;
}