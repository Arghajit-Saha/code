#include <bits/stdc++.h>
using namespace std;

struct Token { int line; string type, lexeme; };

vector<Token> scan(string s) {
    unordered_set<string> keywords = {"int", "float", "double", "char", "void", "if", "else", "while", "for", "return", "break", "continue", "using", "namespace"};
    unordered_set<string> pairs = {"++", "--", "==", "!=", "<=", ">=", "&&", "||", "<<", ">>", "+=", "-=", "*=", "/="};
    vector<Token> tokens;
    int line = 1;
    for (size_t i = 0; i < s.size();) {
        char c = s[i];
        if (isspace((unsigned char)c)) { if (c == '\n') ++line; ++i; continue; }
        if (i + 1 < s.size() && s.substr(i, 2) == "//") {
            while (i < s.size() && s[i] != '\n') ++i;
            continue;
        }
        string value, type;
        if (isalpha((unsigned char)c) || c == '_') {
            while (i < s.size() && (isalnum((unsigned char)s[i]) || s[i] == '_')) value += s[i++];
            type = keywords.count(value) ? "KEYWORD" : "IDENTIFIER";
        } else if (isdigit((unsigned char)c)) {
            while (i < s.size() && isdigit((unsigned char)s[i])) value += s[i++];
            if (i < s.size() && s[i] == '.') {
                value += s[i++];
                while (i < s.size() && isdigit((unsigned char)s[i])) value += s[i++];
            }
            type = "NUMBER";
        } else if (c == '"' || c == '\'') {
            char quote = c;
            value += s[i++];
            while (i < s.size() && s[i] != quote) {
                if (s[i] == '\\' && i + 1 < s.size()) value += s[i++];
                if (i < s.size()) value += s[i++];
            }
            if (i < s.size()) value += s[i++];
            type = "LITERAL";
        } else {
            value = s.substr(i, 1);
            if (i + 1 < s.size() && pairs.count(s.substr(i, 2))) value = s.substr(i, 2);
            i += value.size();
            type = string("+-*/%=<>!&|^").find(c) != string::npos ? "OPERATOR" :
                   string(";:,(){}[]#.").find(c) != string::npos ? "PUNCTUATION" : "UNKNOWN";
        }
        tokens.push_back({line, type, value});
    }
    return tokens;
}

int main() {
    cout << "Enter source code (type END on a separate line to finish) =\n";
    string source, line;
    while (getline(cin, line) && line != "END") source += line + '\n';
    cout << left << setw(8) << "LINE" << setw(18) << "TOKEN TYPE" << "LEXEME\n";
    cout << string(48, '-') << '\n';
    for (auto& t : scan(source)) cout << left << setw(8) << t.line << setw(18) << t.type << t.lexeme << '\n';
}
