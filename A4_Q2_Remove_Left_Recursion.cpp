#include <bits/stdc++.h>
using namespace std;

using Grammar = map<string, vector<vector<string>>>;

void eliminateDirect(Grammar& g, string nt) {
    vector<vector<string>> alpha, beta;
    for (auto rhs : g[nt]) {
        if (!rhs.empty() && rhs[0] == nt) alpha.emplace_back(rhs.begin() + 1, rhs.end());
        else beta.push_back(rhs);
    }
    if (alpha.empty()) return;
    string next = nt + "'";
    while (g.count(next)) next += "'";
    vector<vector<string>> base, recursive;
    for (auto rhs : beta) {
        if (rhs == vector<string>{"eps"}) rhs.clear();
        rhs.push_back(next);
        base.push_back(rhs);
    }
    for (auto rhs : alpha) {
        rhs.push_back(next);
        recursive.push_back(rhs);
    }
    recursive.push_back({"eps"});
    g[nt] = base;
    g[next] = recursive;
}

void show(Grammar g) {
    for (auto& [nt, rules] : g) {
        cout << nt << " -> ";
        for (size_t i = 0; i < rules.size(); ++i) {
            if (i) cout << " | ";
            for (auto& s : rules[i]) cout << s << ' ';
        }
        cout << '\n';
    }
}

int main() {
    Grammar g = {
        {"E", {{"E", "+", "T"}, {"T"}}},
        {"T", {{"T", "*", "F"}, {"F"}}},
        {"F", {{"(", "E", ")"}, {"id"}}}
    };
    cout << "Original grammar =\n"; show(g);
    for (string nt : {"E", "T", "F"}) eliminateDirect(g, nt);
    cout << "LL(1) grammar =\n"; show(g);
}
