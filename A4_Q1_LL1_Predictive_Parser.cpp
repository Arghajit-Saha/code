#include <bits/stdc++.h>
using namespace std;

using Grammar = map<string, vector<vector<string>>>;
string EPS = "eps";
Grammar grammar = {
    {"S", {{"a", "A", "B", "e"}}},
    {"A", {{"b", "A", "c"}, {"d"}}},
    {"B", {{"f"}, {EPS}}}
};
string startSymbol = "S";
map<string, set<string>> first, follow;
map<pair<string,string>, vector<string>> table;

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

bool addEntry(string lhs, string terminal, vector<string> rhs) {
    auto key = make_pair(lhs, terminal);
    if (table.count(key) && table[key] != rhs) {
        cout << "Conflict at M[" << lhs << ", " << terminal << "] = ";
        for (auto symbol : table[key]) cout << symbol << ' ';
        cout << "versus ";
        for (auto symbol : rhs) cout << symbol << ' ';
        cout << '\n';
        return false;
    }
    table[key] = rhs;
    return true;
}

bool buildTable() {
    bool valid = true;
    for (auto& [lhs, productions] : grammar)
        for (auto& rhs : productions) {
            auto f = firstOf(rhs);
            for (auto& t : f) if (t != EPS) {
                if (!addEntry(lhs, t, rhs)) valid = false;
            }
            if (f.count(EPS)) for (auto& t : follow[lhs]) {
                if (!addEntry(lhs, t, rhs)) valid = false;
            }
        }
    return valid;
}

bool parse(vector<string> input) {
    input.push_back("$");
    vector<string> st = {"$", startSymbol};
    size_t pos = 0;
    while (!st.empty()) {
        string top = st.back(); st.pop_back();
        if (pos >= input.size()) return false;
        if (!grammar.count(top)) {
            if (top != input[pos++]) return false;
        } else {
            auto it = table.find({top, input[pos]});
            if (it == table.end()) return false;
            for (auto rit = it->second.rbegin(); rit != it->second.rend(); ++rit)
                if (*rit != EPS) st.push_back(*rit);
        }
    }
    return pos == input.size();
}

int main() {
    cout << "Grammar = S -> a A B e, A -> b A c | d, B -> f | eps\n";
    computeFirst(); computeFollow();
    bool valid = buildTable();
    cout << "FIRST sets =\n";
    for (auto& [nt, values] : first) {
        cout << nt << " = "; for (auto& v : values) cout << v << ' '; cout << '\n';
    }
    cout << "FOLLOW sets =\n";
    for (auto& [nt, values] : follow) {
        cout << nt << " = "; for (auto& v : values) cout << v << ' '; cout << '\n';
    }
    cout << "LL(1) = " << (valid ? "YES" : "NO") << '\n';
    cout << "Parsing table =\n";
    for (auto& [key, rhs] : table) {
        cout << "M[" << key.first << ", " << key.second << "] = " << key.first << " -> ";
        for (auto& s : rhs) cout << s << ' ';
        cout << '\n';
    }
    if (!valid) {
        cout << "Grammar is not LL(1). Resolve table conflicts before parsing.\n";
        return 1;
    }
    cout << "Enter space-separated terminals (blank line for empty input; omit $) = ";
    string line, t;
    getline(cin, line);
    istringstream stream(line);
    vector<string> input;
    while (stream >> t) input.push_back(t);
    if (find(input.begin(), input.end(), "$") != input.end() ||
        find(input.begin(), input.end(), EPS) != input.end()) {
        cout << "Use a blank line for empty input and omit $\n";
        return 1;
    }
    cout << "Result = " << (parse(input) ? "ACCEPTED" : "REJECTED") << '\n';
}
