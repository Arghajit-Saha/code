#include <bits/stdc++.h>
using namespace std;

class Set {
    vector<int> elems;

public:
    Set() {}

    Set(const Set &s) {
        elems = s.elems;
    }

    void insert(int x) {
        if (find(elems.begin(), elems.end(), x) == elems.end()) {
            elems.push_back(x);
        }
    }

    Set operator+(const Set &s) {
        Set r = *this;
        for (int x : s.elems) {
            r.insert(x);
        }
        return r;
    }

    Set operator-(const Set &s) {
        Set r;
        for (int x : elems) {
            if (find(s.elems.begin(), s.elems.end(), x) == s.elems.end()) {
                r.insert(x);
            }
        }
        return r;
    }

    Set operator*(const Set &s) {
        Set r;
        for (int x : elems) {
            if (find(s.elems.begin(), s.elems.end(), x) != s.elems.end()) {
                r.insert(x);
            }
        }
        return r;
    }

    bool operator<(const Set &s) {
        for (int x : elems) {
            if (find(s.elems.begin(), s.elems.end(), x) == s.elems.end()) {
                return false;
            }
        }
        return true;
    }

    bool operator==(const Set &s) {
        return (elems.size() == s.elems.size() && *this < s);
    }

    friend istream& operator>>(istream &in, Set &s) {
        int n;
        in >> n;
        for (int i = 0; i < n; i++) {
            int x;
            in >> x;
            s.insert(x);
        }
        return in;
    }

    friend ostream& operator<<(ostream &out, const Set &s) {
        out << "{";
        for (size_t i = 0; i < s.elems.size(); i++) {
            out << s.elems[i];
            if (i < s.elems.size() - 1) {
                out << ",";
            }
        }
        out << "}";
        return out;
    }
};

int main() {
    Set a, b;
    int choice;

    cout << "Enter size and elements of Set A: ";
    cin >> a;
    cout << "Enter size and elements of Set B: ";
    cin >> b;

    do {
        cout << "\n1. Union\n2. Difference\n3. Intersection\n4. Subset(A<B)\n5. Equal\n0. Exit\nChoice: ";
        cin >> choice;

        switch(choice) {
            case 1: cout << (a + b) << "\n"; break;
            case 2: cout << (a - b) << "\n"; break;
            case 3: cout << (a * b) << "\n"; break;
            case 4: cout << (a < b ? "Yes" : "No") << "\n"; break;
            case 5: cout << (a == b ? "Equal" : "Not Equal") << "\n"; break;
        }
    } while(choice != 0);
}
