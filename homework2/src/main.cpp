#include <iostream>
#include <algorithm>
#include <cmath>
using namespace std;

class Polynomial;

class Term {
    friend class Polynomial;
    friend ostream& operator<<(ostream& os, const Polynomial& p);//多項式輸出
    friend istream& operator>>(istream& is, Polynomial& p);//多項式輸入
private:
    float coef;//係數
    int exp;//次方
};

class Polynomial {
public:
    Polynomial();
    Polynomial(const Polynomial& p);
    ~Polynomial();//
    Polynomial& operator=(const Polynomial& p);

    Polynomial Add(Polynomial poly);
    Polynomial Mult(Polynomial poly);
    float Eval(float f);

    void NewTerm(const float theCoeff, const int theExp);

    friend ostream& operator<<(ostream& os, const Polynomial& p);
    friend istream& operator>>(istream& is, Polynomial& p);

private:
    Term *termArray;
    int capacity;
    int terms;
};

// 建構子
Polynomial::Polynomial() {
    capacity = 1;
    terms = 0;
    termArray = new Term[capacity];
}

// 複製建構子
Polynomial::Polynomial(const Polynomial& p) {
    capacity = p.capacity;
    terms = p.terms;
    termArray = new Term[capacity];
    copy(p.termArray, p.termArray + terms, termArray);
}

// 解構子
Polynomial::~Polynomial() {
    delete[] termArray;
}

// 賦值運算子
Polynomial& Polynomial::operator=(const Polynomial& p) {
    if (this != &p) {
        delete[] termArray;
        capacity = p.capacity;
        terms = p.terms;
        termArray = new Term[capacity];
        copy(p.termArray, p.termArray + terms, termArray);
    }
    return *this;
}

// 新增非0項 (課本 2.9)
void Polynomial::NewTerm(const float theCoeff, const int theExp) {
    if (theCoeff == 0) return;

    // 檢查是否有同次方項，有的話累加係數
    for (int i = 0; i < terms; i++) {
        if (termArray[i].exp == theExp) {
            termArray[i].coef += theCoeff;
            // 相加後若剛好抵消為 0 則移除此項
            if (termArray[i].coef == 0) {
                for (int j = i; j < terms - 1; j++) {
                    termArray[j] = termArray[j + 1];
                }
                terms--;
                if (terms < 0) terms = 0; // 防止變成負數引發崩潰
            }
            return;
        }
    }

    // 空間不足時加倍擴充(段考考題)
    if (terms == capacity) {
        capacity *= 2;
        Term *temp = new Term[capacity];
        copy(termArray, termArray + terms, temp);
        delete[] termArray;
        termArray = temp;
    }
    termArray[terms].coef = theCoeff;
    termArray[terms].exp = theExp;
    terms++;
}

// 多項式相加 (課本 2.8)
Polynomial Polynomial::Add(Polynomial b) {
    Polynomial c;
    int aPos = 0, bPos = 0;

    while ((aPos < terms) && (bPos < b.terms)) {
        if (termArray[aPos].exp == b.termArray[bPos].exp) {
            float t = termArray[aPos].coef + b.termArray[bPos].coef;
            if (t != 0) c.NewTerm(t, termArray[aPos].exp);
            aPos++;
            bPos++;
        }
        else if (termArray[aPos].exp < b.termArray[bPos].exp) {
            c.NewTerm(b.termArray[bPos].coef, b.termArray[bPos].exp);
            bPos++;
        }
        else {
            c.NewTerm(termArray[aPos].coef, termArray[aPos].exp);
            aPos++;
        }
    }

    for (; aPos < terms; aPos++)
        c.NewTerm(termArray[aPos].coef, termArray[aPos].exp);

    for (; bPos < b.terms; bPos++)
        c.NewTerm(b.termArray[bPos].coef, b.termArray[bPos].exp);

    return c;
}

// 多項式相乘
Polynomial Polynomial::Mult(Polynomial poly) {
    Polynomial ans;
    if (terms == 0 || poly.terms == 0) return ans;

    for (int i = 0; i < terms; i++) {
        for (int j = 0; j < poly.terms; j++) {
            float c = termArray[i].coef * poly.termArray[j].coef;
            int e = termArray[i].exp + poly.termArray[j].exp;
            ans.NewTerm(c, e);
        }
    }

    // 氣泡排序法由大到小排好
    for (int i = 0; i < ans.terms - 1; i++) {
        for (int j = 0; j < ans.terms - 1 - i; j++) {
            if (ans.termArray[j].exp < ans.termArray[j + 1].exp) {
                swap(ans.termArray[j], ans.termArray[j + 1]);
            }
        }
    }

    return ans;
}

// 代入數值求值
float Polynomial::Eval(float f) {
    float ans = 0;
    for (int i = 0; i < terms; i++) {
        ans += termArray[i].coef * pow(f, termArray[i].exp);
    }
    return ans;
}

// 多載輸入運算子
istream& operator>>(istream& is, Polynomial& p) {
    int n;
    cout << "請輸入非0項個數: ";
    is >> n;

    p.terms = 0;
    for (int i = 0; i < n; i++) {
        float c;
        int e;
        cout << "輸入第 " << i + 1 << " 項 (係數 次方): ";
        is >> c >> e;
        p.NewTerm(c, e);
    }

    // 氣泡排序由大到小排列次方
    for (int i = 0; i < p.terms - 1; i++) {
        for (int j = 0; j < p.terms - 1 - i; j++) {
            if (p.termArray[j].exp < p.termArray[j + 1].exp) {
                swap(p.termArray[j], p.termArray[j + 1]);
            }
        }
    }
    return is;
}

// 多載輸出運算子
ostream& operator<<(ostream& os, const Polynomial& p) {
    if (p.terms == 0) {
        os << "0";
        return os;
    }

    for (int i = 0; i < p.terms; i++) {
        float c = p.termArray[i].coef;
        int e = p.termArray[i].exp;

        if (i > 0) {
            if (c > 0) os << " + ";
            else {
                os << " - ";
                c = -c;
            }
        } else if (c < 0) {
            os << "-";
            c = -c;
        }

        if (e == 0) {
            os << c;
        } else {
            if (c != 1) os << c;
            os << "x";
            if (e != 1) os << "^" << e;
        }
    }
    return os;
}

int main() {
    Polynomial a, b;

    cout << "[輸入多項式 A]\n";
    cin >> a;
    cout << "A(x) = " << a << "\n\n";

    cout << "[輸入多項式 B]\n";
    cin >> b;
    cout << "B(x) = " << b << "\n\n";

    cout << "A(x) + B(x) = " << a.Add(b) << "\n";
    cout << "A(x) * B(x) = " << a.Mult(b) << "\n\n";

    float x;
    cout << "請輸入欲代入的 x 值: ";
    if (cin >> x) {
        cout << "A(" << x << ") = " << a.Eval(x) << "\n";
        cout << "B(" << x << ") = " << b.Eval(x) << "\n";
    }

    return 0;
}