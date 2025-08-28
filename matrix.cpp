#include <bits/stdc++.h>
using namespace std;

class Matrix {
    int n, m;
    vector<vector<int>> a;

public:
    Matrix(int n = 0, int m = 0) {
        this->n = n;
        this->m = m;
        a.assign(n, vector<int>(m, 0));
    }

    Matrix(const Matrix &mat) {
        n = mat.n;
        m = mat.m;
        a = mat.a;
    }

    Matrix operator+(const Matrix &mat) {
        Matrix r(n, m);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                r.a[i][j] = a[i][j] + mat.a[i][j];
            }
        }
        return r;
    }

    Matrix operator-(const Matrix &mat) {
        Matrix r(n, m);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                r.a[i][j] = a[i][j] - mat.a[i][j];
            }
        }
        return r;
    }

    Matrix operator*(const Matrix &mat) {
        Matrix r(n, mat.m);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < mat.m; j++) {
                for (int k = 0; k < m; k++) {
                    r.a[i][j] += a[i][k] * mat.a[k][j];
                }
            }
        }
        return r;
    }

    Matrix operator!() {
        Matrix r(m, n);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                r.a[j][i] = a[i][j];
            }
        }
        return r;
    }

    bool operator==(const Matrix &mat) {
        return a == mat.a;
    }

    Matrix& operator=(const Matrix &mat) {
        n = mat.n;
        m = mat.m;
        a = mat.a;
        return *this;
    }

    friend istream& operator>>(istream &in, Matrix &mat) {
        for (int i = 0; i < mat.n; i++) {
            for (int j = 0; j < mat.m; j++) {
                in >> mat.a[i][j];
            }
        }
        return in;
    }

    friend ostream& operator<<(ostream &out, const Matrix &mat) {
        for (int i = 0; i < mat.n; i++) {
            for (int j = 0; j < mat.m; j++) {
                out << mat.a[i][j] << " ";
            }
            out << "\n";
        }
        return out;
    }
};

int main() {
    int n, m;
    cout << "Enter size of matrices (n m): ";
    cin >> n >> m;

    Matrix a(n, m), b(n, m);
    cout << "Enter elements of A:\n";
    cin >> a;
    cout << "Enter elements of B:\n";
    cin >> b;

    int choice;
    do {
        cout << "\n1. Add\n2. Subtract\n3. Multiply\n4. Transpose(A)\n5. Compare\n0. Exit\nChoice: ";
        cin >> choice;

        switch(choice) {
            case 1: cout << (a + b); break;
            case 2: cout << (a - b); break;
            case 3: cout << (a * b); break;
            case 4: cout << (!a); break;
            case 5: cout << (a == b ? "Equal" : "Not Equal") << "\n"; break;
        }
    } while(choice != 0);
}
