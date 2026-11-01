#include <bits/stdc++.h>
using namespace std;

void fail(string msg) { cerr << "Error = " << msg << '\n'; exit(1); }

struct Quad { string op, a, b, result; };

class Generator {
    vector<string> tokens;
    vector<Quad> code;
    size_t pos = 0;
    int temp = 0;
    bool take(string s) { if (pos < tokens.size() && tokens[pos] == s) { ++pos; return true; } return false; }
    string emit(string op, string a, string b) {
        string result = "t" + to_string(++temp);
        code.push_back({op, a, b, result});
        return result;
    }
    string factor() {
        if (take("(")) {
            string value = expression();
            if (!take(")")) fail("Missing )");
            return value;
        }
        if (pos == tokens.size() || (!isalnum((unsigned char)tokens[pos][0]) && tokens[pos][0] != '_'))
            fail("Missing operand");
        return tokens[pos++];
    }
    string term() {
        string left = factor();
        while (pos < tokens.size() && (tokens[pos] == "*" || tokens[pos] == "/")) {
            string op = tokens[pos++]; left = emit(op, left, factor());
        }
        return left;
    }
    string expression() {
        string left = term();
        while (pos < tokens.size() && (tokens[pos] == "+" || tokens[pos] == "-")) {
            string op = tokens[pos++]; left = emit(op, left, term());
        }
        return left;
    }
public:
    vector<string> lex(string s) {
        vector<string> result;
        for (size_t i = 0; i < s.size();) {
            if (isspace((unsigned char)s[i])) { ++i; continue; }
            if (isalnum((unsigned char)s[i]) || s[i] == '_') {
                string word;
                while (i < s.size() && (isalnum((unsigned char)s[i]) || s[i] == '_')) word += s[i++];
                result.push_back(word);
            } else result.push_back(s.substr(i++, 1));
        }
        return result;
    }
    vector<Quad> generate(string input) {
        tokens = lex(input); pos = 0; temp = 0; code.clear();
        if (tokens.empty()) fail("Empty input");
        bool assignment = tokens.size() > 1 && tokens[1] == "=";
        string target;
        if (assignment) { target = tokens[0]; pos = 2; }
        string result = expression();
        if (pos < tokens.size() && tokens[pos] == ";") ++pos;
        if (pos != tokens.size()) fail("Unexpected token");
        if (assignment) code.push_back({"=", result, "-", target});
        return code;
    }
};

void display(vector<Quad> code) {
    cout << "Three-address code =\n";
    for (auto& q : code)
        cout << q.result << " = " << q.a << (q.op == "=" ? "" : " " + q.op + " " + q.b) << '\n';
    cout << "Quadruples =\n";
    cout << "INDEX   OP      ARG1    ARG2    RESULT\n";
    for (size_t i = 0; i < code.size(); ++i) {
        auto& q = code[i];
        cout << left << setw(8) << i << setw(8) << q.op << setw(8) << q.a << setw(8) << q.b << q.result << '\n';
    }
    cout << "Triples =\n";
    cout << "INDEX   OP      ARG1    ARG2\n";
    unordered_map<string,string> temporaries;
    for (size_t i = 0; i < code.size(); ++i) {
        auto q = code[i];
        if (temporaries.count(q.a)) q.a = temporaries[q.a];
        if (temporaries.count(q.b)) q.b = temporaries[q.b];
        cout << left << setw(8) << i << setw(8) << q.op << setw(8) << q.a << q.b << '\n';
        if (!code[i].result.empty() && code[i].result[0] == 't') temporaries[code[i].result] = "(" + to_string(i) + ")";
    }
}

int main() {
    string input;
    cout << "Expression or assignment = ";
    getline(cin, input);
    display(Generator().generate(input));
}
