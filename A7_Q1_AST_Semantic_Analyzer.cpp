#include <bits/stdc++.h>
using namespace std;

void fail(string msg) { cerr << "Error = " << msg << '\n'; exit(1); }

enum class Type { INT, BOOL, ERROR };
string label(Type t) { return t == Type::INT ? "int" : t == Type::BOOL ? "bool" : "error"; }
struct Node {
    string value;
    unique_ptr<Node> left, right;
    explicit Node(string s) : value(move(s)) {}
};

class SemanticAnalyzer {
    unordered_map<string, Type> symbols;
public:
    void declare(string name, Type type) {
        if (symbols.count(name)) fail("Redeclaration = " + name);
        symbols[name] = type;
    }
    Type check(Node* node) {
        if (!node) return Type::ERROR;
        string v = node->value;
        if (v == "true" || v == "false") return Type::BOOL;
        if (isdigit((unsigned char)v[0])) return Type::INT;
        if (v == "+" || v == "-" || v == "*" || v == "/") {
            Type a = check(node->left.get()), b = check(node->right.get());
            if (a != Type::INT || b != Type::INT) fail("Arithmetic operands must be int");
            return Type::INT;
        }
        if (v == "==" || v == "!=") {
            Type a = check(node->left.get()), b = check(node->right.get());
            if (a != b) fail("Comparison type mismatch");
            return Type::BOOL;
        }
        auto it = symbols.find(v);
        if (it == symbols.end()) fail("Undeclared identifier = " + v);
        return it->second;
    }
    void assign(string name, Node* root) {
        auto it = symbols.find(name);
        if (it == symbols.end()) fail("Undeclared identifier = " + name);
        Type actual = check(root);
        if (actual != it->second) fail("Assignment mismatch = " + name);
        cout << "Valid assignment = " << name << " (" << label(actual) << ")\n";
    }
};

vector<string> tokenize(string s) {
    vector<string> result;
    for (size_t i = 0; i < s.size();) {
        if (isspace((unsigned char)s[i])) { ++i; continue; }
        if (isalnum((unsigned char)s[i]) || s[i] == '_') {
            string word;
            while (i < s.size() && (isalnum((unsigned char)s[i]) || s[i] == '_')) word += s[i++];
            result.push_back(word);
        } else if (i + 1 < s.size() && (s.substr(i, 2) == "==" || s.substr(i, 2) == "!=")) {
            result.push_back(s.substr(i, 2)); i += 2;
        } else result.push_back(s.substr(i++, 1));
    }
    return result;
}

class Parser {
    vector<string> tokens;
    size_t pos = 0;
    bool match(string s) { if (pos < tokens.size() && tokens[pos] == s) { ++pos; return true; } return false; }
    unique_ptr<Node> atom() {
        if (match("(")) {
            auto result = comparison();
            if (!match(")")) fail("Missing )");
            return result;
        }
        if (pos == tokens.size()) fail("Missing operand");
        string s = tokens[pos++];
        if (!isalnum((unsigned char)s[0]) && s[0] != '_') fail("Unexpected token = " + s);
        return make_unique<Node>(s);
    }
    unique_ptr<Node> binary(unique_ptr<Node> left, string op, unique_ptr<Node> right) {
        auto root = make_unique<Node>(op);
        root->left = move(left); root->right = move(right);
        return root;
    }
    unique_ptr<Node> term() {
        auto left = atom();
        while (pos < tokens.size() && (tokens[pos] == "*" || tokens[pos] == "/")) {
            string op = tokens[pos++]; left = binary(move(left), op, atom());
        }
        return left;
    }
    unique_ptr<Node> addition() {
        auto left = term();
        while (pos < tokens.size() && (tokens[pos] == "+" || tokens[pos] == "-")) {
            string op = tokens[pos++]; left = binary(move(left), op, term());
        }
        return left;
    }
    unique_ptr<Node> comparison() {
        auto left = addition();
        if (pos < tokens.size() && (tokens[pos] == "==" || tokens[pos] == "!=")) {
            string op = tokens[pos++]; left = binary(move(left), op, addition());
        }
        return left;
    }
public:
    explicit Parser(string input) : tokens(tokenize(input)) {}
    unique_ptr<Node> parse() {
        auto result = comparison();
        if (pos != tokens.size()) fail("Unexpected trailing token");
        return result;
    }
};

int main() {
    SemanticAnalyzer checker;
    cout << "Enter declarations or assignments, one per line (type END to finish) =\n";
    string line;
    while (getline(cin, line) && line != "END") {
        if (line.empty()) continue;
        if (line.back() == ';') line.pop_back();
        istringstream words(line);
        string first, name;
        words >> first;
        if (first == "int" || first == "bool") {
            words >> name;
            if (name.empty()) fail("Missing declaration name");
            checker.declare(name, first == "int" ? Type::INT : Type::BOOL);
            cout << "Declared = " << name << '\n';
        } else {
            size_t at = line.find('=');
            if (at == string::npos || (at + 1 < line.size() && line[at + 1] == '=')) fail("Expected assignment");
            string lhs = line.substr(0, at);
            lhs.erase(remove_if(lhs.begin(), lhs.end(), [](unsigned char c) { return isspace(c); }), lhs.end());
            auto ast = Parser(line.substr(at + 1)).parse();
            checker.assign(lhs, ast.get());
        }
    }
}
