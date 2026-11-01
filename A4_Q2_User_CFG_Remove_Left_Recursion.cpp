#include <bits/stdc++.h>
using namespace std;

using Grammar = map<string, vector<vector<string>>>;
string EPS = "eps";
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
    return true;
}

void eliminateDirect(Grammar& g, string nt) {
    vector<vector<string>> alpha, beta;
    bool recursive = false;
    for (auto rhs : g[nt]) {
        if (rhs == vector<string>{EPS}) rhs.clear();
        if (!rhs.empty() && rhs[0] == nt) {
            recursive = true;
            if (rhs.size() > 1) alpha.emplace_back(rhs.begin() + 1, rhs.end());
        } else beta.push_back(rhs);
    }
    if (!recursive) return;
    if (beta.empty()) {
        g[nt].clear();
        return;
    }
    if (alpha.empty()) {
        for (auto& rhs : beta) if (rhs.empty()) rhs.push_back(EPS);
        g[nt] = beta;
        return;
    }
    string next = nt + "'";
    while (g.count(next)) next += "'";
    for (auto& rhs : beta) rhs.push_back(next);
    for (auto& rhs : alpha) rhs.push_back(next);
    alpha.push_back({EPS});
    g[nt] = beta;
    g[next] = alpha;
}

void show(Grammar g) {
    for (auto& [nt, rules] : g) {
        cout << nt << " -> ";
        if (rules.empty()) cout << "[no productions]";
        for (size_t i = 0; i < rules.size(); ++i) {
            if (i) cout << " | ";
            for (auto& s : rules[i]) cout << s << ' ';
        }
        cout << '\n';
    }
}

int main() {
    if (!readGrammar()) return 1;
    cout << "Original grammar =\n";
    show(grammar);
    vector<string> names;
    for (auto entry : grammar) names.push_back(entry.first);
    for (auto name : names) eliminateDirect(grammar, name);
    cout << "Grammar after removing direct left recursion =\n";
    show(grammar);
    cout << "Indirect left recursion and LL(1) conflicts require separate checks.\n";
    return 0;
}
