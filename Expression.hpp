#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <complex>
#include <memory>
#include <map>
#include <string>
#include <type_traits>

const std::pair<char, std::string> UNARY_OPERATORS[4] = {{'s', "sin"}, {'c', "cos"}, {'l', "ln"}, {'e', "exp"}};

//Хотел сделать с template, но по адекватному тогда нужно реализацию в hpp :((
//using T = std::complex<long double>;
using T = long double;

class Expression {
public:
    virtual ~Expression() = default;
    virtual std::unique_ptr<Expression> clone() const = 0;

    static std::unique_ptr<Expression> create(T);
    static std::unique_ptr<Expression> create(std::string);

    virtual T evaluate(const std::map<char, T>& = {}) const = 0;
    virtual std::unique_ptr<Expression> differentiate(char x) const = 0;
    virtual std::unique_ptr<Expression> specify(char, T) = 0;
    virtual std::string to_string() const = 0;
};

class Constant : public Expression {
    T value;
public:
    Constant(T);
    std::unique_ptr<Expression> clone() const override;

    T evaluate(const std::map<char, T>& = {}) const override;
    std::unique_ptr<Expression> differentiate(char x) const override;
    std::unique_ptr<Expression> specify(char, T) override;
    std::string to_string() const override;
};

class Variable : public Expression {
    char name;
public:
    Variable(char);
    std::unique_ptr<Expression> clone() const override;

    T evaluate(const std::map<char, T>& = {}) const override;
    std::unique_ptr<Expression> differentiate(char x) const override;
    std::unique_ptr<Expression> specify(char, T) override;
    std::string to_string() const override;
};

class Binary : public Expression {
    char op;
    std::unique_ptr<Expression> left, right;
public:
    Binary(char, std::unique_ptr<Expression>, std::unique_ptr<Expression>);
    Binary(const Binary &);
    Binary& operator=(const Binary &);
    std::unique_ptr<Expression> clone() const override;

    T evaluate(const std::map<char, T>& = {}) const override;
    std::unique_ptr<Expression> differentiate(char x) const override;
    std::unique_ptr<Expression> specify(char, T) override;
    std::string to_string() const override;
};

class Unary : public Expression {
    char op;
    std::unique_ptr<Expression> expr;
public:
    Unary(char, std::unique_ptr<Expression>);
    Unary(const Unary &);
    Unary& operator=(const Unary &);
    std::unique_ptr<Expression> clone() const override;

    T evaluate(const std::map<char, T>& = {}) const override;
    std::unique_ptr<Expression> differentiate(char x) const override;
    std::unique_ptr<Expression> specify(char, T) override;
    std::string to_string() const override;
};


bool is_number(std::string);
template <typename L>
L to_number(std::string);
size_t find_close(std::string);
size_t find_operator(std::string);
std::string delete_zeros(std::string);

// Я офигел от своей гениальности
namespace std {
    std::string to_string(std::complex<long double>);
}

#endif