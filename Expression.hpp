#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <complex>
#include <memory>
#include <map>
#include <string>

//Хотел сделать с template, но по адекватному тогда нужно реализацию в hpp :((
using T = std::complex<long double>;
//using T = long double;

class Expression {
public:
    virtual ~Expression() = default;

    static std::unique_ptr<Expression> create(T);
    static std::unique_ptr<Expression> create(std::string);

    virtual T evaluate(std::map<char, T> &) const = 0;
    virtual std::unique_ptr<Expression> specify(char, T) = 0;
};

class Constant : public Expression {
    T value;
public:
    Constant(T);

    T evaluate(std::map<char, T> &) const override;
    std::unique_ptr<Expression> specify(char, T) override;
};

class Variable : public Expression {
    char name;
public:
    Variable(char);

    T evaluate(std::map<char, T> &) const override;
    std::unique_ptr<Expression> specify(char, T) override;
};

class Binary : public Expression {
    char op;
    std::unique_ptr<Expression> left, right;
public:
    Binary(char, std::unique_ptr<Expression>, std::unique_ptr<Expression>);
    Binary(const Binary &);
    Binary& operator=(const Binary &);

    T evaluate(std::map<char, T> &) const override;
    std::unique_ptr<Expression> specify(char, T) override;
};

class Unary : public Expression {
    char op;
    std::unique_ptr<Expression> expr;
public:
    Unary(char, std::unique_ptr<Expression>);
    Unary(const Unary &);
    Unary& operator=(const Unary &);

    T evaluate(std::map<char, T> &) const override;
    std::unique_ptr<Expression> specify(char, T) override;
};


#endif