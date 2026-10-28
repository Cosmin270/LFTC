# Lab 4 – Finite Automata and Regular Grammars for the Locomotive DSL

Identifiers and constants of the Locomotive DSL (Lab 1) are defined as **finite automata (FA)**. The program reads an FA from a file and **transforms it into an equivalent regular grammar (RG)**.

[Github repository](https://github.com/Cosmin270/LFTC)

## Files

| File                     | Purpose                                     |
| ------------------------ | ------------------------------------------- |
| `FA_identifier.in`       | FA for identifiers                          |
| `FA_integer_constant.in` | FA for integer constants                    |
| `FA_double_constant.in`  | FA for real (double) constants              |
| `FiniteAutomaton.h/.c`   | Reading the FA file                         |
| `RegularGrammar.h/.c`    | FA → RG transformation and grammar printing |
| `main.c`                 | Reads the FA, transforms it, writes the RG  |
| `RG_*.out`               | Grammars produced from the FA files         |

## Build and run

```sh
make
./fa FA_identifier.in
```

The program prints the grammar and writes it to `RG_<name>.out` (e.g. `FA_identifier.in` → `RG_identifier.out`).

## FA file format (EBNF)

```
fa_file          = states_line alphabet_line initial_line final_line transitions_block ;
states_line      = "states:" state { state } NEWLINE ;
alphabet_line    = "alphabet:" symbol { symbol } NEWLINE ;
initial_line     = "initial:" state NEWLINE ;
final_line       = "final:" { state } NEWLINE ;
transitions_block = "transitions:" NEWLINE { transition NEWLINE } ;
transition       = state symbol state ;
state            = name ;
symbol           = name ;
name             = non_blank_char { non_blank_char } ;
```

Elements on a line are separated by spaces. Blank lines are ignored. Every state and symbol used in `initial`, `final` and the transitions must be declared in `states` / `alphabet`; otherwise reading stops with an error and the line number.

## The automata

### Identifiers — `letter (letter | digit | _)*`

| State                      | Meaning               | letter                    | digit | `_` |
| -------------------------- | --------------------- | ------------------------- | ----- | --- |
| → q0                       | start                 | q1                        | –     | –   |
| q1 \*                      | inside the identifier | q1                        | q1    | q1  |
| ### Integer constants — `0 | [+                    | -] nonzero_digit digit\*` |

| State | Meaning          | `+` `-` | `0` | `1`–`9` |
| ----- | ---------------- | ------- | --- | ------- |
| → q0  | start            | q1      | q2  | q3      |
| q1    | after the sign   | –       | –   | q3      |
| q2 \* | the constant `0` | –       | –   | –       |
| q3 \* | non-zero integer | –       | q3  | q3      |

No leading zeros and no signed zero.

### Real constants — `[+|-] (0 | nonzero_digit digit*) . digit+`

| State | Meaning                     | `+` `-` | `0` | `1`–`9` | `.` |
| ----- | --------------------------- | ------- | --- | ------- | --- |
| → q0  | start                       | q1      | q2  | q3      | –   |
| q1    | after the sign              | –       | q2  | q3      | –   |
| q2    | integer part `0`            | –       | –   | –       | q4  |
| q3    | integer part without lead 0 | –       | q3  | q3      | q4  |
| q4    | after the point             | –       | q5  | q5      | –   |
| q5 \* | fractional part             | –       | q5  | q5      | –   |

The integer part follows the same rule as integer constants, but `-0.25` is allowed.

(→ marks the initial state, \* the final states.)

## FA → RG transformation

For `M = (Q, E, d, q0, F)` the grammar is `G = (N, E, P, S)` with `N = Q`, the same terminals, `S = q0`, and for every transition `d(p, a) = q`:

- `p -> a q`
- `p -> a`, if `q ∈ F`

The grammar generates exactly the words accepted by the FA: a derivation `q0 => a1 q1 => a1 a2 q2 => ... => a1...an` follows a path of the automaton that ends in a final state.

**Empty word.** If `q0 ∈ F`, the FA accepts ε, so the grammar needs `S -> epsilon`. In a regular grammar `S` must then not appear on any right-hand side. If no transition goes into `q0`, the program adds `q0 -> epsilon`. Otherwise it adds a new start symbol `q0'` with a copy of every production of `q0` and `q0' -> epsilon`.

Example: `states: s`, `final: s`, `d(s, a) = s` gives

```
S = s'
  s' -> a s | a | epsilon
  s -> a s | a
```

## Resulting grammars

Productions are grouped by left-hand side. The full output is in the `RG_*.out` files.

**Identifiers** (`RG_identifier.out`)

```
S = q0
  q0 -> a q1 | a | ... | Z q1 | Z
  q1 -> a q1 | a | ... | 9 q1 | 9 | _ q1 | _
```

**Integer constants** (`RG_integer_constant.out`)

```
S = q0
  q0 -> + q1 | - q1 | 0 q2 | 0 | 1 q3 | 1 | ... | 9 q3 | 9
  q1 -> 1 q3 | 1 | ... | 9 q3 | 9
  q3 -> 0 q3 | 0 | ... | 9 q3 | 9
```

`q2` is final but has no outgoing transitions, so it has no productions. `q0 -> 0 q2` can never finish a derivation, and the constant `0` comes from `q0 -> 0`.

**Real constants** (`RG_double_constant.out`)

```
S = q0
  q0 -> + q1 | - q1 | 0 q2 | 1 q3 | ... | 9 q3
  q1 -> 0 q2 | 1 q3 | ... | 9 q3
  q2 -> . q4
  q3 -> 0 q3 | ... | 9 q3 | . q4
  q4 -> 0 q5 | 0 | ... | 9 q5 | 9
  q5 -> 0 q5 | 0 | ... | 9 q5 | 9
```
