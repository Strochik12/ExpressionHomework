#include "Expression.hpp"


std::unique_ptr<Expression> Expression::create(T val) {
    return std::make_unique<Constant>(val);
}
std::unique_ptr<Expression> Expression::create(std::string source) {
    ;
    //ОЧЕНЬ СЛОЖНЫЙ КОД ЗДЕСЬ
}


//--------------//
//---CONSTANT---//
//--------------//
Constant::Constant(T val) : value(val) {}
T Constant::evaluate(std::map<char, T> &x) const {
    return value;
}
std::unique_ptr<Expression> Constant::specify(char x, T val) {
    return nullptr;
}


//--------------//
//---VARIABLE---//
//--------------//
Variable::Variable(char x) : name(x) {}
T Variable::evaluate(std::map<char, T> &x) const {
    return x[name];
}
std::unique_ptr<Expression> Variable::specify(char x, T val) {
    if (x == name)
        return std::make_unique<Constant>(val);
    return nullptr;
}



//--------------//
//----BINARY----//
//--------------//
Binary::Binary(char operation, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
    : op(operation), left(std::move(l)), right(std::move(r)) {}
Binary::Binary(const Binary &other) {
    op = other.op;
    left = std::make_unique<Expression>(*other.left);
    right = std::make_unique<Expression>(*other.right);
}
Binary& Binary::operator=(const Binary &other) {
    if (this == &other) return *this;
    op = other.op;
    left = std::make_unique<Expression>(*other.left);
    right = std::make_unique<Expression>(*other.left);
    return *this;
}
T Binary::evaluate(std::map<char, T> &x) const {
    auto l = left->evaluate(x);
    auto r = right->evaluate(x);
    switch (op) {
        case '+': return l + r;
        case '-': return l - r;
        case '*': return l * r;
        case '/': return l / r;
        case '^': return pow(l, r);
        default: throw std::runtime_error("Unknown operator");
    }
}
std::unique_ptr<Expression> Binary::specify(char x, T val) {
    std::unique_ptr<Expression> new_left = left->specify(x, val);
    std::unique_ptr<Expression> new_right = right->specify(x, val);
    if (new_left)
        left = std::move(new_left);
    if (new_right)
        right = std::move(new_right);
    return nullptr;
}


//-------------//
//----Unary----//
//-------------//
Unary::Unary(char operation, std::unique_ptr<Expression> expression) 
    : op(operation), expr(std::move(expression)) {}
Unary::Unary(const Unary &other) {
    op = other.op;
    expr = std::make_unique<Expression>(*other.expr);
}
Unary& Unary::operator=(const Unary &other) {
    if (this == &other) return *this;
    op = other.op;
    expr = std::make_unique<Expression>(*other.expr);
    return *this;
}
T Unary::evaluate(std::map<char, T> &x) const {
    auto res = expr->evaluate(x);
    switch (op) {
        case 's': return sin(res);
        case 'c': return cos(res);
        case 'l': return log(res);
        case 'e': return exp(res);
        default: throw std::runtime_error("Unknown operator");
    }
}
std::unique_ptr<Expression> Unary::specify(char x, T val) {
    std::unique_ptr<Expression> new_expr = expr->specify(x, val);
    if (new_expr)
        expr = std::move(new_expr);
    return nullptr;
}

