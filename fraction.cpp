#include <bits/stdc++.h>
using namespace std;

class Fraction {
    int num, den;
    void reduce();

public:
    Fraction(int n = 0, int d = 1);

    Fraction operator+(const Fraction &f) const;
    Fraction operator-(const Fraction &f) const;
    Fraction operator*(const Fraction &f) const;
    Fraction operator/(const Fraction &f) const;

    bool operator==(const Fraction &f) const;
    bool operator!=(const Fraction &f) const;
    bool operator<(const Fraction &f) const;
    bool operator>(const Fraction &f) const;

    Fraction& operator=(const Fraction &f);
    int operator[](int i) const;
    operator double() const;

    void input();
    void print() const;
};

// ---------------- Definitions ----------------

Fraction::Fraction(int n, int d) {
    num = n;
    den = d;
    reduce();
}

void Fraction::reduce() {
    int g = gcd(num, den);
    num /= g;
    den /= g;
    if (den < 0) {
        num = -num;
        den = -den;
    }
}

Fraction Fraction::operator+(const Fraction &f) const {
    return Fraction(num * f.den + f.num * den, den * f.den);
}

Fraction Fraction::operator-(const Fraction &f) const {
    return Fraction(num * f.den - f.num * den, den * f.den);
}

Fraction Fraction::operator*(const Fraction &f) const {
    return Fraction(num * f.num, den * f.den);
}

Fraction Fraction::operator/(const Fraction &f) const {
    return Fraction(num * f.den, den * f.num);
}

bool Fraction::operator==(const Fraction &f) const {
    return (num == f.num && den == f.den);
}

bool Fraction::operator!=(const Fraction &f) const {
    return !(*this == f);
}

bool Fraction::operator<(const Fraction &f) const {
    return (num * f.den < f.num * den);
}

bool Fraction::operator>(const Fraction &f) const {
    return f < *this;
}

Fraction& Fraction::operator=(const Fraction &f) {
    num = f.num;
    den = f.den;
    return *this;
}

int Fraction::operator[](int i) const {
    if (i == 0) return num;
    else return den;
}

Fraction::operator double() const {
    return (double)num / den;
}

void Fraction::input() {
    cin >> num >> den;
    reduce();
}

void Fraction::print() const {
    cout << num << "/" << den;
}

// ---------------- Main ----------------

int main() {
    Fraction a, b;
    int choice;

    cout << "Enter first fraction (num den): ";
    a.input();
    cout << "Enter second fraction (num den): ";
    b.input();

    do {
        cout << "\n1. Add\n2. Subtract\n3. Multiply\n4. Divide\n5. Compare\n6. Double(a)\n0. Exit\nChoice: ";
        cin >> choice;

        switch (choice) {
            case 1: (a + b).print(); cout << "\n"; break;
            case 2: (a - b).print(); cout << "\n"; break;
            case 3: (a * b).print(); cout << "\n"; break;
            case 4: (a / b).print(); cout << "\n"; break;
            case 5: cout << (a == b ? "Equal\n" : "Not Equal\n"); break;
            case 6: cout << (double)a << "\n"; break;
        }
    } while (choice != 0);
}
