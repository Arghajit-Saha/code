#include <bits/stdc++.h>
using namespace std;

using Grammar = map<string, vector<vector<string>>>;
string EPS = "eps", startSymbol;
Grammar grammar;
map<string, set<string>> first, follow;

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
        first[lhs];
        follow[lhs];
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

set<string> firstOf(vector<string> rhs) {
    set<string> result;
    bool nullable = true;
    for (string symbol : rhs) {
        if (symbol == EPS) break;
        if (!grammar.count(symbol)) { result.insert(symbol); nullable = false; break; }
        for (string t : first[symbol]) if (t != EPS) result.insert(t);
        if (!first[symbol].count(EPS)) { nullable = false; break; }
    }
    if (nullable) result.insert(EPS);
    return result;
}

void computeFirst() {
    bool changed;
    do {
        changed = false;
        for (auto& [lhs, productions] : grammar)
            for (auto& rhs : productions)
                for (auto& t : firstOf(rhs)) changed |= first[lhs].insert(t).second;
    } while (changed);
}

void computeFollow() {
    follow[startSymbol].insert("$");
    bool changed;
    do {
        changed = false;
        for (auto& [lhs, productions] : grammar)
            for (auto& rhs : productions)
                for (size_t i = 0; i < rhs.size(); ++i) {
                    if (!grammar.count(rhs[i])) continue;
                    vector<string> tail(rhs.begin() + i + 1, rhs.end());
                    auto f = firstOf(tail);
                    for (auto& t : f) if (t != EPS) changed |= follow[rhs[i]].insert(t).second;
                    if (f.count(EPS)) for (auto& t : follow[lhs]) changed |= follow[rhs[i]].insert(t).second;
                }
    } while (changed);
}

struct Production { string lhs; vector<string> rhs; };
struct Item { int rule, dot; };
bool operator<(Item a, Item b) { return tie(a.rule, a.dot) < tie(b.rule, b.dot); }
bool operator==(Item a, Item b) { return a.rule == b.rule && a.dot == b.dot; }
using State = set<Item>;
using Actions = map<pair<int, string>, string>;
vector<Production> rules;
set<string> terminals, symbols;

State closure(State items) {
    bool changed;
    do {
        changed = false;
        auto old = items;
        for (auto item : old) {
            auto rule = rules[item.rule];
            if (item.dot == (int)rule.rhs.size()) continue;
            string symbol = rule.rhs[item.dot];
            if (!grammar.count(symbol)) continue;
            for (int k = 1; k < (int)rules.size(); ++k)
                if (rules[k].lhs == symbol) changed |= items.insert({k, 0}).second;
        }
    } while (changed);
    return items;
}

State nextState(State items, string symbol) {
    State moved;
    for (auto item : items) {
        auto rule = rules[item.rule];
        if (item.dot < (int)rule.rhs.size() && rule.rhs[item.dot] == symbol)
            moved.insert({item.rule, item.dot + 1});
    }
    return closure(moved);
}

bool addAction(Actions& actions, int state, string symbol, string value, bool report) {
    auto key = make_pair(state, symbol);
    if (actions.count(key) && actions[key] != value) {
        if (report) cout << "Conflict at ACTION[" << state << ", " << symbol
                         << "] = " << actions[key] << " versus " << value << '\n';
        return false;
    }
    actions[key] = value;
    return true;
}

int main() {
    if (!readGrammar()) return 1;
    computeFirst();
    computeFollow();
    string augmented = startSymbol + "'";
    while (grammar.count(augmented)) augmented += "'";
    rules.push_back({augmented, {startSymbol}});
    for (auto entry : grammar) {
        symbols.insert(entry.first);
        for (auto rhs : entry.second) {
            if (rhs == vector<string>{EPS}) rhs.clear();
            rules.push_back({entry.first, rhs});
            for (auto symbol : rhs) {
                symbols.insert(symbol);
                if (!grammar.count(symbol)) terminals.insert(symbol);
            }
        }
    }
    terminals.insert("$");
    vector<State> states = {closure({{0, 0}})};
    map<pair<int, string>, int> transitions, go;
    for (size_t i = 0; i < states.size(); ++i) {
        for (auto symbol : symbols) {
            auto next = nextState(states[i], symbol);
            if (next.empty()) continue;
            auto it = find(states.begin(), states.end(), next);
            int dest = it == states.end() ? (int)states.size() : (int)distance(states.begin(), it);
            if (it == states.end()) states.push_back(next);
            transitions[{(int)i, symbol}] = dest;
        }
    }
    cout << "Productions (rN reduces by production N) =\n";
    for (int k = 0; k < (int)rules.size(); ++k) {
        cout << k << ": " << rules[k].lhs << " -> ";
        if (rules[k].rhs.empty()) cout << EPS;
        for (auto symbol : rules[k].rhs) cout << symbol << ' ';
        cout << '\n';
    }
    cout << "Canonical LR(0) items =\n";
    for (int i = 0; i < (int)states.size(); ++i) {
        cout << "I" << i << " =\n";
        for (auto item : states[i]) {
            auto rule = rules[item.rule];
            cout << "  " << rule.lhs << " -> ";
            for (int j = 0; j <= (int)rule.rhs.size(); ++j) {
                if (j == item.dot) cout << ". ";
                if (j < (int)rule.rhs.size()) cout << rule.rhs[j] << ' ';
            }
            cout << '\n';
        }
    }
    Actions lr0, slr;
    bool lr0Valid = true, slrValid = true;
    for (auto edge : transitions) {
        if (grammar.count(edge.first.second)) go[edge.first] = edge.second;
        else {
            string action = "s" + to_string(edge.second);
            addAction(lr0, edge.first.first, edge.first.second, action, false);
            addAction(slr, edge.first.first, edge.first.second, action, false);
        }
    }
    for (int i = 0; i < (int)states.size(); ++i) {
        for (auto item : states[i]) {
            auto rule = rules[item.rule];
            if (item.dot != (int)rule.rhs.size()) continue;
            if (item.rule == 0) {
                if (!addAction(lr0, i, "$", "acc", false)) lr0Valid = false;
                if (!addAction(slr, i, "$", "acc", true)) slrValid = false;
            } else {
                string action = "r" + to_string(item.rule);
                for (auto terminal : terminals)
                    if (!addAction(lr0, i, terminal, action, false)) lr0Valid = false;
                for (auto terminal : follow[rule.lhs])
                    if (!addAction(slr, i, terminal, action, true)) slrValid = false;
            }
        }
    }
    cout << "LR(0) = " << (lr0Valid ? "YES" : "NO") << '\n';
    cout << "SLR(1) = " << (slrValid ? "YES" : "NO") << '\n';
    cout << "SLR(1) ACTION/GOTO table =\n";
    cout << setw(12) << "STATE";
    for (auto terminal : terminals) cout << setw(12) << terminal;
    for (auto entry : grammar) cout << setw(12) << entry.first;
    cout << '\n';
    for (int i = 0; i < (int)states.size(); ++i) {
        cout << setw(12) << i;
        for (auto terminal : terminals) {
            auto key = make_pair(i, terminal);
            cout << setw(12) << (slr.count(key) ? slr[key] : "-");
        }
        for (auto entry : grammar) {
            auto key = make_pair(i, entry.first);
            cout << setw(12) << (go.count(key) ? to_string(go[key]) : "-");
        }
        cout << '\n';
    }
    if (!slrValid) {
        cout << "Grammar has SLR(1) conflicts; parsing stopped.\n";
        return 1;
    }
    cout << "Enter space-separated terminals (blank for empty input; omit $) = ";
    string line, token;
    getline(cin, line);
    istringstream words(line);
    vector<string> input;
    while (words >> token) {
        if (token == "$" || !terminals.count(token)) {
            cout << "Invalid input terminal = " << token << '\n';
            return 1;
        }
        input.push_back(token);
    }
    input.push_back("$");
    vector<int> stack = {0};
    vector<string> symbolStack;
    size_t pos = 0;
    int step = 1;
    cout << "Parsing steps (stack bottom to top) =\n";
    cout << left << setw(6) << "STEP" << " | " << setw(32) << "STACK"
         << " | " << setw(32) << "INPUT" << " | ACTION\n";
    auto showStep = [&](string description) {
        string content = to_string(stack[0]), remaining;
        for (size_t i = 0; i < symbolStack.size(); ++i)
            content += " " + symbolStack[i] + " " + to_string(stack[i + 1]);
        for (size_t i = pos; i < input.size(); ++i) {
            if (i > pos) remaining += ' ';
            remaining += input[i];
        }
        cout << left << setw(6) << step++ << " | " << setw(32) << content
             << " | " << setw(32) << remaining << " | " << description << '\n';
    };
    while (pos < input.size()) {
        auto it = slr.find({stack.back(), input[pos]});
        if (it == slr.end()) {
            showStep("Error: no ACTION entry");
            break;
        }
        string command = it->second;
        if (command == "acc") {
            showStep("Accept");
            cout << "Result = ACCEPTED\n";
            return 0;
        }
        int number = stoi(command.substr(1));
        if (command[0] == 's') {
            showStep(command + ": Shift " + input[pos] + ", go to state " + to_string(number));
            symbolStack.push_back(input[pos]);
            stack.push_back(number);
            ++pos;
        } else {
            auto rule = rules[number];
            string description = command + ": Reduce " + rule.lhs + " ->";
            if (rule.rhs.empty()) description += " " + EPS;
            for (auto symbol : rule.rhs) description += " " + symbol;
            showStep(description);
            for (size_t k = 0; k < rule.rhs.size(); ++k) {
                stack.pop_back();
                symbolStack.pop_back();
            }
            auto dest = go.find({stack.back(), rule.lhs});
            if (dest == go.end()) {
                showStep("Error: no GOTO entry");
                break;
            }
            symbolStack.push_back(rule.lhs);
            stack.push_back(dest->second);
        }
    }
    cout << "Result = REJECTED\n";
    return 0;
}
