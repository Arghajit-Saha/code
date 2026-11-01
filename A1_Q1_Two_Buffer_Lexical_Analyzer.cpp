#include <bits/stdc++.h>
using namespace std;

class TwoBuffer {
    enum { N = 16 };
    string source;
    size_t position = 0;
    char data[2][N + 1]{};
    int length[2]{}, active = 0, forwardPtr = 0, lexemeBegin = 0;
    bool ended = false;
    void refill(int b) {
        length[b] = 0;
        while (length[b] < N && position < source.size())
            data[b][length[b]++] = source[position++];
        data[b][length[b]] = '\0';
    }
public:
    explicit TwoBuffer(string input) : source(input) {
        refill(0);
    }

    char peek() {
        while (forwardPtr == length[active] && !ended) {
            if (length[active] < N) { ended = true; break; }
            active ^= 1;
            refill(active);
            forwardPtr = 0;
            if (!length[active]) ended = true;
        }
        return ended ? '\0' : data[active][forwardPtr];
    }
    char get() {
        char c = peek();
        if (c) ++forwardPtr;
        return c;
    }
    void beginLexeme() { lexemeBegin = forwardPtr; }
};

void tokenize(TwoBuffer& input) {
    unordered_set<string> keywords = {"int", "float", "double", "char", "void", "for", "while", "if", "else", "return", "using", "namespace"};
    while (char c = input.peek()) {
        if (isspace((unsigned char)c)) { input.get(); continue; }
        input.beginLexeme();
        string s;
        if (isalpha((unsigned char)c) || c == '_') {
            do { s += input.get(); c = input.peek(); }
            while (isalnum((unsigned char)c) || c == '_');
            cout << (keywords.count(s) ? "KEYWORD" : "IDENTIFIER") << " = " << s << '\n';
        } else if (isdigit((unsigned char)c)) {
            do { s += input.get(); c = input.peek(); }
            while (isdigit((unsigned char)c));
            cout << "NUMBER = " << s << '\n';
        } else if (c == '"' || c == '\'') {
            char quote = input.get(); s += quote;
            while (input.peek() && input.peek() != quote) {
                if (input.peek() == '\\') s += input.get();
                if (input.peek()) s += input.get();
            }
            if (input.peek()) s += input.get();
            cout << "STRING/CHAR = " << s << '\n';
        } else {
            s += input.get();
            string two = s + input.peek();
            if (unordered_set<string>{"++", "--", "<=", ">=", "==", "!=", "&&", "||", "<<", ">>", "+=", "-=", "*=", "/="}.count(two)) s += input.get();
            cout << "SYMBOL = " << s << '\n';
        }
    }
}

int main() {
    cout << "Enter source code (type END on a separate line to finish) =\n";
    string source, line;
    while (getline(cin, line) && line != "END") source += line + '\n';
    TwoBuffer input(source);
    tokenize(input);
}
