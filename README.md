# Compiler Design Lab Codes

All questions are in one folder, ordered by assignment and question number (`A1_Q1`, `A1_Q2`, ... `A8_Q1`). Programs use C++17 unless Flex/Bison is required. Source files have no comments.

| File prefix | Program |
| --- | --- |
| A1_Q1 | Two-buffer lexical analyzer with sentinels |
| A1_Q2 | Lexical analyzer with token table |
| A2_Q1 | Maximal Munch tokenization |
| A2_Q2 | CFG and language pattern for `aabaabaa` |
| A2_Q2_User_CFG | Strings from a user-entered CFG using exactly n production applications |
| A3_Q1 | Flex token recognizer |
| A3_Q2 | Recursive-descent parser and AST |
| A3_Q2_User_CFG | General CFG parser with a branching parse tree |
| A4_Q1 | LL(1) predictive parser with the predefined example grammar |
| A4_Q1_User_CFG | LL(1) predictive parser with a user-entered CFG |
| A4_Q2 | Left recursion removal |
| A4_Q2_User_CFG | Direct left recursion removal for a user-entered CFG |
| A5_Q1 | LR(0) items and SLR(1) parser |
| A5_Q1_User_CFG | LR(0) items and computed SLR(1) table for a user-entered CFG |
| A6_Q1 | Flex/Bison variable evaluator |
| A6_Q2 | Flex/Bison expression and parentheses validator |
| A7_Q1 | AST type checking |
| A8_Q1 | Three-address code, quadruples and triples |

## Run C++

```bash
g++ -std=c++17 A2_Q1_Maximal_Munch.cpp -o program
./program
```

The C++ programs use `cin` / `getline(cin, ...)` and `cout` for terminal input and output. Enter a single expression or number and press Enter when prompted. For multiline input (`A1_Q1`, `A1_Q2`, `A2_Q1`, `A7_Q1`), type `END` on a separate line to finish. `A4_Q2` uses a built-in example grammar and needs no input.

The Flex/Bison programs (`A3_Q1`, `A6_Q1`, `A6_Q2`) use their standard C input/output. Finish their input with Ctrl+D on a new line (Linux/macOS), or Ctrl+Z then Enter (Windows).

One file-based version is kept as `A1_Q1_Two_Buffer_Lexical_Analyzer_File_Input.cpp`. To swap to file input, compile that file instead:

```bash
g++ -std=c++17 A1_Q1_Two_Buffer_Lexical_Analyzer_File_Input.cpp -o program
./program source.txt
```

The file-based version defaults to `source.txt` if no filename is supplied.

Use the same commands with any other `.cpp` filename. A `.cppm` C++ module interface needs module-aware compilation (for example `g++ -std=c++20 -fmodules-ts -x c++ -c module.cppm`); these files are ordinary `.cpp` programs, not modules.

## Run Flex

```bash
flex A3_Q1_Token_Recognizer.l
gcc lex.yy.c -o scanner -lfl
./scanner
```

## Run Flex + Bison

```bash
bison -d -o q1.tab.c A6_Q1_Variable_Evaluator.y
flex -o q1.lex.c A6_Q1_Variable_Evaluator.l
gcc q1.tab.c q1.lex.c -o evaluator -lfl
./evaluator
```

For question 2, replace `Q1_Variable_Evaluator` with `Q2_Parentheses_Validator`, and `q1` with `q2` in all commands.

**Note:** Assignment 5's grammar has a conflict in pure LR(0), so its program uses SLR(1) actions for parsing.

Both A5 Q1 versions print a parsing trace with STEP, STACK, INPUT and ACTION columns. The stack shows states alternating with grammar symbols, from bottom to top. Each row shows the configuration before a shift, reduction, acceptance or error; epsilon reductions consume no input and pop no symbols.

## Separate versions with user-entered grammars

The original grammar examples remain in their original files. Compile a `_User_CFG_` file instead to supply your own grammar. Each file is a standalone C++17 program and uses STL and terminal input/output without `static`, `const`, or `constexpr` in the source.

| File | Input after the production lines | Output |
| --- | --- | --- |
| `A2_Q2_User_CFG_String_Generator.cpp` | Start symbol, then n | Distinct strings after exactly n production applications |
| `A3_Q2_User_CFG_Parse_Tree.cpp` | Start symbol, then input tokens | Acceptance/rejection and one branching parse tree |
| `A4_Q1_User_CFG_Predictive_Parser.cpp` | Start symbol, then input tokens | FIRST/FOLLOW, predictive table, acceptance/rejection |
| `A4_Q2_User_CFG_Remove_Left_Recursion.cpp` | Nothing more | Grammar with direct left recursion removed |
| `A5_Q1_User_CFG_LR0_Parsing_Table.cpp` | Start symbol, then input tokens | LR(0) items, LR(0)/SLR(1) status, SLR table, acceptance/rejection |

For A3, A4 and A5, enter the production-line count followed by rules such as `E -> E + T | T`. Separate symbols with spaces, use uppercase initial letters for nonterminal names, and use `eps` alone for an empty alternative. Input strings also need spaces between tokens, for example `id + id * id`; a blank line represents empty input. Omit `$`.

For example, run the general parse-tree version:

```bash
g++ -std=c++17 A3_Q2_User_CFG_Parse_Tree.cpp -o parse_tree
./parse_tree
```

```text
3
E -> E + T | T
T -> T * F | F
F -> ( E ) | id
E
id + id * id
```

The A3 variant uses an Earley chart parser to support general CFGs, including left recursion, epsilon and cycles. It prints one parse tree for ambiguous inputs; the original A3 recursive-descent program still produces an arithmetic AST. A4 Q2 removes direct left recursion only, and does not guarantee an LL(1) grammar. `[no productions]` means that nonterminal has an empty language, not an epsilon production. A5 computes FOLLOW sets and reports conflicts instead of choosing an arbitrary parsing action; it parses only when the grammar is SLR(1). The Flex/Bison, semantic analysis and code generation programs retain their language-specific behavior.

## Predictive parser with a user-entered CFG

Compile and run `A4_Q1_User_CFG_Predictive_Parser.cpp`. Enter the number of production lines, the rules, the start symbol, and the input tokens. Separate every RHS symbol and input token with spaces; names such as `E'` and the terminal `id` are supported. Nonterminal names start with uppercase letters. Use `eps` alone for an empty alternative, and a blank input line for the empty string. `$` is added automatically. The program prints FIRST/FOLLOW sets and the parsing table, reports conflicting entries for grammars that are not LL(1), and parses only when there are no conflicts.

Example terminal input:

```text
3
S -> a A B e
A -> b A c | d
B -> f | eps
S
a b d c f e
```

This input is accepted.

## User-entered CFG string generator

```bash
g++ -std=c++17 A2_Q2_User_CFG_String_Generator.cpp -o cfg_generator
./cfg_generator
```

Enter the number of production lines, one rule per line, the start symbol, and `n`. Nonterminals are single uppercase letters; other characters are terminals. Spaces in rules are ignored. Separate alternatives with `|` and use `eps` for the empty string.

Example terminal input:

```text
1
S -> aabS | aabaa
S
2
```

This generates `aabaabaa`. Here `n` counts production applications, including the final terminating production, just as in the original A2 Q2 grammar. The program prints all distinct terminal strings derivable in exactly `n` applications, in alphabetical order. A completed string is not carried forward to later steps. Rules such as `S -> aS | eps` are supported, as are left recursion, cycles, and multiple nonterminals. The search expands the leftmost nonterminal at each step; every derivation tree has a leftmost derivation with the same number of applications. Use small values of `n` for branching grammars because the number of intermediate forms can grow rapidly.
