#include <bits/stdc++.h>
using namespace std;

class Fraction {
    int num, den;
    void reduce();

public:
    Fraction(int n = 0, int d = 1);

    Fraction operator+(Fraction &f);
    Fraction operator-(Fraction &f);
    Fraction operator*(Fraction &f);
    Fraction operator/(Fraction &f);

    bool operator==(Fraction &f);
    bool operator!=(Fraction &f);
    bool operator<(Fraction &f);
    bool operator>(Fraction &f);

    Fraction& operator=(Fraction &f);
    int operator[](int i);
    operator double();

    void input();
    void print();
};

// ---------------- Definitions ----------------

Fraction::Fraction(int n, int d) {
    num = n;
    den = d;
    reduce();
}

void Fraction::reduce() {
    int g = __gcd(num, den);
    num /= g;
    den /= g;
    if (den < 0) {
        num = -num;
        den = -den;
    }
}

Fraction Fraction::operator+(Fraction &f) {
    return Fraction(num * f.den + f.num * den, den * f.den);
}

Fraction Fraction::operator-(Fraction &f) {
    return Fraction(num * f.den - f.num * den, den * f.den);
}

Fraction Fraction::operator*(Fraction &f) {
    return Fraction(num * f.num, den * f.den);
}

Fraction Fraction::operator/(Fraction &f) {
    return Fraction(num * f.den, den * f.num);
}

bool Fraction::operator==(Fraction &f) {
    return (num == f.num && den == f.den);
}

bool Fraction::operator!=(Fraction &f) {
    return !(*this == f);
}

bool Fraction::operator<(Fraction &f) {
    return (num * f.den < f.num * den);
}

bool Fraction::operator>(Fraction &f) {
    return f < *this;
}

Fraction& Fraction::operator=(Fraction &f) {
    num = f.num;
    den = f.den;
    return *this;
}

int Fraction::operator[](int i) {
    if (i == 0) return num;
    else return den;
}

Fraction::operator double() {
    return (double)num / den;
}

void Fraction::input() {
    cin >> num >> den;
    reduce();
}

void Fraction::print() {
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
