#include <bits/stdc++.h>
using namespace std;

void fail(string msg) { cerr << "Error = " << msg << '\n'; exit(1); }

struct Node {
    string value;
    unique_ptr<Node> left, right;
    explicit Node(string v) : value(move(v)) {}
};

class Parser {
    string s;
    size_t pos = 0;
    void skip() { while (pos < s.size() && isspace((unsigned char)s[pos])) ++pos; }
    bool accept(char c) { skip(); if (pos < s.size() && s[pos] == c) { ++pos; return true; } return false; }
    unique_ptr<Node> factor() {
        skip();
        if (accept('(')) {
            auto node = expression();
            if (!accept(')')) fail("Missing )");
            return node;
        }
        string value;
        while (pos < s.size() && (isalnum((unsigned char)s[pos]) || s[pos] == '_')) value += s[pos++];
        if (value.empty()) fail("Expected identifier or number");
        return make_unique<Node>(value);
    }
    unique_ptr<Node> term() {
        auto left = factor();
        while (true) {
            skip();
            if (pos == s.size() || (s[pos] != '*' && s[pos] != '/')) return left;
            string op(1, s[pos++]);
            auto node = make_unique<Node>(op);
            node->left = move(left);
            node->right = factor();
            left = move(node);
        }
    }
    unique_ptr<Node> expression() {
        auto left = term();
        while (true) {
            skip();
            if (pos == s.size() || (s[pos] != '+' && s[pos] != '-')) return left;
            string op(1, s[pos++]);
            auto node = make_unique<Node>(op);
            node->left = move(left);
            node->right = term();
            left = move(node);
        }
    }
public:
    explicit Parser(string input) : s(move(input)) {}
    unique_ptr<Node> parse() {
        auto root = expression();
        skip();
        if (pos != s.size()) fail("Unexpected input");
        return root;
    }
};

void printAST(Node* node, string prefix = "", bool last = true, bool root = true) {
    if (!node) return;
    cout << prefix;
    if (!root) cout << (last ? "`-- " : "|-- ");
    cout << node->value << '\n';
    string next = prefix + (root ? "" : last ? "    " : "|   ");
    printAST(node->left.get(), next, !node->right, false);
    printAST(node->right.get(), next, true, false);
}

int main() {
    string s;
    cout << "Expression = ";
    getline(cin, s);
    auto tree = Parser(s).parse();
    cout << "AST =\n";
    printAST(tree.get());
}
