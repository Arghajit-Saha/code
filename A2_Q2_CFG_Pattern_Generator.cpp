#include <bits/stdc++.h>
using namespace std;

string generate(int n) {
    string result;
    while (n--) result += "aab";
    result += "aa";
    return result;
}

int main() {
    int n;
    cout << "Enter n = ";
    if (!(cin >> n) || n <= 0) { cerr << "n must be positive\n"; return 1; }
    cout << "CFG = S -> aabS | aabaa\n";
    cout << "Language = { (aab)^n aa | n > 0 }\n";
    cout << "Generated string = " << generate(n) << '\n';
    cout << "Given string matches n = " << (generate(2) == "aabaabaa" ? "2" : "none") << '\n';
}
