#include <bits/stdc++.h>
using namespace std;

class Complex {
    double real, imag;

public:
    Complex(double r = 0, double i = 0) {
        real = r;
        imag = i;
    }

    Complex operator+(const Complex &c) {
        return Complex(real + c.real, imag + c.imag);
    }

    Complex operator-(const Complex &c) {
        return Complex(real - c.real, imag - c.imag);
    }

    Complex operator*(const Complex &c) {
        return Complex(real * c.real - imag * c.imag,
                       real * c.imag + imag * c.real);
    }

    Complex operator/(const Complex &c) {
        double d = c.real * c.real + c.imag * c.imag;
        return Complex((real * c.real + imag * c.imag) / d,
                       (imag * c.real - real * c.imag) / d);
    }

    Complex operator!() {
        return Complex(real, -imag);
    }

    bool operator==(const Complex &c) {
        return (real == c.real && imag == c.imag);
    }

    bool operator!=(const Complex &c) {
        return !(*this == c);
    }

    Complex& operator=(const Complex &c) {
        real = c.real;
        imag = c.imag;
        return *this;
    }

    double operator[](int i) {
        if (i == 0) return real;
        else return imag;
    }

    friend istream& operator>>(istream &in, Complex &c) {
        in >> c.real >> c.imag;
        return in;
    }

    friend ostream& operator<<(ostream &out, const Complex &c) {
        out << c.real << "+" << c.imag << "i";
        return out;
    }

    operator double() {
        return sqrt(real * real + imag * imag);
    }
};

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
