#include <bits/stdc++.h>
using namespace std;

struct Production { string lhs, rhs; };
struct Item {
    int rule, dot;
};
bool operator<(Item a, Item b) { return tie(a.rule, a.dot) < tie(b.rule, b.dot); }
bool operator==(Item a, Item b) { return a.rule == b.rule && a.dot == b.dot; }
using State = set<Item>;
vector<Production> rules = {{"Z", "S"}, {"S", "A"}, {"A", "Ab"}, {"A", "a"}};
set<char> nonterminals = {'Z', 'S', 'A'};

State closure(State items) {
    bool changed;
    do {
        changed = false;
        auto old = items;
        for (auto item : old) {
            auto& r = rules[item.rule];
            if (item.dot >= (int)r.rhs.size()) continue;
            char symbol = r.rhs[item.dot];
            if (!nonterminals.count(symbol)) continue;
            for (int k = 0; k < (int)rules.size(); ++k)
                if (rules[k].lhs == string(1, symbol)) changed |= items.insert({k, 0}).second;
        }
    } while (changed);
    return items;
}

State nextState(State items, char symbol) {
    State moved;
    for (auto item : items) {
        auto& r = rules[item.rule];
        if (item.dot < (int)r.rhs.size() && r.rhs[item.dot] == symbol) moved.insert({item.rule, item.dot + 1});
    }
    return closure(moved);
}

int main() {
    vector<State> states = {closure({{0, 0}})};
    map<pair<int,char>, int> transitions;
    for (size_t i = 0; i < states.size(); ++i) {
        for (char symbol : string("aAbS")) {
            State next = nextState(states[i], symbol);
            if (next.empty()) continue;
            auto it = find(states.begin(), states.end(), next);
            int dest = it == states.end() ? (int)states.size() : (int)distance(states.begin(), it);
            if (it == states.end()) states.push_back(next);
            transitions[{(int)i, symbol}] = dest;
        }
    }
    map<pair<int,char>, string> action;
    map<pair<int,char>, int> go;
    bool conflict = false;
    auto addAction = [&](int state, char symbol, string value) {
        auto key = make_pair(state, symbol);
        if (action.count(key) && action[key] != value) conflict = true;
        action[key] = value;
    };
    for (auto& [edge, destination] : transitions) {
        if (nonterminals.count(edge.second)) go[edge] = destination;
        else addAction(edge.first, edge.second, "s" + to_string(destination));
    }
    for (int i = 0; i < (int)states.size(); ++i)
        for (auto item : states[i]) {
            if (item.dot != (int)rules[item.rule].rhs.size()) continue;
            if (item.rule == 0) addAction(i, '$', "acc");
            else for (char t : (item.rule == 1 ? string("$") : string("b$"))) addAction(i, t, "r" + to_string(item.rule));
        }
    cout << "Grammar = S -> A, A -> Ab | a\n";
    cout << "Canonical LR(0) items =\n";
    for (int i = 0; i < (int)states.size(); ++i) {
        cout << "I" << i << " =\n";
        for (auto item : states[i]) {
            auto r = rules[item.rule];
            cout << "  " << r.lhs << " -> " << r.rhs.substr(0, item.dot) << '.' << r.rhs.substr(item.dot) << '\n';
        }
    }
    cout << "LR(0) status = NOT LR(0), shift/reduce conflict at completed S -> A and shift b\n";
    cout << "SLR(1) parsing table =\n";
    cout << "STATE   a     b     $     A     S\n";
    for (int i = 0; i < (int)states.size(); ++i) {
        cout << setw(8) << i;
        for (char symbol : string("ab$AS")) {
            string value = action.count({i, symbol}) ? action[{i, symbol}] : go.count({i, symbol}) ? to_string(go[{i, symbol}]) : "-";
            cout << setw(6) << value;
        }
        cout << '\n';
    }
    cout << "Conflicts = " << (conflict ? "YES" : "NO") << '\n';
    if (conflict) return 1;
    cout << "Input string = ";
    string input; getline(cin, input); input += '$';
    vector<int> stack = {0};
    vector<string> symbolStack;
    size_t pos = 0;
    int step = 1;
    cout << "Parsing steps (stack bottom to top) =\n";
    cout << left << setw(6) << "STEP" << " | " << setw(32) << "STACK"
         << " | " << setw(24) << "INPUT" << " | ACTION\n";
    auto showStep = [&](string description) {
        string content = to_string(stack[0]);
        for (size_t i = 0; i < symbolStack.size(); ++i)
            content += " " + symbolStack[i] + " " + to_string(stack[i + 1]);
        cout << left << setw(6) << step++ << " | " << setw(32) << content
             << " | " << setw(24) << input.substr(pos) << " | " << description << '\n';
    };
    while (pos < input.size()) {
        string command = action[{stack.back(), input[pos]}];
        if (command == "acc") {
            showStep("Accept");
            cout << "Result = ACCEPTED\n";
            return 0;
        }
        if (command.empty()) {
            showStep("Error: no ACTION entry");
            break;
        }
        int number = stoi(command.substr(1));
        if (command[0] == 's') {
            showStep(command + ": Shift " + string(1, input[pos]) + ", go to state " + to_string(number));
            symbolStack.push_back(string(1, input[pos]));
            stack.push_back(number);
            ++pos;
        }
        else if (command[0] == 'r') {
            showStep(command + ": Reduce " + rules[number].lhs + " -> " + rules[number].rhs);
            for (size_t k = 0; k < rules[number].rhs.size(); ++k) {
                stack.pop_back();
                symbolStack.pop_back();
            }
            char lhs = rules[number].lhs[0];
            if (!go.count({stack.back(), lhs})) {
                showStep("Error: no GOTO entry");
                break;
            }
            symbolStack.push_back(rules[number].lhs);
            stack.push_back(go[{stack.back(), lhs}]);
        } else break;
    }
    cout << "Result = REJECTED\n";
}
