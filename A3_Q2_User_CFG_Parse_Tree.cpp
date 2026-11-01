#include <bits/stdc++.h>
using namespace std;

using Grammar = map<string, vector<vector<string>>>;
string EPS = "eps", startSymbol;
Grammar grammar;

string trim(string text) {
    size_t begin = text.find_first_not_of(" \t\r\n");
    if (begin == string::npos) return "";
    return text.substr(begin, text.find_last_not_of(" \t\r\n") - begin + 1);
}

bool readGrammar() {
    int count;
    cout << "Number of production lines = ";
    if (!(cin >> count) || count <= 0) {
        cout << "Number of production lines must be positive\n";
        return false;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Separate every RHS symbol with spaces, for example E -> T E'\n";
    cout << "Nonterminal names start with uppercase letters. Use | for alternatives and eps for epsilon.\n";
    for (int i = 0; i < count; ++i) {
        string line;
        cout << "Production " << i + 1 << " = ";
        if (!getline(cin, line)) return false;
        size_t arrow = line.find("->");
        if (arrow == string::npos || line.find("->", arrow + 2) != string::npos) {
            cout << "Invalid production. Expected A -> symbol symbol | eps\n";
            return false;
        }
        string lhs = trim(line.substr(0, arrow));
        if (lhs.empty() || !isupper((unsigned char)lhs[0]) ||
            lhs.find_first_of(" \t|$") != string::npos) {
            cout << "Invalid nonterminal name\n";
            return false;
        }
        size_t begin = arrow + 2;
        while (true) {
            size_t end = line.find('|', begin);
            istringstream words(line.substr(begin, end == string::npos ? end : end - begin));
            vector<string> rhs;
            string symbol;
            while (words >> symbol) rhs.push_back(symbol);
            if (rhs.empty() || (rhs.size() > 1 && find(rhs.begin(), rhs.end(), EPS) != rhs.end()) ||
                find(rhs.begin(), rhs.end(), "$") != rhs.end()) {
                cout << "Invalid alternative. Use eps alone for epsilon; $ is reserved\n";
                return false;
            }
            auto& productions = grammar[lhs];
            if (find(productions.begin(), productions.end(), rhs) == productions.end())
                productions.push_back(rhs);
            if (end == string::npos) break;
            begin = end + 1;
        }
    }
    for (auto& [lhs, productions] : grammar) {
        for (auto rhs : productions)
            for (auto symbol : rhs)
                if (isupper((unsigned char)symbol[0]) && !grammar.count(symbol)) {
                    cout << "Missing productions for nonterminal = " << symbol << '\n';
                    return false;
                }
    }
    cout << "Start symbol = ";
    if (!(cin >> startSymbol) || !grammar.count(startSymbol)) {
        cout << "Start symbol must be a defined nonterminal\n";
        return false;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return true;
}

struct Node {
    string value;
    vector<shared_ptr<Node>> children;
    explicit Node(string text) : value(move(text)) {}
};
struct Production { string lhs; vector<string> rhs; };
struct Item { int rule, dot, origin; };
bool operator<(Item a, Item b) {
    return tie(a.rule, a.dot, a.origin) < tie(b.rule, b.dot, b.origin);
}
using Children = vector<shared_ptr<Node>>;
using Chart = map<Item, Children>;

shared_ptr<Node> parse(vector<string>& input) {
    string augmented = startSymbol + "'";
    while (grammar.count(augmented)) augmented += "'";
    vector<Production> rules = {{augmented, {startSymbol}}};
    for (auto entry : grammar)
        for (auto rhs : entry.second) {
            if (rhs == vector<string>{EPS}) rhs.clear();
            rules.push_back({entry.first, rhs});
        }
    vector<Chart> chart(input.size() + 1);
    chart[0][{0, 0, 0}] = {};
    for (int pos = 0; pos <= (int)input.size(); ++pos) {
        bool changed;
        do {
            changed = false;
            auto items = chart[pos];
            for (auto entry : items) {
                auto item = entry.first;
                auto rule = rules[item.rule];
                if (item.dot < (int)rule.rhs.size()) {
                    string symbol = rule.rhs[item.dot];
                    if (grammar.count(symbol)) {
                        for (int k = 1; k < (int)rules.size(); ++k)
                            if (rules[k].lhs == symbol)
                                changed |= chart[pos].insert({{k, 0, pos}, {}}).second;
                    } else if (pos < (int)input.size() && input[pos] == symbol) {
                        auto children = entry.second;
                        children.push_back(make_shared<Node>(symbol));
                        chart[pos + 1].insert({{item.rule, item.dot + 1, item.origin}, children});
                    }
                } else {
                    auto node = make_shared<Node>(rule.lhs);
                    node->children = entry.second;
                    if (rule.rhs.empty()) node->children.push_back(make_shared<Node>(EPS));
                    auto waiting = chart[item.origin];
                    for (auto previous : waiting) {
                        auto parent = previous.first;
                        auto rhs = rules[parent.rule].rhs;
                        if (parent.dot == (int)rhs.size() || rhs[parent.dot] != rule.lhs) continue;
                        auto children = previous.second;
                        children.push_back(node);
                        changed |= chart[pos].insert({{parent.rule, parent.dot + 1, parent.origin}, children}).second;
                    }
                }
            }
        } while (changed);
    }
    auto it = chart.back().find({0, 1, 0});
    if (it == chart.back().end()) return nullptr;
    return it->second[0];
}

void printTree(shared_ptr<Node> node, string prefix = "", bool last = true, bool root = true) {
    cout << prefix;
    if (!root) cout << (last ? "`-- " : "|-- ");
    cout << node->value << '\n';
    string next = prefix + (root ? "" : last ? "    " : "|   ");
    for (size_t i = 0; i < node->children.size(); ++i)
        printTree(node->children[i], next, i + 1 == node->children.size(), false);
}

int main() {
    if (!readGrammar()) return 1;
    cout << "Enter space-separated terminals (blank for empty input; omit $) = ";
    string line, symbol;
    getline(cin, line);
    istringstream words(line);
    vector<string> input;
    while (words >> symbol) input.push_back(symbol);
    auto tree = parse(input);
    if (!tree) {
        cout << "Result = REJECTED\n";
        return 0;
    }
    cout << "Result = ACCEPTED\nParse tree =\n";
    printTree(tree);
    return 0;
}
