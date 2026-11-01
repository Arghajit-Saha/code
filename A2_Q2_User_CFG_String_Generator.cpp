#include <bits/stdc++.h>
using namespace std;

bool nonterminal(char symbol) {
    return symbol >= 'A' && symbol <= 'Z';
}

set<string> generate(map<char, vector<string>>& grammar, char start, int n) {
    set<string> current = {string(1, start)};
    for (int step = 0; step < n && !current.empty(); ++step) {
        set<string> next;
        for (auto form : current) {
            size_t position = 0;
            while (position < form.size() && !nonterminal(form[position])) ++position;
            if (position == form.size()) continue;
            for (auto rhs : grammar[form[position]])
                next.insert(form.substr(0, position) + rhs + form.substr(position + 1));
        }
        current = move(next);
    }
    set<string> result;
    for (auto form : current) {
        bool complete = true;
        for (char symbol : form)
            if (nonterminal(symbol)) complete = false;
        if (complete) result.insert(form);
    }
    return result;
}

int main() {
    map<char, vector<string>> grammar;
    int count, n;
    cout << "Use single uppercase letters for nonterminals.\n";
    cout << "Write rules like S -> aabS | aabaa. Use eps for the empty string.\n";
    cout << "Number of production lines = ";
    if (!(cin >> count) || count <= 0) {
        cout << "Number of production lines must be positive\n";
        return 1;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    for (int i = 0; i < count; ++i) {
        string line;
        cout << "Production " << i + 1 << " = ";
        if (!getline(cin, line)) {
            cout << "Missing production\n";
            return 1;
        }
        line.erase(remove_if(line.begin(), line.end(), [](unsigned char c) {
            return isspace(c);
        }), line.end());
        if (line.size() < 4 || !nonterminal(line[0]) || line.substr(1, 2) != "->") {
            cout << "Invalid production. Expected A -> alternative | alternative\n";
            return 1;
        }
        size_t begin = 3;
        while (true) {
            size_t end = line.find('|', begin);
            string rhs = line.substr(begin, end == string::npos ? end : end - begin);
            if (rhs.empty() || rhs.find("->") != string::npos) {
                cout << "Invalid alternative. Use eps for the empty string\n";
                return 1;
            }
            grammar[line[0]].push_back(rhs == "eps" ? "" : rhs);
            if (end == string::npos) break;
            begin = end + 1;
        }
    }
    for (auto entry : grammar)
        for (auto rhs : entry.second)
            for (char symbol : rhs)
                if (nonterminal(symbol) && !grammar.count(symbol)) {
                    cout << "Missing productions for nonterminal = " << symbol << '\n';
                    return 1;
                }
    string start;
    cout << "Start symbol = ";
    if (!(cin >> start) || start.size() != 1 || !grammar.count(start[0])) {
        cout << "Start symbol must be a defined nonterminal\n";
        return 1;
    }
    cout << "Enter n (exact number of production applications) = ";
    if (!(cin >> n) || n < 0) {
        cout << "n must be nonnegative\n";
        return 1;
    }
    auto strings = generate(grammar, start[0], n);
    cout << "Strings generated using exactly " << n << " production applications =\n";
    if (strings.empty()) cout << "No strings generated\n";
    for (auto value : strings) cout << (value.empty() ? "eps" : value) << '\n';
    return 0;
}
