#include <bits/stdc++.h>
using namespace std;

class Fraction {
    int num, den;

    void reduce() {
        int g = gcd(num, den);
        num /= g;
        den /= g;
        if (den < 0) {
            num = -num;
            den = -den;
        }
    }

public:
    Fraction(int n = 0, int d = 1) {
        num = n;
        den = d;
        reduce();
    }

    Fraction operator+(const Fraction &f) {
        return Fraction(num * f.den + f.num * den, den * f.den);
    }

    Fraction operator-(const Fraction &f) {
        return Fraction(num * f.den - f.num * den, den * f.den);
    }

    Fraction operator*(const Fraction &f) {
        return Fraction(num * f.num, den * f.den);
    }

    Fraction operator/(const Fraction &f) {
        return Fraction(num * f.den, den * f.num);
    }

    bool operator==(const Fraction &f) {
        return (num == f.num && den == f.den);
    }

    bool operator!=(const Fraction &f) {
        return !(*this == f);
    }

    bool operator<(const Fraction &f) {
        return (num * f.den < f.num * den);
    }

    bool operator>(const Fraction &f) {
        return f < *this;
    }

    Fraction& operator=(const Fraction &f) {
        num = f.num;
        den = f.den;
        return *this;
    }

    int operator[](int i) {
        if (i == 0) return num;
        else return den;
    }

    friend istream& operator>>(istream &in, Fraction &f) {
        in >> f.num >> f.den;
        f.reduce();
        return in;
    }

    friend ostream& operator<<(ostream &out, const Fraction &f) {
        out << f.num << "/" << f.den;
        return out;
    }

    operator double() {
        return (double)num / den;
    }
};

int main() {
    Fraction a, b;
    int choice;

    cout << "Enter first fraction (num den): ";
    cin >> a;
    cout << "Enter second fraction (num den): ";
    cin >> b;

    do {
        cout << "\n1. Add\n2. Subtract\n3. Multiply\n4. Divide\n5. Compare\n6. Double(a)\n0. Exit\nChoice: ";
        cin >> choice;

        switch(choice) {
            case 1: cout << (a + b) << "\n"; break;
            case 2: cout << (a - b) << "\n"; break;
            case 3: cout << (a * b) << "\n"; break;
            case 4: cout << (a / b) << "\n"; break;
            case 5: cout << (a == b ? "Equal" : "Not Equal") << "\n"; break;
            case 6: cout << (double)a << "\n"; break;
        }
    } while(choice != 0);
}
