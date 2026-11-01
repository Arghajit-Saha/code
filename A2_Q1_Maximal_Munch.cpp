#include <bits/stdc++.h>
using namespace std;

void tokenize(string s) {
    unordered_set<string> keywords = {"int", "if", "else", "return", "while", "for", "break", "continue"};
    vector<pair<string,string>> ops = {{"++", "INC_OP"}, {"--", "DEC_OP"}, {"+=", "ARITH_OP"}, {"-=", "ARITH_OP"}, {"==", "REL_OP"}, {"!=", "REL_OP"}, {"<=", "REL_OP"}, {">=", "REL_OP"}, {"&&", "LOGIC_OP"}, {"||", "LOGIC_OP"}, {"+", "OPERATOR"}, {"-", "OPERATOR"}, {"*", "OPERATOR"}, {"/", "OPERATOR"}, {"=", "OPERATOR"}, {"<", "OPERATOR"}, {">", "OPERATOR"}};
    for (size_t i = 0; i < s.size();) {
        if (isspace((unsigned char)s[i])) { ++i; continue; }
        string value, type;
        if (isalpha((unsigned char)s[i]) || s[i] == '_') {
            while (i < s.size() && (isalnum((unsigned char)s[i]) || s[i] == '_')) value += s[i++];
            type = keywords.count(value) ? "KEYWORD" : "IDENTIFIER";
        } else if (isdigit((unsigned char)s[i])) {
            while (i < s.size() && isdigit((unsigned char)s[i])) value += s[i++];
            type = "NUMBER";
        } else {
            for (auto& [op, category] : ops)
                if (s.compare(i, op.size(), op) == 0 && op.size() > value.size()) { value = op; type = category; }
            if (value.empty()) { value = s.substr(i, 1); type = string(";:,(){}[]").find(s[i]) != string::npos ? "PUNCTUATION" : "UNKNOWN"; }
            i += value.size();
        }
        cout << '<' << type << ", " << value << ">\n";
    }
}

int main() {
    cout << "Enter source code (type END on a separate line to finish) =\n";
    string s, line;
    while (getline(cin, line) && line != "END") s += line + '\n';
    tokenize(s);
}
