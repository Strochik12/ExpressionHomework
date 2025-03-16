#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <complex>
#include <memory>
#include <map>
#include <string>
#include <type_traits>

const std::pair<char, std::string> UNARY_OPERATORS[4] = {{'s', "sin"}, {'c', "cos"}, {'l', "ln"}, {'e', "exp"}};

using T = long double;

class Expression {
public:
    virtual ~Expression() = default;
    virtual std::unique_ptr<Expression> clone() const = 0;

    static std::unique_ptr<Expression> create(T);
    static std::unique_ptr<Expression> create(std::string);

    virtual T evaluate(const std::map<std::string, T>& = {}) const = 0;
    virtual std::unique_ptr<Expression> differentiate(std::string x) const = 0;
    virtual std::unique_ptr<Expression> specify(std::string, T) = 0;
    virtual std::string to_string() const = 0;

    // -1: нет переменных | 0: есть переменные | 1: (0 - epxr) (пока не сделал)
    virtual std::pair<std::unique_ptr<Expression>, int> simplify() = 0;
};

class Constant : public Expression {
    T value;
public:
    Constant(T);
    std::unique_ptr<Expression> clone() const override;

    T evaluate(const std::map<std::string, T>& = {}) const override;
    std::unique_ptr<Expression> differentiate(std::string x) const override;
    std::unique_ptr<Expression> specify(std::string, T) override;
    std::string to_string() const override;
    std::pair<std::unique_ptr<Expression>, int> simplify() override;
};

class Variable : public Expression {
    std::string name;
public:
    Variable(std::string);
    std::unique_ptr<Expression> clone() const override;

    T evaluate(const std::map<std::string, T>& = {}) const override;
    std::unique_ptr<Expression> differentiate(std::string x) const override;
    std::unique_ptr<Expression> specify(std::string, T) override;
    std::string to_string() const override;
    std::pair<std::unique_ptr<Expression>, int> simplify() override;
};

class Binary : public Expression {
    char op;
    std::unique_ptr<Expression> left, right;
public:
    Binary(char, std::unique_ptr<Expression>, std::unique_ptr<Expression>);
    Binary(const Binary &);
    Binary& operator=(const Binary &);
    std::unique_ptr<Expression> clone() const override;

    T evaluate(const std::map<std::string, T>& = {}) const override;
    std::unique_ptr<Expression> differentiate(std::string x) const override;
    std::unique_ptr<Expression> specify(std::string, T) override;
    std::string to_string() const override;
    std::pair<std::unique_ptr<Expression>, int> simplify() override;
};

class Unary : public Expression {
    char op;
    std::unique_ptr<Expression> expr;
public:
    Unary(char, std::unique_ptr<Expression>);
    Unary(const Unary &);
    Unary& operator=(const Unary &);
    std::unique_ptr<Expression> clone() const override;

    T evaluate(const std::map<std::string, T>& = {}) const override;
    std::unique_ptr<Expression> differentiate(std::string x) const override;
    std::unique_ptr<Expression> specify(std::string, T) override;
    std::string to_string() const override;
    std::pair<std::unique_ptr<Expression>, int> simplify() override;
};

bool is_number(std::string);
bool is_name(std::string);
template <typename L>
L to_number(std::string source) {
    bool im = (source.back() == 'i');
    if (im) source.pop_back();
    long double int_part = 0;
    int i = 0;
    for (; i < source.length() && source[i] != '.' && source[i] != ','; ++i) {
        int_part *= 10;
        int_part += (long double)(source[i] - '0');
    }
    long double frac_part = 0;
    for (int j = source.length() - 1; j > i; --j) {
        frac_part += (long double)(source[j] - '0');
        frac_part /= 10;
    }
    long double res = int_part + frac_part;
    if constexpr (std::is_same_v<T, std::complex<long double>>) {
        if (im) return std::complex<long double>(0, res);
    }
    return L(res);
}

void simplify(std::unique_ptr<Expression> &);
size_t find_close(std::string);
size_t find_operator(std::string);
std::string delete_zeros(std::string);

namespace std {
    std::string to_string(std::complex<long double>);
}

#endif