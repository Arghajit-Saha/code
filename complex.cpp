#include <bits/stdc++.h>
using namespace std;

class Complex {
    double real, imag;

public:
    Complex(double r = 0, double i = 0);

    Complex operator+(const Complex &c) const;
    Complex operator-(const Complex &c) const;
    Complex operator*(const Complex &c) const;
    Complex operator/(const Complex &c) const;
    Complex operator!() const;

    bool operator==(const Complex &c) const;
    bool operator!=(const Complex &c) const;

    Complex& operator=(const Complex &c);

    double operator[](int i) const;

    friend istream& operator>>(istream &in, Complex &c);
    friend ostream& operator<<(ostream &out, const Complex &c);

    operator double() const;
};

// ---------------- Definitions ----------------

Complex::Complex(double r, double i) {
    real = r;
    imag = i;
}

Complex Complex::operator+(const Complex &c) const {
    return Complex(real + c.real, imag + c.imag);
}

Complex Complex::operator-(const Complex &c) const {
    return Complex(real - c.real, imag - c.imag);
}

Complex Complex::operator*(const Complex &c) const {
    return Complex(real * c.real - imag * c.imag,
                   real * c.imag + imag * c.real);
}

Complex Complex::operator/(const Complex &c) const {
    double d = c.real * c.real + c.imag * c.imag;
    return Complex((real * c.real + imag * c.imag) / d,
                   (imag * c.real - real * c.imag) / d);
}

Complex Complex::operator!() const {
    return Complex(real, -imag);
}

bool Complex::operator==(const Complex &c) const {
    return (real == c.real && imag == c.imag);
}

bool Complex::operator!=(const Complex &c) const {
    return !(*this == c);
}

Complex& Complex::operator=(const Complex &c) {
    real = c.real;
    imag = c.imag;
    return *this;
}

double Complex::operator[](int i) const {
    if (i == 0) return real;
    else return imag;
}

istream& operator>>(istream &in, Complex &c) {
    in >> c.real >> c.imag;
    return in;
}

ostream& operator<<(ostream &out, const Complex &c) {
    out << c.real << "+" << c.imag << "i";
    return out;
}

Complex::operator double() const {
    return sqrt(real * real + imag * imag);
}

// ---------------- Main ----------------

int main() {
    Complex a, b;
    int choice;

    cout << "Enter first complex (real imag): ";
    cin >> a;
    cout << "Enter second complex (real imag): ";
    cin >> b;

    do {
        cout << "\n1. Add\n2. Subtract\n3. Multiply\n4. Divide\n5. Conjugate(a)\n6. Compare\n7. Modulus(a)\n0. Exit\nChoice: ";
        cin >> choice;

        switch(choice) {
            case 1: cout << (a + b) << "\n"; break;
            case 2: cout << (a - b) << "\n"; break;
            case 3: cout << (a * b) << "\n"; break;
            case 4: cout << (a / b) << "\n"; break;
            case 5: cout << (!a) << "\n"; break;
            case 6: cout << (a == b ? "Equal" : "Not Equal") << "\n"; break;
            case 7: cout << (double)a << "\n"; break;
        }
    } while(choice != 0);
}
