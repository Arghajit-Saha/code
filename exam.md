### 01. Write a program that implements a two-buffer input system to read an input source file. Your program must efficiently manage buffer pairs, handle lookahead pointers, detect lexemes (like keywords and identifiers), and handle end-of-buffer conditions using sentinels.
`code.cpp`
``` cpp
#include <bits/stdc++.h>
using namespace std;

class TwoBufferScanner {
  static const int BUFFER_SIZE = 1024;

  struct Cursor {
    int buffer;
    int position;
    int line;
    int column;
  };

  ifstream &input;
  int buffer[2][BUFFER_SIZE + 1];
  bool endOfFile[2];
  Cursor forward, previous, lexemeBegin;
  bool canRetract;

  unordered_set<string> keywords = {
      "int",      "float",     "double",    "char",    "void",   "if",
      "else",     "while",     "for",       "do",      "return", "break",
      "continue", "switch",    "case",      "default", "struct", "class",
      "public",   "private",   "protected", "static",  "const",  "long",
      "short",    "unsigned",  "signed",    "bool",    "true",   "false",
      "sizeof",   "namespace", "using",     "new",     "delete", "try",
      "catch",    "throw"};

  unordered_set<string> doubleOperators = {
      "++", "--", "+=", "-=", "*=", "/=", "%=", "==", "!=", "<=",
      ">=", "&&", "||", "&=", "|=", "^=", "<<", ">>", "->", "::"};

  void fillBuffer(int index) {
    int count = 0;
    char ch;

    while (count < BUFFER_SIZE && input.get(ch)) {
      buffer[index][count++] = (unsigned char)ch;
    }

    buffer[index][count] = EOF;
    endOfFile[index] = input.eof() || input.bad();
  }

  int getNextChar() {
    if (buffer[forward.buffer][forward.position] == EOF) {
      if (endOfFile[forward.buffer])
        return EOF;

      int next = 1 - forward.buffer;
      fillBuffer(next);

      forward.buffer = next;
      forward.position = 0;

      if (buffer[next][0] == EOF)
        return EOF;
    }

    previous = forward;
    canRetract = true;

    int ch = buffer[forward.buffer][forward.position++];

    if (ch == '\n') {
      forward.line++;
      forward.column = 1;
    } else {
      forward.column++;
    }

    return ch;
  }

  void retract() {
    if (canRetract) {
      forward = previous;
      canRetract = false;
    }
  }

  void markLexemeBegin() { lexemeBegin = previous; }

  void printToken(const string &type, const string &lexeme) {
    cout << left << setw(14) << type << setw(18) << lexeme << lexemeBegin.line
         << ":" << lexemeBegin.column << '\n';
  }

public:
  TwoBufferScanner(ifstream &file) : input(file) {
    endOfFile[0] = endOfFile[1] = false;
    forward = {0, 0, 1, 1};
    previous = forward;
    lexemeBegin = forward;
    canRetract = false;
    fillBuffer(0);
  }

  void analyze() {
    cout << left << setw(14) << "TOKEN TYPE" << setw(18) << "LEXEME"
         << "POSITION\n";

    while (true) {
      int ch = getNextChar();

      while (ch != EOF && isspace((unsigned char)ch))
        ch = getNextChar();

      if (ch == EOF)
        break;

      markLexemeBegin();

      string lexeme;
      lexeme += (char)ch;

      if (isalpha((unsigned char)ch) || ch == '_') {
        while (true) {
          int next = getNextChar();

          if (next != EOF && (isalnum((unsigned char)next) || next == '_')) {
            lexeme += (char)next;
          } else {
            if (next != EOF)
              retract();
            break;
          }
        }

        if (keywords.count(lexeme))
          printToken("KEYWORD", lexeme);
        else
          printToken("IDENTIFIER", lexeme);
      }

      else if (isdigit((unsigned char)ch)) {
        while (true) {
          int next = getNextChar();

          if (next != EOF && isdigit((unsigned char)next)) {
            lexeme += (char)next;
          } else {
            if (next != EOF)
              retract();
            break;
          }
        }

        printToken("NUMBER", lexeme);
      }

      else if (string("+-*/%=<>!&|^~?:").find((char)ch) != string::npos) {
        int next = getNextChar();

        if (next != EOF) {
          string candidate = lexeme + (char)next;

          if (doubleOperators.count(candidate))
            lexeme = candidate;
          else
            retract();
        }

        printToken("OPERATOR", lexeme);
      }

      else if (string("(){}[];,.").find((char)ch) != string::npos) {
        printToken("DELIMITER", lexeme);
      }

      else {
        printToken("UNKNOWN", lexeme);
      }
    }
  }
};

int main() {
  string filename;

  cout << "Enter source file name: ";
  getline(cin, filename);

  ifstream file(filename, ios::binary);

  if (!file) {
    cout << "Unable to open source file.\n";
    return 1;
  }

  TwoBufferScanner scanner(file);
  scanner.analyze();

  file.close();

  return 0;
}
```
### User Input
``` bash
> g++ code.cpp && ./a.out
> sample.cpp
```
`sample.cpp`
``` cpp
int main() {
  int count = 10;
  if (count > 0) {
    return count;
  }
}
```

### 02. Write a Lexical Analyzer (Scanner) in C, C++, or Java that reads a source code file of a subset language and breaks it down into a stream of lexemes and valid tokens. Show the output in tabular form.
`code.cpp`
``` cpp
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_set>
using namespace std;

struct Token {
  string type;
  string lexeme;
  int line;
  int column;
};

class Lexer {
  string source;
  int pos = 0;
  int line = 1;
  int column = 1;

  unordered_set<string> keywords = {
      "int",      "float",  "double", "char",    "void",   "if",
      "else",     "while",  "for",    "do",      "return", "break",
      "continue", "switch", "case",   "default", "long",   "short",
      "bool",     "true",   "false",  "const",   "static"};

  unordered_set<string> operators = {
      "+",  "-",  "*",  "/",  "%",   "=",   "==", "!=", "<",  ">",  "<=", ">=",
      "++", "--", "+=", "-=", "*=",  "/=",  "%=", "&&", "||", "!",  "&",  "|",
      "^",  "~",  "<<", ">>", "<<=", ">>=", "&=", "|=", "^=", "->", "::", "?"};

  unordered_set<string> delimiters = {"(", ")", "{", "}", "[", "]", ";", ",", "."};

  char peek(int offset = 0) {
    if (pos + offset >= (int)source.size())
      return '\0';
    return source[pos + offset];
  }

  char get() {
    char ch = source[pos++];

    if (ch == '\n') {
      line++;
      column = 1;
    } else {
      column++;
    }

    return ch;
  }

  void skipWhitespaceAndComments() {
    while (pos < (int)source.size()) {
      if (isspace((unsigned char)peek())) {
        get();
      } else if (peek() == '/' && peek(1) == '/') {
        while (pos < (int)source.size() && peek() != '\n')
          get();
      } else if (peek() == '/' && peek(1) == '*') {
        get();
        get();

        while (pos < (int)source.size() && !(peek() == '*' && peek(1) == '/')) {
          get();
        }

        if (pos < (int)source.size()) {
          get();
          get();
        }
      } else {
        break;
      }
    }
  }

public:
  Lexer(string text) { source = text; }

  bool nextToken(Token &t) {
    skipWhitespaceAndComments();

    if (pos >= (int)source.size())
      return false;

    t.line = line;
    t.column = column;
    t.lexeme = "";

    char ch = peek();

    if (isalpha((unsigned char)ch) || ch == '_') {
      while (isalnum((unsigned char)peek()) || peek() == '_')
        t.lexeme += get();

      t.type = keywords.count(t.lexeme) ? "KEYWORD" : "IDENTIFIER";
    }

    else if (isdigit((unsigned char)ch)) {
      while (isdigit((unsigned char)peek()))
        t.lexeme += get();

      if (peek() == '.' && isdigit((unsigned char)peek(1))) {
        t.lexeme += get();

        while (isdigit((unsigned char)peek()))
          t.lexeme += get();
      }

      t.type = "NUMBER";
    }

    else if (ch == '"') {
      t.lexeme += get();
      bool closed = false;

      while (pos < (int)source.size() && peek() != '\n') {
        char c = get();
        t.lexeme += c;

        if (c == '\\' && pos < (int)source.size()) {
          t.lexeme += get();
        } else if (c == '"') {
          closed = true;
          break;
        }
      }

      t.type = closed ? "STRING" : "INVALID";
    }

    else if (ch == '\'') {
      t.lexeme += get();
      bool closed = false;

      while (pos < (int)source.size() && peek() != '\n') {
        char c = get();
        t.lexeme += c;

        if (c == '\\' && pos < (int)source.size()) {
          t.lexeme += get();
        } else if (c == '\'') {
          closed = true;
          break;
        }
      }

      t.type = closed ? "CHARACTER" : "INVALID";
    }

    else {
      string three = "";
      three += peek();
      three += peek(1);
      three += peek(2);

      string two = "";
      two += peek();
      two += peek(1);

      if (operators.count(three)) {
        t.lexeme += get();
        t.lexeme += get();
        t.lexeme += get();
        t.type = "OPERATOR";
      } else if (operators.count(two)) {
        t.lexeme += get();
        t.lexeme += get();
        t.type = "OPERATOR";
      } else if (operators.count(string(1, ch))) {
        t.lexeme += get();
        t.type = "OPERATOR";
      } else if (delimiters.count(string(1, ch))) {
        t.lexeme += get();
        t.type = "DELIMITER";
      } else {
        t.lexeme += get();
        t.type = "INVALID";
      }
    }

    return true;
  }
};

int main() {
  string filename;

  cout << "Enter source file name: ";
  getline(cin, filename);

  ifstream file(filename);

  if (!file) {
    cout << "Error opening file!\n";
    return 1;
  }

  string source((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

  Lexer lexer(source);
  Token t;

  cout << "\n";
  cout << left << setw(10) << "LINE" << setw(10) << "COLUMN" << setw(18)
       << "TOKEN TYPE" << "LEXEME\n";

  cout << string(55, '-') << "\n";

  while (lexer.nextToken(t)) {
    cout << left << setw(10) << t.line << setw(10) << t.column << setw(18) << t.type << t.lexeme << "\n";
  }

  file.close();

  return 0;
}
```
### User Input
``` bash
> g++ code.cpp && ./a.out
> sample.cpp
```
`sample.cpp`
``` cpp
int main() {
  int a = 10;
  float b = 3.14;

  if (a >= 5) {
    a = a + 1;
  }
  return a;
}
```

### 03. Write a Lex (or Flex) program that recognizes tokens for a small programming language fragment and prints each token along with its type and lexeme.
`program.l`
``` cpp
% option noyywrap

% {
#include <stdio.h>
% }

DIGIT[0 - 9] 
LETTER[A - Za - z_] 
ID{LETTER}({LETTER} | {DIGIT}) *

%%
"int" | "float" | "double" | "char" | "void" | "if" | "else" | "while" | "for" | "return" | "break" | "continue" {
  printf("%-18s %s\n", "KEYWORD", yytext);
}

{ ID }
{ printf("%-18s %s\n", "IDENTIFIER", yytext); }

{DIGIT} + (\.{DIGIT} +) ? { printf("%-18s %s\n", "NUMBER", yytext); }

\"([^\\\n]|\\.)*\" {
    printf("%-18s %s\n", "STRING", yytext);
}

\'([^\\\n]|\\.)\' { printf("%-18s %s\n", "CHARACTER", yytext);
}

"//".*{}

"<<=" | ">>=" | "==" | "!=" | "<=" | ">=" | "++" | "--" | "+=" | "-=" | "*=" | "/=" | "%=" | "&&" | "||" | "<<" | ">>" | "->" | "::" | [+\- * / %= <> !& | ^~] { printf("%-18s %s\n", "OPERATOR", yytext); }

"(" | ")" | "{" | "}" | "[" | "]" | ";" | "," | "." | ":" {
  printf("%-18s %s\n", "DELIMITER", yytext);
}

[ \t\r\n] + {}

. {
  printf("%-18s %s\n", "INVALID", yytext);
}

%%
int main() {
  printf("%-18s %s\n", "TOKEN TYPE", "LEXEME");
  printf("--------------------------------\n");
  yylex();
  return 0;
}
```
### User Input
``` bash
> sudo apt update
> sudo apt install g++ gcc flex
> flex program1.l
> gcc lex.yy.c -o program1 && ./program1
> int main() {
>    int a = 10;
>    float b = 3.14;
>    if (a >= 5) {
>        a++;
>    }
>    return a;
> }
> Ctrl + D
```
### 04. Write a program to implement a lexical analyzer component that applies the Maximal Munch (Longest Match) rule to resolve tokenization ambiguities.
`code.cpp`
``` cpp
#include <bits/stdc++.h>
using namespace std;

int main() {
  string source, line;

  cout << "Enter source code (type END to finish):\n";

  while (getline(cin, line)) {
    if (line == "END")
      break;

    source += line + '\n';
  }

  unordered_set<string> keywords = {"int",  "float",  "double", "char",
                                    "void", "if",     "else",   "while",
                                    "for",  "return", "break",  "continue"};

  vector<string> operators = {"<<=", ">>=", "++", "--", "+=", "-=", "*=", "/=",
                              "%=",  "==",  "!=", "<=", ">=", "&&", "||", "<<",
                              ">>",  "->",  "::", "+",  "-",  "*",  "/",  "%",
                              "=",   "<",   ">",  "!",  "&",  "|",  "^",  "~"};

  string delimiters = "(){}[];,.:";

  cout << "\n";
  cout << left << setw(18) << "TOKEN TYPE" << "LEXEME\n";
  cout << "----------------------------------\n";

  int i = 0;

  while (i < (int)source.size()) {
    if (isspace((unsigned char)source[i])) {
      i++;
      continue;
    }

    if (isalpha((unsigned char)source[i]) || source[i] == '_') {
      string lexeme;

      while (i < (int)source.size() &&
             (isalnum((unsigned char)source[i]) || source[i] == '_')) {
        lexeme += source[i++];
      }

      if (keywords.count(lexeme))
        cout << left << setw(18) << "KEYWORD" << lexeme << '\n';
      else
        cout << left << setw(18) << "IDENTIFIER" << lexeme << '\n';
    }

    else if (isdigit((unsigned char)source[i])) {
      string lexeme;

      while (i < (int)source.size() && isdigit((unsigned char)source[i])) {
        lexeme += source[i++];
      }

      if (i + 1 < (int)source.size() && source[i] == '.' &&
          isdigit((unsigned char)source[i + 1])) {
        lexeme += source[i++];

        while (i < (int)source.size() && isdigit((unsigned char)source[i])) {
          lexeme += source[i++];
        }
      }

      cout << left << setw(18) << "NUMBER" << lexeme << '\n';
    }

    else {
      string longest = "";

      for (const string &op : operators) {
        if (source.compare(i, op.length(), op) == 0 &&
            op.length() > longest.length()) {
          longest = op;
        }
      }

      if (!longest.empty()) {
        cout << left << setw(18) << "OPERATOR" << longest << '\n';
        i += longest.length();
      }

      else if (delimiters.find(source[i]) != string::npos) {
        cout << left << setw(18) << "DELIMITER" << source[i] << '\n';
        i++;
      }

      else {
        cout << left << setw(18) << "INVALID" << source[i] << '\n';
        i++;
      }
    }
  }

  return 0;
}
```
### User Input
``` bash
> g++ code.cpp && ./a.out
> int a = 10;
> if (a >= 10) {
>   a++;
>   a += 5;
>   a = a == 15;
> }
> END
```

### 05. Write a program to generate a suitable CFG and the language pattern generated using the CFG in terms of n where n>0, for the following input of terminals: aabaabaa.
`code.cpp`
``` cpp
#include <bits/stdc++.h>
using namespace std;

string nonTerminal(int i) {
  if (i == 0)
    return "S";

  return "A" + to_string(i);
}

int main() {
  string input;

  cout << "Enter terminal string: ";
  cin >> input;

  size_t first = input.find('b');
  size_t second = input.find('b', first == string::npos ? 0 : first + 1);

  bool valid = false;
  int n = 0;

  if (first != string::npos && second != string::npos &&
      input.find('b', second + 1) == string::npos) {

    string x = input.substr(0, first);
    string y = input.substr(first + 1, second - first - 1);
    string z = input.substr(second + 1);

    bool onlyA = all_of(x.begin(), x.end(), [](char c) { return c == 'a'; }) &&
                 all_of(y.begin(), y.end(), [](char c) { return c == 'a'; }) &&
                 all_of(z.begin(), z.end(), [](char c) { return c == 'a'; });

    if (onlyA && !x.empty() && x.length() == y.length() &&
        y.length() == z.length()) {
      valid = true;
      n = x.length();
    }
  }

  if (valid) {
    cout << "\nPattern detected: a^" << n << " b a^" << n << " b a^" << n
         << '\n';

    cout << "Generalized language:\n";
    cout << "L = {a^n b a^n b a^n | n > 0}\n";

    cout << "\nThe generalized language is not context-free.\n";
    cout << "Therefore, no CFG exists for all n > 0.\n";
  } else {
    cout << "\nThe input does not match the required pattern.\n";
  }

  cout << "\nCFG for the given input string only:\n";

  for (int i = 0; i < (int)input.length(); i++) {
    cout << nonTerminal(i) << " -> " << input[i] << nonTerminal(i + 1) << '\n';
  }

  cout << nonTerminal(input.length()) << " -> epsilon\n";

  return 0;
}
```
### User Input
``` bash
> g++ code.cpp && ./a.out
> aabaabaa
```

### 06. Write a recursive-descent parser for the following grammar and build a basic abstract syntax tree (AST) and evaluate the expression id + id * num, (id - num) / id.

Grammar:

E  → T E'

E' → + T E' | - T E' | ε

T  → F T'

T' → * F T' | / F T' | ε

F  → ( E ) | id | num

`code.cpp`
``` cpp
#include <bits/stdc++.h>
using namespace std;

struct Node {
  string type;
  string value;
  unique_ptr<Node> left, right;

  Node(string t, string v) {
    type = t;
    value = v;
  }
};

struct Token {
  string type, value;
};

class Parser {
  string input;
  int pos = 0;
  Token current;

  Token getNextToken() {
    while (pos < (int)input.size() && isspace((unsigned char)input[pos]))
      pos++;

    if (pos >= (int)input.size())
      return {"END", ""};

    char ch = input[pos];

    if (isalpha((unsigned char)ch) || ch == '_') {
      string value;

      while (pos < (int)input.size() &&
             (isalnum((unsigned char)input[pos]) || input[pos] == '_'))
        value += input[pos++];

      return {"ID", value};
    }

    if (isdigit((unsigned char)ch)) {
      string value;

      while (pos < (int)input.size() && isdigit((unsigned char)input[pos]))
        value += input[pos++];

      if (pos < (int)input.size() && input[pos] == '.' &&
          pos + 1 < (int)input.size() &&
          isdigit((unsigned char)input[pos + 1])) {
        value += input[pos++];

        while (pos < (int)input.size() && isdigit((unsigned char)input[pos]))
          value += input[pos++];
      }

      return {"NUM", value};
    }

    if (string("+-*/()").find(ch) != string::npos) {
      pos++;
      return {string(1, ch), string(1, ch)};
    }

    throw runtime_error("Invalid character in expression");
  }

  void advance() { current = getNextToken(); }

  unique_ptr<Node> createOperator(string op, unique_ptr<Node> left,
                                  unique_ptr<Node> right) {
    auto node = make_unique<Node>("OP", op);
    node->left = move(left);
    node->right = move(right);
    return node;
  }

  unique_ptr<Node> E() {
    auto node = T();
    return EPrime(move(node));
  }

  unique_ptr<Node> EPrime(unique_ptr<Node> left) {
    if (current.type == "+" || current.type == "-") {
      string op = current.type;
      advance();

      auto right = T();
      auto node = createOperator(op, move(left), move(right));

      return EPrime(move(node));
    }

    return left;
  }

  unique_ptr<Node> T() {
    auto node = F();
    return TPrime(move(node));
  }

  unique_ptr<Node> TPrime(unique_ptr<Node> left) {
    if (current.type == "*" || current.type == "/") {
      string op = current.type;
      advance();

      auto right = F();
      auto node = createOperator(op, move(left), move(right));

      return TPrime(move(node));
    }

    return left;
  }

  unique_ptr<Node> F() {
    if (current.type == "(") {
      advance();

      auto node = E();

      if (current.type != ")")
        throw runtime_error("Missing closing parenthesis");

      advance();
      return node;
    }

    if (current.type == "ID") {
      auto node = make_unique<Node>("ID", current.value);
      advance();
      return node;
    }

    if (current.type == "NUM") {
      auto node = make_unique<Node>("NUM", current.value);
      advance();
      return node;
    }

    throw runtime_error("Invalid expression");
  }

public:
  Parser(string s) {
    input = s;
    advance();
  }

  unique_ptr<Node> parse() {
    auto root = E();

    if (current.type != "END")
      throw runtime_error("Unexpected token: " + current.value);

    return root;
  }
};

void printAST(const Node *root, int level = 0) {
  if (!root)
    return;

  cout << string(level * 3, ' ') << root->value;

  if (root->type == "ID")
    cout << " [ID]";
  else if (root->type == "NUM")
    cout << " [NUM]";

  cout << '\n';

  printAST(root->left.get(), level + 1);
  printAST(root->right.get(), level + 1);
}

void collectIdentifiers(const Node *root, set<string> &ids) {
  if (!root)
    return;

  if (root->type == "ID")
    ids.insert(root->value);

  collectIdentifiers(root->left.get(), ids);
  collectIdentifiers(root->right.get(), ids);
}

double evaluate(const Node *root, unordered_map<string, double> &values) {
  if (root->type == "NUM")
    return stod(root->value);

  if (root->type == "ID")
    return values.at(root->value);

  double left = evaluate(root->left.get(), values);
  double right = evaluate(root->right.get(), values);

  if (root->value == "+")
    return left + right;

  if (root->value == "-")
    return left - right;

  if (root->value == "*")
    return left * right;

  if (root->value == "/") {
    if (right == 0)
      throw runtime_error("Division by zero");

    return left / right;
  }

  throw runtime_error("Unknown operator");
}

int main() {
  string expression;

  cout << "Enter arithmetic expression: ";
  getline(cin, expression);

  try {
    Parser parser(expression);

    auto root = parser.parse();

    cout << "\nAbstract Syntax Tree (AST):\n";
    printAST(root.get());

    set<string> identifiers;
    collectIdentifiers(root.get(), identifiers);

    unordered_map<string, double> values;

    for (const string &id : identifiers) {
      cout << "Enter value of " << id << ": ";
      if (!(cin >> values[id]))
        throw runtime_error("Invalid identifier value");
    }

    double result = evaluate(root.get(), values);

    cout << "\nEvaluated Result: " << setprecision(10) << result << '\n';

  } catch (const exception &e) {
    cout << "\nError: " << e.what() << '\n';
    return 1;
  }

  return 0;
}
```
### User Input
``` bash
> g++ code.cpp && ./a.out
> Enter arithmetic expression: a + b * 5
> Enter value of a: 10
> Enter value of b: 2

> g++ code.cpp && ./a.out
> Enter arithmetic expression: (a - 5) / a
> Enter value of a: 10
```

### 07. Write a program to transform the following non-LL(1)/left-recursive grammar into an equivalent LL(1) form.

Grammar:

E → E + T | T

T → T * F | F

F → ( E ) | id

`code.cpp`
``` cpp
#include <cctype>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

struct Production {
  string lhs;
  vector<vector<string>> alternatives;
};

vector<string> tokenize(string rhs) {
  vector<string> tokens;

  for (int i = 0; i < (int)rhs.length(); i++) {
    if (isspace((unsigned char)rhs[i]))
      continue;

    if (i + 1 < (int)rhs.length() && rhs[i] == 'i' && rhs[i + 1] == 'd') {
      tokens.push_back("id");
      i++;
    } else {
      tokens.push_back(string(1, rhs[i]));
    }
  }

  return tokens;
}

string join(vector<string> tokens) {
  string result;

  for (int i = 0; i < (int)tokens.size(); i++) {
    if (i > 0)
      result += " ";

    result += tokens[i];
  }

  return result;
}

void displayProduction(string lhs, vector<vector<string>> alternatives) {
  cout << lhs << " -> ";

  for (int i = 0; i < (int)alternatives.size(); i++) {
    if (i > 0)
      cout << " | ";

    cout << join(alternatives[i]);
  }

  cout << '\n';
}

int main() {
  int n;

  cout << "Enter number of non-terminals: ";
  cin >> n;

  vector<Production> grammar(n);

  for (int i = 0; i < n; i++) {
    int count;

    cout << "\nEnter non-terminal and number of alternatives: ";
    cin >> grammar[i].lhs >> count;

    for (int j = 0; j < count; j++) {
      string rhs;

      cout << "Enter alternative " << j + 1 << ": ";
      cin >> rhs;

      grammar[i].alternatives.push_back(tokenize(rhs));
    }
  }

  cout << "\nOriginal Grammar:\n";

  for (auto p : grammar)
    displayProduction(p.lhs, p.alternatives);

  cout << "\nTransformed LL(1) Grammar:\n";

  for (auto p : grammar) {
    vector<vector<string>> recursive, nonRecursive;

    for (auto rhs : p.alternatives) {
      if (!rhs.empty() && rhs[0] == p.lhs)
        recursive.push_back(vector<string>(rhs.begin() + 1, rhs.end()));
      else
        nonRecursive.push_back(rhs);
    }

    if (recursive.empty()) {
      displayProduction(p.lhs, p.alternatives);
      continue;
    }

    if (nonRecursive.empty()) {
      cout << "Cannot eliminate left recursion: no beta production.\n";
      return 1;
    }

    string newNT = p.lhs + "'";

    vector<vector<string>> newAlternatives;

    for (auto beta : nonRecursive) {
      beta.push_back(newNT);
      newAlternatives.push_back(beta);
    }

    displayProduction(p.lhs, newAlternatives);

    vector<vector<string>> recursiveAlternatives;

    for (auto alpha : recursive) {
      alpha.push_back(newNT);
      recursiveAlternatives.push_back(alpha);
    }

    recursiveAlternatives.push_back({"epsilon"});

    displayProduction(newNT, recursiveAlternatives);
  }

  return 0;
}
```
### User Input
``` bash
> g++ code.cpp && ./a.out
> Enter number of non-terminals: 3

> Enter non-terminal and number of alternatives: E 2
> Enter alternative 1: E+T
> Enter alternative 2: T

> Enter non-terminal and number of alternatives: T 2
> Enter alternative 1: T*F
> Enter alternative 2: F

> Enter non-terminal and number of alternatives: F 2
> Enter alternative 1: (E)
> Enter alternative 2: id
```

### 08. Consider the following grammar and construct LR(0) parsing table. Also, parse the input string 𝑎𝑏𝑏 using the parse table.

Grammar:

𝑆 → 𝐴

𝐴 → 𝐴𝑏 | 𝑎

`code.cpp`
``` cpp
#include <bits/stdc++.h>
using namespace std;

struct Production {
  char lhs;
  string rhs;
};

using Item = pair<int, int>;
using ItemSet = set<Item>;

vector<Production> grammar = {{'Z', "S"}, {'S', "A"}, {'A', "Ab"}, {'A', "a"}};

vector<char> symbols = {'S', 'A', 'a', 'b'};
vector<char> terminals = {'a', 'b', '$'};
vector<char> nonTerminals = {'S', 'A'};

vector<ItemSet> states;
map<pair<int, char>, int> transitions;
map<pair<int, char>, string> lr0Table, slrTable;
map<pair<int, char>, int> goToTable;

bool isNonTerminal(char c) { return c == 'Z' || c == 'S' || c == 'A'; }

ItemSet closure(ItemSet items) {
  bool changed = true;

  while (changed) {
    changed = false;

    vector<Item> current(items.begin(), items.end());

    for (auto item : current) {
      int p = item.first;
      int dot = item.second;

      if (dot < (int)grammar[p].rhs.size()) {
        char next = grammar[p].rhs[dot];

        if (isNonTerminal(next)) {
          for (int i = 0; i < (int)grammar.size(); i++) {
            if (grammar[i].lhs == next) {
              if (items.insert({i, 0}).second)
                changed = true;
            }
          }
        }
      }
    }
  }

  return items;
}

ItemSet goTo(const ItemSet &items, char symbol) {
  ItemSet moved;

  for (auto item : items) {
    int p = item.first;
    int dot = item.second;

    if (dot < (int)grammar[p].rhs.size() && grammar[p].rhs[dot] == symbol) {
      moved.insert({p, dot + 1});
    }
  }

  if (moved.empty())
    return moved;

  return closure(moved);
}

void constructStates() {
  states.clear();
  transitions.clear();

  states.push_back(closure({{0, 0}}));

  for (int i = 0; i < (int)states.size(); i++) {
    for (char symbol : symbols) {
      ItemSet next = goTo(states[i], symbol);

      if (next.empty())
        continue;

      auto it = find(states.begin(), states.end(), next);
      int stateNumber;

      if (it == states.end()) {
        states.push_back(next);
        stateNumber = states.size() - 1;
      } else {
        stateNumber = it - states.begin();
      }

      transitions[{i, symbol}] = stateNumber;
    }
  }
}

void addAction(map<pair<int, char>, string> &table, int state, char lookahead,
               string action) {
  auto key = make_pair(state, lookahead);

  if (!table.count(key)) {
    table[key] = action;
  } else if (table[key] != action && table[key].find(action) == string::npos) {
    table[key] += "/" + action;
  }
}

void constructTables() {
  lr0Table.clear();
  slrTable.clear();
  goToTable.clear();

  map<char, set<char>> follow;
  follow['S'] = {'$'};
  follow['A'] = {'b', '$'};

  for (int i = 0; i < (int)states.size(); i++) {
    for (char nt : nonTerminals) {
      if (transitions.count({i, nt}))
        goToTable[{i, nt}] = transitions[{i, nt}];
    }

    for (char t : terminals) {
      if (t != '$' && transitions.count({i, t})) {
        string shift = "s" + to_string(transitions[{i, t}]);

        addAction(lr0Table, i, t, shift);
        addAction(slrTable, i, t, shift);
      }
    }

    for (auto item : states[i]) {
      int p = item.first;
      int dot = item.second;

      if (dot == (int)grammar[p].rhs.size()) {
        if (p == 0) {
          addAction(lr0Table, i, '$', "acc");
          addAction(slrTable, i, '$', "acc");
        } else {
          string reduce = "r" + to_string(p);

          for (char t : terminals)
            addAction(lr0Table, i, t, reduce);

          for (char t : follow[grammar[p].lhs])
            addAction(slrTable, i, t, reduce);
        }
      }
    }
  }
}

string itemText(Item item) {
  int p = item.first;
  int dot = item.second;

  string rhs = grammar[p].rhs;
  rhs.insert(rhs.begin() + dot, '.');

  return string(1, grammar[p].lhs) + " -> " + rhs;
}

void displayStates() {
  cout << "\nCANONICAL LR(0) ITEM SETS\n";

  for (int i = 0; i < (int)states.size(); i++) {
    cout << "\nI" << i << ":\n";

    for (auto item : states[i])
      cout << "  " << itemText(item) << '\n';
  }
}

void displayTable(const string &title,
                  const map<pair<int, char>, string> &table) {
  cout << "\n" << title << "\n\n";

  cout << left << setw(9) << "STATE";

  for (char t : terminals)
    cout << setw(12) << t;

  for (char nt : nonTerminals)
    cout << setw(12) << (string("GOTO ") + nt);

  cout << '\n';

  for (int i = 0; i < (int)states.size(); i++) {
    cout << left << setw(9) << i;

    for (char t : terminals) {
      auto it = table.find({i, t});

      cout << setw(12) << (it == table.end() ? "-" : it->second);
    }

    for (char nt : nonTerminals) {
      auto it = goToTable.find({i, nt});

      cout << setw(12) << (it == goToTable.end() ? "-" : to_string(it->second));
    }

    cout << '\n';
  }
}

string stackText(const vector<int> &stateStack,
                 const vector<char> &symbolStack) {
  string result = to_string(stateStack[0]);

  for (int i = 0; i < (int)symbolStack.size(); i++) {
    result +=
        " " + string(1, symbolStack[i]) + " " + to_string(stateStack[i + 1]);
  }

  return result;
}

bool parseSLR(string input) {
  for (char c : input) {
    if (c != 'a' && c != 'b') {
      cout << "Invalid input symbol: " << c << '\n';
      return false;
    }
  }

  input += '$';

  vector<int> stateStack = {0};
  vector<char> symbolStack;

  int pos = 0;

  cout << "\nSLR(1) PARSING TRACE\n\n";

  cout << left << setw(22) << "STACK" << setw(18) << "INPUT" << "ACTION\n";

  cout << string(70, '-') << '\n';

  while (true) {
    int state = stateStack.back();
    char lookahead = input[pos];

    auto it = slrTable.find({state, lookahead});

    string action = (it == slrTable.end()) ? "ERROR" : it->second;

    cout << left << setw(22) << stackText(stateStack, symbolStack) << setw(18)
         << input.substr(pos) << action;

    if (action == "acc") {
      cout << '\n';
      return true;
    }

    if (action == "ERROR" || action.find('/') != string::npos) {
      cout << '\n';
      return false;
    }

    if (action[0] == 's') {
      int nextState = stoi(action.substr(1));

      symbolStack.push_back(lookahead);
      stateStack.push_back(nextState);

      pos++;

      cout << '\n';
    }

    else if (action[0] == 'r') {
      int productionNumber = stoi(action.substr(1));

      Production p = grammar[productionNumber];
      int length = p.rhs.size();

      for (int i = 0; i < length; i++) {
        symbolStack.pop_back();
        stateStack.pop_back();
      }

      int previousState = stateStack.back();

      if (!goToTable.count({previousState, p.lhs})) {
        cout << "\nMissing GOTO entry\n";
        return false;
      }

      symbolStack.push_back(p.lhs);

      stateStack.push_back(goToTable[{previousState, p.lhs}]);

      cout << " (" << p.lhs << " -> " << p.rhs << ")\n";
    }

    else {
      cout << '\n';
      return false;
    }
  }
}

int main() {
  constructStates();
  constructTables();

  displayStates();

  displayTable("LR(0) PARSING TABLE (CONFLICTS SHOWN)", lr0Table);

  cout << "\nThe grammar has a shift-reduce conflict in LR(0).";
  cout << "\nIt is not an LR(0) grammar.\n";

  displayTable("SLR(1) PARSING TABLE", slrTable);

  string input;

  cout << "\nEnter input string to parse: ";
  cin >> input;

  if (parseSLR(input))
    cout << "\nString ACCEPTED by the SLR(1) parser.\n";
  else
    cout << "\nString REJECTED by the SLR(1) parser.\n";

  return 0;
}
```
### User Input
``` bash
> g++ code.cpp && ./a.out
> Enter input string to parse: abb
```

### 09. To design and implement a syntax checker and evaluator for a basic configuration language or subset of a scripting language that supports variable assignments and dynamic variable evaluation using the Bison parser generator and Flex lexical analyzer.
`lexer.l`
``` cpp
%option noyywrap yylineno

%{
#include "parser.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
%}

DIGIT [0-9]
ID [a-zA-Z_][a-zA-Z0-9_]*
NUMBER {DIGIT}+(\.{DIGIT}+)?

%%
"print" {
    return PRINT;
}

{NUMBER} {
    yylval.number = atof(yytext);
    return NUMBER;
}

{ID} {
    yylval.text = strdup(yytext);
    return IDENTIFIER;
}

"//".* {}

"#".* {}

[ \t\r\n]+ {}

"+" { return '+'; }

"-" { return '-'; }

"*" { return '*'; }

"/" { return '/'; }

"=" { return '='; }

"(" { return '('; }

")" { return ')'; }

";" { return ';'; }

. {
    fprintf(stderr, "Lexical error at line %d: %s\n", yylineno, yytext);
    return INVALID;
}
%% 
```
`parser.y`
``` cpp
%{
#include <iostream>
#include <map>
#include <string>
#include <cstdlib>
using namespace std;

extern int yylex();
extern int yylineno;

void yyerror(const char *);

map<string, double> variables;

bool syntaxError = false;
bool semanticError = false;
%}

%define parse.error verbose

%union {
    double number;
    char *text;
}

%token <number> NUMBER
%token <text> IDENTIFIER
%token PRINT INVALID

%type <number> expression

%destructor { free($$); } <text>

%left '+' '-'
%left '*' '/'
%right UMINUS

%%

program:
    statements
;

statements:
    %empty
    | statements statement
;

statement:
    IDENTIFIER '=' expression ';'
    {
      variables[$1] = $3;
      cout << "Assigned " << $1 << " = " << $3 << '\n';
      free($1);
    }
    | PRINT expression ';'
    { cout << "Output: " << $2 << '\n'; }
    | expression ';'
    { cout << "Result: " << $1 << '\n'; }
    | error ';'
    {
      syntaxError = true;
      cout << "Invalid statement. Skipping...\n";
      yyerrok;
    }
;

expression:
    NUMBER
    { $$ = $1; }
    | IDENTIFIER
    {
      auto it = variables.find($1);

      if (it == variables.end()) {
        cerr << "Semantic error at line " << yylineno
            << ": Undefined variable " << $1 << '\n';
            semanticError = true;
            $$ = 0.0;
        } else {
          $$ = it->second;
        }
        free($1);
    }
    | expression '+' expression
    { $$ = $1 + $3; }
    | expression '-' expression
    { $$ = $1 - $3; }
    | expression '*' expression
    { $$ = $1 * $3; }
    | expression '/' expression
    {
      if ($3 == 0) {
        cerr << "Semantic error: Division by zero\n";
        semanticError = true;
        $$ = 0.0;
      } else {
        $$ = $1 / $3;
      }
    }
    | '-' expression %prec UMINUS
    { $$ = -$2; }
    | '(' expression ')'
    { $$ = $2; }
;
%%

void yyerror(const char *msg) {
    syntaxError = true;
    cerr << "Syntax error at line " << yylineno
         << ": " << msg << '\n';
}

int main() {
    cout << "Enter configuration statements:\n";
    cout << "Press Ctrl+D to finish input.\n\n";

    int result = yyparse();

    if (result == 0 && !syntaxError && !semanticError)
        cout << "\nConfiguration parsed successfully.\n";
    else cout << "\nConfiguration contains errors.\n";

    return (result == 0 && !syntaxError && !semanticError) ? 0 : 1;
}
```
### User Input
``` bash
> sudo apt update
> sudo apt install g++ flex bison
> bison -d parser.y
> flex lexer.l
> g++ parser.tab.c lex.yy.c -o config
> ./config
> x = 10;
> y = 5;
> z = x + y * 2;
> total = (z - 5) / 3;
> print x;
> print y;
> print z;
> print total;
> Ctrl + D
```
### 10. Design and implement a syntax analyzer and semantic validator that processes and evaluates nested parentheses balance and mathematical structural formatting using the Bison parser generator and Flex lexical analyzer.
` lexer.l`
``` cpp

%option noyywrap

%{
#include "parser.tab.h"
#include <cstdlib>
#include <cstdio>
%}

NUMBER (([0-9]+(\.[0-9]*)?)|(\.[0-9]+))([eE][+-]?[0-9]+)?

%%

{NUMBER} {
    yylval.value = strtod(yytext, nullptr);
    return NUMBER;
}

"+" { return '+'; }

"-" { return '-'; }

"*" { return '*'; }

"/" { return '/'; }

"(" { return '('; }

")" { return ')'; }

[ \t\r\n]+ {}

. {
    fprintf(stderr, "Lexical error: Invalid character '%s'\n", yytext);
    return INVALID;
}
%%
```
`parser.y`
``` cpp
%{
#include <iostream>
#include <iomanip>
#include <cmath>
using namespace std;

int yylex();
void yyerror(const char *);

bool semanticError = false;
%}

%define parse.error verbose

%union { double value; }
%token <value> NUMBER
%token INVALID
%type <value> expression term unary primary
%start input
%%
input:
    expression {
      if (!semanticError) {
        cout << "\nExpression is structurally valid.\n";
        cout << "Parentheses are balanced.\n";
        cout << "Evaluated Result: "
              << setprecision(12) << $1 << '\n';
      }
      else {
        cout << "\nExpression structure is valid, "
              << "but semantic validation failed.\n";
      }
    }
;

expression:
    expression '+' term
    { $$ = $1 + $3; }
    | expression '-' term
    { $$ = $1 - $3; }
    | term
    { $$ = $1; }
;

term:
    term '*' unary
    { $$ = $1 * $3; }
    | term '/' unary
    {
      if ($3 == 0) {
          cerr << "Semantic Error: Division by zero.\n";
          semanticError = true;
          $$ = 0;
      }
      else {
          $$ = $1 / $3;
      }
    }
    | unary
    { $$ = $1; }
;

unary:
    '+' unary
    { $$ = $2; }
    | '-' unary
    { $$ = -$2; }
    | primary
    { $$ = $1; }
;

primary:
    NUMBER
    { $$ = $1; }
    | '(' expression ')'
    { $$ = $2; }
;

%%

void yyerror(const char *message) {
    cerr << "\nSyntax Error: " << message << '\n';
}

int main() {
    cout << "Enter mathematical expression:\n";

    int status = yyparse();

    if (status == 0 && !semanticError) {
        cout << "\nValidation Successful.\n";
        return 0;
    }
    else if (semanticError) {
        cout << "\nSemantic Validation Failed.\n";
    }
    else {
        cout << "\nSyntax Validation Failed.\n";
    }
    return 1;
}
```
### User Input
``` bash
> sudo apt update
> sudo apt install g++ flex bison
> bison -d parser.y
> flex lexer.l
> g++ parser.tab.c lex.yy.c -o mathchecker
> ./mathchecker
> (10 + (20 * 3)) / 2
```
### 11. Write a program to implement a semantic analyzer that performs type checking, and semantic error detection on an Abstract Syntax Tree (AST) produced by a parser.

`code.cpp`
``` cpp
#include <bits/stdc++.h>
using namespace std;

enum class DataType { INT, FLOAT, BOOL, ERROR };

string typeName(DataType type) {
  if (type == DataType::INT)
    return "int";
  if (type == DataType::FLOAT)
    return "float";
  if (type == DataType::BOOL)
    return "bool";
  return "error";
}

struct Token {
  string type;
  string value;
};

class Lexer {
  string source;
  int pos = 0;

public:
  Lexer(string s) : source(s) {}

  vector<Token> tokenize() {
    vector<Token> tokens;

    while (pos < (int)source.size()) {
      char ch = source[pos];

      if (isspace((unsigned char)ch)) {
        pos++;
        continue;
      }

      if (isalpha((unsigned char)ch) || ch == '_') {
        string word;

        while (pos < (int)source.size() &&
               (isalnum((unsigned char)source[pos]) || source[pos] == '_')) {
          word += source[pos++];
        }

        if (word == "int" || word == "float" || word == "bool" ||
            word == "print" || word == "true" || word == "false") {
          tokens.push_back({word, word});
        } else {
          tokens.push_back({"ID", word});
        }

        continue;
      }

      if (isdigit((unsigned char)ch) ||
          (ch == '.' && pos + 1 < (int)source.size() &&
           isdigit((unsigned char)source[pos + 1]))) {
        string number;

        while (pos < (int)source.size() && isdigit((unsigned char)source[pos]))
          number += source[pos++];

        if (pos < (int)source.size() && source[pos] == '.') {
          number += source[pos++];

          while (pos < (int)source.size() &&
                 isdigit((unsigned char)source[pos]))
            number += source[pos++];
        }

        tokens.push_back({"NUMBER", number});
        continue;
      }

      string symbol(1, ch);

      if (string("+-*/=();").find(ch) != string::npos) {
        tokens.push_back({symbol, symbol});
        pos++;
        continue;
      }

      throw runtime_error("Lexical error: Invalid character " + symbol);
    }

    tokens.push_back({"END", ""});
    return tokens;
  }
};

struct Expr {
  string kind;
  string value;
  unique_ptr<Expr> left, right;

  Expr(string k, string v) : kind(k), value(v) {}
};

enum class StatementType { DECLARATION, ASSIGNMENT, PRINT };

struct Statement {
  StatementType kind;
  string dataType;
  string name;
  unique_ptr<Expr> expression;
};

class Parser {
  vector<Token> tokens;
  int pos = 0;

  Token current() { return tokens[pos]; }

  Token consume() { return tokens[pos++]; }

  bool match(string type) {
    if (current().type == type) {
      consume();
      return true;
    }
    return false;
  }

  Token expect(string type) {
    if (current().type != type)
      throw runtime_error("Syntax error: Expected " + type + " but found " + current().value);

    return consume();
  }

  unique_ptr<Expr> makeBinary(string op, unique_ptr<Expr> left, unique_ptr<Expr> right) {
    auto node = make_unique<Expr>("BINARY", op);
    node->left = move(left);
    node->right = move(right);
    return node;
  }

  unique_ptr<Expr> parseExpression() {
    auto left = parseTerm();

    while (current().type == "+" || current().type == "-") {
      string op = consume().value;
      auto right = parseTerm();
      left = makeBinary(op, move(left), move(right));
    }

    return left;
  }

  unique_ptr<Expr> parseTerm() {
    auto left = parseUnary();

    while (current().type == "*" || current().type == "/") {
      string op = consume().value;
      auto right = parseUnary();
      left = makeBinary(op, move(left), move(right));
    }

    return left;
  }

  unique_ptr<Expr> parseUnary() {
    if (current().type == "+" || current().type == "-") {
      string op = consume().value;
      auto node = make_unique<Expr>("UNARY", op);
      node->left = parseUnary();
      return node;
    }

    return parsePrimary();
  }

  unique_ptr<Expr> parsePrimary() {
    if (current().type == "NUMBER") {
      return make_unique<Expr>("NUMBER", consume().value);
    }

    if (current().type == "true" || current().type == "false") {
      return make_unique<Expr>("BOOL", consume().value);
    }

    if (current().type == "ID") {
      return make_unique<Expr>("IDENTIFIER", consume().value);
    }

    if (match("(")) {
      auto node = parseExpression();
      expect(")");
      return node;
    }

    throw runtime_error("Syntax error: Invalid expression near " + current().value);
  }

  unique_ptr<Statement> parseStatement() {
    auto stmt = make_unique<Statement>();

    if (current().type == "int" || current().type == "float" ||
        current().type == "bool") {

      stmt->kind = StatementType::DECLARATION;
      stmt->dataType = consume().value;
      stmt->name = expect("ID").value;

      if (match("="))
        stmt->expression = parseExpression();

      expect(";");
      return stmt;
    }

    if (match("print")) {
      stmt->kind = StatementType::PRINT;
      stmt->expression = parseExpression();
      expect(";");
      return stmt;
    }

    if (current().type == "ID") {
      stmt->kind = StatementType::ASSIGNMENT;
      stmt->name = consume().value;
      expect("=");
      stmt->expression = parseExpression();
      expect(";");
      return stmt;
    }

    throw runtime_error("Syntax error: Invalid statement");
  }

public:
  Parser(vector<Token> t) : tokens(t) {}

  vector<unique_ptr<Statement>> parse() {
    vector<unique_ptr<Statement>> program;

    while (current().type != "END") {
      program.push_back(parseStatement());
    }

    return program;
  }
};

void printExprAST(const Expr *node, int level = 0) {
  if (!node)
    return;

  cout << string(level * 3, ' ');

  if (node->kind == "BINARY")
    cout << "Operator: " << node->value << '\n';
  else if (node->kind == "UNARY")
    cout << "Unary: " << node->value << '\n';
  else if (node->kind == "IDENTIFIER")
    cout << "Identifier: " << node->value << '\n';
  else if (node->kind == "NUMBER")
    cout << "Number: " << node->value << '\n';
  else
    cout << "Boolean: " << node->value << '\n';

  printExprAST(node->left.get(), level + 1);
  printExprAST(node->right.get(), level + 1);
}

void displayAST(const vector<unique_ptr<Statement>> &program) {
  cout << "\nABSTRACT SYNTAX TREE\n";

  for (const auto &stmt : program) {
    if (stmt->kind == StatementType::DECLARATION)
      cout << "\nDeclaration: " << stmt->dataType << " " << stmt->name << '\n';

    else if (stmt->kind == StatementType::ASSIGNMENT)
      cout << "\nAssignment: " << stmt->name << '\n';

    else
      cout << "\nPrint Statement\n";

    printExprAST(stmt->expression.get(), 1);
  }
}

struct Symbol {
  DataType type;
  bool initialized;
};

class SemanticAnalyzer {
  map<string, Symbol> symbolTable;
  int errors = 0;

  void report(string message) {
    cout << "Semantic Error: " << message << '\n';
    errors++;
  }

  DataType convertType(string type) {
    if (type == "int")
      return DataType::INT;
    if (type == "float")
      return DataType::FLOAT;
    if (type == "bool")
      return DataType::BOOL;
    return DataType::ERROR;
  }

  bool isNumeric(DataType type) {
    return type == DataType::INT || type == DataType::FLOAT;
  }

  bool compatible(DataType target, DataType source) {
    if (target == source)
      return true;

    if (target == DataType::FLOAT && source == DataType::INT)
      return true;

    return false;
  }

  DataType analyzeExpression(const Expr *node) {
    if (!node)
      return DataType::ERROR;

    if (node->kind == "NUMBER") {
      return node->value.find('.') == string::npos ? DataType::INT : DataType::FLOAT;
    }

    if (node->kind == "BOOL")
      return DataType::BOOL;

    if (node->kind == "IDENTIFIER") {
      auto it = symbolTable.find(node->value);

      if (it == symbolTable.end()) {
        report("Variable '" + node->value + "' is not declared.");
        return DataType::ERROR;
      }

      if (!it->second.initialized) {
        report("Variable '" + node->value + "' is used before initialization.");
        return DataType::ERROR;
      }

      return it->second.type;
    }

    if (node->kind == "UNARY") {
      DataType type = analyzeExpression(node->left.get());

      if (type == DataType::ERROR)
        return DataType::ERROR;

      if (!isNumeric(type)) {
        report("Unary operator '" + node->value +
               "' requires a numeric operand.");
        return DataType::ERROR;
      }

      return type;
    }

    if (node->kind == "BINARY") {
      DataType left = analyzeExpression(node->left.get());
      DataType right = analyzeExpression(node->right.get());

      if (left == DataType::ERROR || right == DataType::ERROR)
        return DataType::ERROR;

      if (!isNumeric(left) || !isNumeric(right)) {
        report("Operator '" + node->value +
               "' requires numeric operands, received " + typeName(left) +
               " and " + typeName(right) + ".");
        return DataType::ERROR;
      }

      if (left == DataType::FLOAT || right == DataType::FLOAT)
        return DataType::FLOAT;

      return DataType::INT;
    }

    return DataType::ERROR;
  }

public:
  void analyze(const vector<unique_ptr<Statement>> &program) {
    cout << "\nSEMANTIC ANALYSIS\n\n";

    for (const auto &stmt : program) {
      if (stmt->kind == StatementType::DECLARATION) {
        DataType declared = convertType(stmt->dataType);

        if (symbolTable.count(stmt->name)) {
          report("Variable '" + stmt->name + "' is declared more than once.");

          if (stmt->expression)
            analyzeExpression(stmt->expression.get());

          continue;
        }

        symbolTable[stmt->name] = {declared, false};

        if (stmt->expression) {
          DataType actual = analyzeExpression(stmt->expression.get());

          if (actual != DataType::ERROR) {
            if (compatible(declared, actual)) {
              symbolTable[stmt->name].initialized = true;
              cout << "Declaration OK: " << stmt->name << " : "
                   << typeName(declared) << '\n';
            } else {
              report("Cannot initialize " + typeName(declared) + " variable '" +
                     stmt->name + "' with " + typeName(actual) + ".");
            }
          }
        } else {
          cout << "Declaration OK: " << stmt->name << " : "
               << typeName(declared) << " (uninitialized)\n";
        }
      }

      else if (stmt->kind == StatementType::ASSIGNMENT) {
        DataType actual = analyzeExpression(stmt->expression.get());

        auto it = symbolTable.find(stmt->name);

        if (it == symbolTable.end()) {
          report("Assignment to undeclared variable '" + stmt->name + "'.");
        } else if (actual != DataType::ERROR) {
          if (compatible(it->second.type, actual)) {
            it->second.initialized = true;

            cout << "Assignment OK: " << stmt->name << " : "
                 << typeName(it->second.type) << '\n';
          } else {
            report("Type mismatch in assignment to '" + stmt->name +
                   "'. Expected " + typeName(it->second.type) +
                   " but received " + typeName(actual) + ".");
          }
        }
      }

      else if (stmt->kind == StatementType::PRINT) {
        DataType actual = analyzeExpression(stmt->expression.get());

        if (actual != DataType::ERROR) {
          cout << "Print expression type: " << typeName(actual) << '\n';
        }
      }
    }

    cout << "\nSYMBOL TABLE\n\n";

    cout << left << setw(15) << "VARIABLE" << setw(15) << "DATA TYPE"
         << "STATUS\n";

    cout << string(45, '-') << '\n';

    for (const auto &entry : symbolTable) {
      cout << left << setw(15) << entry.first << setw(15)
           << typeName(entry.second.type)
           << (entry.second.initialized ? "Initialized" : "Uninitialized")
           << '\n';
    }

    cout << "\n";

    if (errors == 0)
      cout << "Semantic Analysis Successful.\n";
    else
      cout << "Semantic Analysis Failed. Total Errors: " << errors << '\n';
  }
};

int main() {
  string source, line;

  cout << "Enter source code (type END to finish):\n";

  while (getline(cin, line)) {
    if (line == "END")
      break;

    source += line + '\n';
  }

  try {
    Lexer lexer(source);

    vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);

    auto ast = parser.parse();

    cout << "\nParsing Successful.\n";

    displayAST(ast);

    SemanticAnalyzer analyzer;

    analyzer.analyze(ast);
  } catch (const exception &e) {
    cout << "\n" << e.what() << '\n';
    cout << "Parsing Failed.\n";
    return 1;
  }
  return 0;
}
```
### User Input
``` bash
> g++ code.cpp && ./a.out
> int a = 10;
> float b = 2.5;
> float c = a + b * 2;
> bool flag = true;
> a = 20;
> print c;
> print flag;
> END
```

### 12. Write a program that accepts an arithmetic expression or a simple assignment statement (e.g., a = b + c * d).
- Performs lexical analysis and parsing (you may use a recursive descent parser or operator-precedence parsing).
- Generates Three-Address Code for the given expression.
- Optionally converts the Three-Address Code into Quadruple and Triple representations.
- Handles basic operators: +, -, *, /, and assignment =.
- Use temporary variables (e.g., t1, t2, …) for intermediate results.

`code.cpp`
``` cpp
#include <bits/stdc++.h>
using namespace std;

enum TokenType {
  IDENTIFIER,
  NUMBER,
  PLUS,
  MINUS,
  MUL,
  DIV,
  ASSIGN,
  LPAREN,
  RPAREN,
  END
};
struct Token {
  TokenType type;
  string value;
};
struct TAC {
  string result;
  string arg1;
  string op;
  string arg2;
};
vector<Token> tokens;
vector<TAC> code;
int pos = 0;
int tempCount = 0;
string newTemp() { return "t" + to_string(++tempCount); }

// ---------------- LEXICAL ANALYSIS ----------------
vector<Token> tokenize(const string &input) {
  vector<Token> result;
  for (int i = 0; i < input.length();) {
    if (isspace(input[i])) {
      i++;
      continue;
    }
    if (isalpha(input[i]) || input[i] == '_') {
      string word;
      while (i < input.length() && (isalnum(input[i]) || input[i] == '_')) {
        word += input[i++];
      }
      result.push_back({IDENTIFIER, word});
    } else if (isdigit(input[i])) {
      string number;
      while (i < input.length() && isdigit(input[i])) {
        number += input[i++];
      }
      result.push_back({NUMBER, number});
    } else {
      switch (input[i]) {
      case '+':
        result.push_back({PLUS, "+"});
        break;
      case '-':
        result.push_back({MINUS, "-"});
        break;
      case '*':
        result.push_back({MUL, "*"});
        break;
      case '/':
        result.push_back({DIV, "/"});
        break;
      case '=':
        result.push_back({ASSIGN, "="});
        break;
      case '(':
        result.push_back({LPAREN, "("});
        break;
      case ')':
        result.push_back({RPAREN, ")"});
        break;
      default:
        cout << "Invalid character: " << input[i] << endl;
        exit(1);
      }
      i++;
    }
  }
  result.push_back({END, ""});
  return result;
}
// ---------------- PARSER ----------------
// Grammar:
// statement -> id = expression | expression
// expression -> term { (+|-) term }
// term -> factor { (*|/) factor }
// factor -> id | number | ( expression )
string factor();
string expression();
string term();

string factor() {
  Token token = tokens[pos];
  if (token.type == IDENTIFIER || token.type == NUMBER) {
    pos++;
    return token.value;
  }
  if (token.type == LPAREN) {
    pos++;
    string result = expression();
    if (tokens[pos].type != RPAREN) {
      cout << "Error: Missing ')'\n";
      exit(1);
    }
    pos++;
    return result;
  }
  cout << "Syntax error in factor\n";
  exit(1);
}

string term() {
  string left = factor();
  while (tokens[pos].type == MUL || tokens[pos].type == DIV) {
    string op = tokens[pos].value;
    pos++;
    string right = factor();
    string temp = newTemp();
    code.push_back({temp, left, op, right});
    left = temp;
  }
  return left;
}
string expression() {
  string left = term();
  while (tokens[pos].type == PLUS || tokens[pos].type == MINUS) {
    string op = tokens[pos].value;
    pos++;
    string right = term();
    string temp = newTemp();
    code.push_back({temp, left, op, right});
    left = temp;
  }
  return left;
}
void statement() {
  if (tokens[pos].type == IDENTIFIER && tokens[pos + 1].type == ASSIGN) {
    string variable = tokens[pos].value;
    pos += 2;
    string result = expression();
    code.push_back({variable, result, "=", ""});
  } else {
    string result = expression();
    cout << "\nFinal result stored in: " << result << endl;
  }
  if (tokens[pos].type != END) {
    cout << "Syntax error: Unexpected token " << tokens[pos].value << endl;
    exit(1);
  }
}
void printTokens() {
  cout << "\nLEXICAL ANALYSIS\n";
  for (const Token &t : tokens) {
    if (t.type == END)
      break;
    cout << t.value << "\t";
    switch (t.type) {
    case IDENTIFIER:
      cout << "IDENTIFIER";
      break;
    case NUMBER:
      cout << "NUMBER";
      break;
    case PLUS:
    case MINUS:
    case MUL:
    case DIV:
      cout << "OPERATOR";
      break;
    case ASSIGN:
      cout << "ASSIGNMENT";
      break;
    case LPAREN:
      cout << "LEFT PARENTHESIS";
      break;
    case RPAREN:
      cout << "RIGHT PARENTHESIS";
      break;
    default:
      break;
    }
    cout << endl;
  }
}
void printTAC() {
  cout << "\nTHREE ADDRESS CODE\n";
  for (const TAC &x : code) {
    if (x.op == "=") {
      cout << x.result << " = " << x.arg1 << endl;
    } else {
      cout << x.result << " = " << x.arg1 << " " << x.op << " " << x.arg2
           << endl;
    }
  }
}
void printQuadruples() {
  cout << "\nQUADRUPLES\n";
  cout << "No.\tOperator\tArg1\tArg2\tResult\n";
  int i = 0;
  for (const TAC &x : code) {
    cout << i++ << "\t" << x.op << "\t\t" << x.arg1 << "\t"
         << (x.arg2.empty() ? "-" : x.arg2) << "\t" << x.result << endl;
  }
}
void printTriples() {
  cout << "\nTRIPLES\n";
  cout << "No.\tOperator\tArg1\tArg2\n";
  for (int i = 0; i < code.size(); i++) {
    string arg1 = code[i].arg1;
    string arg2 = code[i].arg2;
    if (arg1.length() > 1 && arg1[0] == 't') {
      bool temp = true;
      for (int j = 1; j < arg1.length(); j++) {
        if (!isdigit(arg1[j]))
          temp = false;
      }
      if (temp) {
        int n = stoi(arg1.substr(1));
        arg1 = "(" + to_string(n - 1) + ")";
      }
    }
    if (arg2.length() > 1 && arg2[0] == 't') {
      bool temp = true;
      for (int j = 1; j < arg2.length(); j++) {
        if (!isdigit(arg2[j]))
          temp = false;
      }
      if (temp) {
        int n = stoi(arg2.substr(1));
        arg2 = "(" + to_string(n - 1) + ")";
      }
    }
    if (code[i].op == "=") {
      cout << i << "\t" << "=" << "\t\t" << arg1 << "\t" << code[i].result
           << endl;
    } else {
      cout << i << "\t" << code[i].op << "\t\t" << arg1 << "\t" << arg2 << endl;
    }
  }
}
int main() {
  string input;
  cout << "Enter an arithmetic expression or assignment:\n";
  getline(cin, input);
  tokens = tokenize(input);
  printTokens();
  statement();
  cout << "SUCCESSFULLY PARSED.\n";
  printTAC();
  printQuadruples();
  printTriples();
  return 0;
}
```
### User Input
``` bash
> g++ code.cpp && ./a.out
> Enter an arithmetic expression or assignment:
> a = b + c * d
```