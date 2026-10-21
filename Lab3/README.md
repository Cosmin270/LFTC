# Lab 3 – Lexical Analyzer for the Locomotive DSL

A scanner for the Locomotive DSL (Lab 1), written with **flex**. It reads a source program and produces the **Program Internal Form (PIF)**, the **Symbol Table (ST)**, and a list of **lexical errors**.

[Github repository](https://github.com/Cosmin270/LFTC)

## Files

| File                | Purpose                                               |
| ------------------- | ----------------------------------------------------- |
| `loco.l`            | flex specification: token rules, error rules, `main`  |
| `SymbolTable.h/.c`  | Symbol table, implemented as a hash table             |
| `Pif.h/.c`          | Program Internal Form, implemented as a dynamic array |
| `program.loco`      | Lexically correct sample program                      |
| `p_err.loco`        | Sample program with lexical errors                    |
| `PIF.out`, `ST.out` | Output of the last run                                |

## Build and run

```sh
flex loco.l
gcc -o loco lex.yy.c SymbolTable.c Pif.c
./loco program.loco
```

The PIF goes to `PIF.out` and the ST to `ST.out`. Lexical errors print on the console, followed by either `Lexically correct` or the number of errors found.

## Token classification

| Category          | Rule                                                                             | PIF entry                  |
| ----------------- | -------------------------------------------------------------------------------- | -------------------------- |
| Keywords          | 62 reserved words of the language (`var`, `if`, `brake`, `PZB`, …)               | `<keyword> -1`             |
| Operators         | `== != <= >= \|\| && .. = < > ! + - * / %`                                       | `<operator> -1`            |
| Separators        | `( ) [ ] : ; , .`                                                                | `<separator> -1`           |
| Identifiers       | `letter (letter \| digit \| _)*`                                                 | `IDENTIFIER <ST position>` |
| Integer constants | `digit+`                                                                         | `CONSTANT <ST position>`   |
| Real constants    | `digit+ . digit+`                                                                | `CONSTANT <ST position>`   |
| String constants  | `" string_char* "` (letters, digits, space, `_ - + . , : ; ! ? ( ) / % # = < >`) | `CONSTANT <ST position>`   |

Keyword rules come before the identifier rule, so a word like `while` is a keyword. Flex also picks the longest match, so `real_speed` is one identifier and not `real` + `_speed`. Two-character operators (`==`, `..`, …) are listed before the one-character ones.

## Lexical errors

Each error is reported with its line number, and scanning continues:

| Error                             | Example       |
| --------------------------------- | ------------- |
| Identifier starting with a digit  | `1speed`      |
| Unterminated string               | `"door open;` |
| Character not allowed in a string | `"a@b"`       |
| Illegal character                 | `$`, `@`, `&` |

## Symbol Table – hash table

The ST holds identifiers and constants in one table.

- **Structure:** an array of `ST_SIZE = 31` buckets. Each bucket is a linked list (separate chaining) of nodes `{ lexeme, position, next }`.
- **Hash function:** a polynomial rolling hash, `h = h * 31 + c` for each character `c`, then `h mod 31`.
- **Insert (`st_insert`):** hash the lexeme and search its bucket. If the lexeme is already there, return its position. Otherwise add a new node at the head of the bucket with the next free position (0, 1, 2, … in order of first appearance) and return that position.
- **Positions** are stable and unique, and each lexeme is stored only once. The PIF refers to ST entries by these positions.
- **Complexity:** O(1) average for insert and search, O(n) in the worst case (all lexemes in one bucket).
- **Output:** `ST.out` lists each entry as `lexeme <--> position`, bucket by bucket.

## PIF

A dynamic array of `(token, position)` pairs that doubles its capacity when full. Entries are appended in the order the tokens appear in the source. Keywords, operators and separators have position `-1`. Identifiers and constants have their ST position.

Example (`var real_speed : int = 0;`):

```
var          -1
IDENTIFIER   0
:            -1
int          -1
=            -1
CONSTANT     1
;            -1
```
