#include "Expression.hpp"

#include <iostream>
#include <iomanip>

std::unique_ptr<Expression> Expression::create(T val) {
    return std::make_unique<Constant>(val);
}
std::unique_ptr<Expression> Expression::create(std::string source) {
    while (source.length() > 0) {
        if (source[0] == ' ') {
            source = source.substr(1);
            continue;
        } else if (source.back() == ' ') {
            source.pop_back();
            continue;
        } else if (source[0] == '(' && find_close(source.substr(1)) == source.length() - 2) {
            source = source.substr(1, source.length() - 1);
            continue;
        }
        break;
    }

    //---CONSTANT---//
    if (source.length() == 0)
        return std::make_unique<Constant>(0);
        //throw std::runtime_error("Empty expression");
    if (is_number(source))
        return std::make_unique<Constant>(to_number<T>(source));

    //---VARIABLE--//
    if (source.length() == 1) {
        char c = source[0];
        if constexpr (std::is_same_v<T, std::complex<long double>>) {
            if (c == 'i') return std::make_unique<Constant>(std::complex<long double>(0, 1));
        }
        if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'))
            return std::make_unique<Variable>(c);
        throw std::runtime_error(std::string("Invalid character for expression: ") + c);
    }

    //----UNARY----//
    for (auto op: UNARY_OPERATORS) {
        if (source.length() < op.second.length()) continue;
        bool op_found = true;
        for (int i = 0; i < op.second.length(); ++i)
            if (source[i] != op.second[i]) op_found = false;
        if (!op_found) continue;
        if (source.length() < op.second.length() + 2 || source[op.second.length()] != '(')
            throw std::runtime_error(std::string("Expected a '(...)' after '") + op.second + "': " + source);
        std::string new_source = source.substr(op.second.length() + 1);
        size_t closed = find_close(new_source);
        if (closed != new_source.length() - 1) break;
        new_source = new_source.substr(0, closed);
        return std::make_unique<Unary>(op.first, Expression::create(new_source));
    }

    //----BINARY----//
    size_t sep = find_operator(source);
    if (sep == std::string::npos)
        throw std::runtime_error(std::string("Invalid expression: ") + source);
    std::string l_source = source.substr(0, sep);
    std::string r_source = source.substr(sep + 1);
    return std::make_unique<Binary>(source[sep], Expression::create(l_source), Expression::create(r_source));
}


//--------------//
//---CONSTANT---//
//--------------//
Constant::Constant(T val) : value(val) {}
std::unique_ptr<Expression> Constant::clone() const {
    return std::make_unique<Constant>(value);
}
bool Constant::has_var(char x) const {
    return false;
}
T Constant::evaluate(const std::map<char, T> &x) const {
    return value;
}
std::unique_ptr<Expression> Constant::differentiate(char x) const {
    return std::make_unique<Constant>(0);
}
std::unique_ptr<Expression> Constant::specify(char x, T val) {
    return nullptr;
}
std::string Constant::to_string() const {
    return delete_zeros(std::to_string(value));
}
std::pair<std::unique_ptr<Expression>, int> Constant::simplify() {
    return {nullptr, -1};
}


//--------------//
//---VARIABLE---//
//--------------//
Variable::Variable(char x) : name(x) {}
std::unique_ptr<Expression> Variable::clone() const {
    return std::make_unique<Variable>(name);
}
bool Variable::has_var(char x) const {
    return x == name || x == ' ';
}
T Variable::evaluate(const std::map<char, T> &x) const {
    auto it = x.find(name);
    return (it != x.end()) ? it->second : T{};
}
std::unique_ptr<Expression> Variable::differentiate(char x) const {
    return std::make_unique<Constant>(x == name ? 1 : 0);
}
std::unique_ptr<Expression> Variable::specify(char x, T val) {
    if (x == name)
        return std::make_unique<Constant>(val);
    return nullptr;
}
std::string Variable::to_string() const {
    return {name};
}
std::pair<std::unique_ptr<Expression>, int> Variable::simplify() {
    return {nullptr, 0};
}



//--------------//
//----BINARY----//
//--------------//
Binary::Binary(char operation, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
    : op(operation), left(std::move(l)), right(std::move(r)) {}
Binary::Binary(const Binary &other) {
    op = other.op;
    left = std::move(other.left->clone());
    right = std::move(other.right->clone());
}
Binary& Binary::operator=(const Binary &other) {
    if (this == &other) return *this;
    op = other.op;
    left = std::move(other.left->clone());
    right = std::move(other.right->clone());
    return *this;
}
std::unique_ptr<Expression> Binary::clone() const {
    return std::make_unique<Binary>(op, left->clone(), right->clone());
}
bool Binary::has_var(char x) const {
    return left->has_var(x) || right->has_var(x);
}
T Binary::evaluate(const std::map<char, T> &x) const {
    T l = left->evaluate(x);
    T r = right->evaluate(x);
    switch (op) {
        case '+': return l + r;
        case '-': return l - r;
        case '*': return l * r;
        case '/': return l / r;
        case '^': return pow(l, r);
        default: throw std::runtime_error(std::string("Unknown operator: ") + op);
    }
}
std::unique_ptr<Expression> Binary::differentiate(char x) const {
    std::unique_ptr<Expression> l = left->differentiate(x);
    std::unique_ptr<Expression> r = right->differentiate(x);
    switch (op) {
        case '+': case '-': return std::make_unique<Binary>(op, std::move(l), std::move(r));
        case '*': return
            std::make_unique<Binary>('+',
                std::make_unique<Binary>('*', std::move(l), right->clone()),
                std::make_unique<Binary>('*', left->clone(), std::move(r)));
        case '/': return
            std::make_unique<Binary>('/',
                std::make_unique<Binary>('-',
                    std::make_unique<Binary>('*', std::move(l), right->clone()),
                    std::make_unique<Binary>('*', left->clone(), std::move(r))),
                std::make_unique<Binary>('^', right->clone(), std::make_unique<Constant>(2)));
        case '^': return
            (std::make_unique<Unary>('e',
                std::make_unique<Binary>('*',
                    right->clone(),
                    std::make_unique<Unary>('l', left->clone())))
            )->differentiate(x);
        default: throw std::runtime_error(std::string("Unknown operator: ") + op);
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
std::string Binary::to_string() const {
    std::string l = left->to_string(), r = right->to_string();
    if (op == '^') return l + op + r;
    if (op == '+') {
        if (l.size() > 1 && l[0] == '(' && l.back() == ')') {
            l.pop_back();
            l = l.substr(1);
        }
    }
    if (op == '-' && l == "0") return "(-" + r + ")";
    return '(' + l + ' ' + op + ' ' + r + ')';
}
std::pair<std::unique_ptr<Expression>, int> Binary::simplify() {
    auto [new_left, left_type] = left->simplify();
    auto [new_right, right_type] = right->simplify();
    if (new_left)
        left = std::move(new_left);
    if (new_right)
        right = std::move(new_right);
    T left_val = (left_type == -1 ? left->evaluate() : -2);
    T right_val = (right_type == -1 ? right->evaluate() : -2);

    if (left_type == -1 && right_type == -1) // в выражении нет переменных, можно вычислить
        return {std::make_unique<Constant>(evaluate()), -1};

    switch (op) {
        case '+':
            if (left_val == 0)
                return {std::move(right), right_type};
            if (right_val == 0)
                return {std::move(left), left_type};
            break;
        case '-':
            if (left_val == 0)
                return {nullptr, 1};
            if (right_val == 0)
                return {std::move(left), left_type};
            break;
        case '*':
            if ((left_val == 0 || right_val == 0))
                return {std::make_unique<Constant>(0), -1};
            if (left_val == 1)
                return {std::move(right), right_type};
            if (right_val == 1)
                return {std::move(left), left_type};
            break;
        case '/':
            if (left_val == 0)
                return {std::make_unique<Constant>(0), -1};
            if (right_val == 1)
                return {std::move(left), left_type};
        case '^':
            if (left_val == 0)
                return {std::make_unique<Constant>(0), -1};
            if (left_val == 1 || right_val == 0)
                return {std::make_unique<Constant>(1), -1};
            if (right_val == 1)
                return {std::move(left), left_type};
            break;
    }
    return {nullptr, 0};
}


//-------------//
//----Unary----//
//-------------//
Unary::Unary(char operation, std::unique_ptr<Expression> expression) 
    : op(operation), expr(std::move(expression)) {}
Unary::Unary(const Unary &other) {
    op = other.op;
    expr = std::move(other.expr->clone());
}
Unary& Unary::operator=(const Unary &other) {
    if (this == &other) return *this;
    op = other.op;
    expr = std::move(other.expr->clone());
    return *this;
}
std::unique_ptr<Expression> Unary::clone() const {
    return std::make_unique<Unary>(op, expr->clone());
}
bool Unary::has_var(char x) const {
    return expr->has_var(x);
}
T Unary::evaluate(const std::map<char, T> &x) const {
    auto res = expr->evaluate(x);
    switch (op) {
        case 's': return sin(res);
        case 'c': return cos(res);
        case 'l': return log(res);
        case 'e': return exp(res);
        default: throw std::runtime_error(std::string("Unknown operator: ") + op);
    }
}
std::unique_ptr<Expression> Unary::differentiate(char x) const {
    std::unique_ptr<Expression> derivative = expr->differentiate(x);
    switch (op) {
        case 's': return
            std::make_unique<Binary>('*',
                std::make_unique<Unary>('c', expr->clone()),
                std::move(derivative));
        case 'c': return
            std::make_unique<Binary>('*',
                std::make_unique<Binary>('-',
                    std::make_unique<Constant>(0),
                    std::make_unique<Unary>('s', expr->clone())),
                std::move(derivative));
        case 'l': return
            std::make_unique<Binary>('/',
                std::move(derivative),
                expr->clone());
        case 'e': return
            std::make_unique<Binary>('*',
                std::make_unique<Unary>('e', expr->clone()),
                std::move(derivative));
        default: throw std::runtime_error(std::string("Unknown operator: ") + op);
    }
}
std::unique_ptr<Expression> Unary::specify(char x, T val) {
    std::unique_ptr<Expression> new_expr = expr->specify(x, val);
    if (new_expr)
        expr = std::move(new_expr);
    return nullptr;
}
std::string Unary::to_string() const {
    std::string operation = "";
    switch (op) {
        case 's': operation = "sin"; break;
        case 'c': operation = "cos"; break;
        case 'l': operation = "ln"; break;
        case 'e': operation = "exp"; break;
    }
    std::string e = expr->to_string();
    if (e.size() > 1 && e[0] == '(' && e.back() == ')') {
        e.pop_back();
        e = e.substr(1);
    }
    return operation + '(' + e + ')';
}
std::pair<std::unique_ptr<Expression>, int> Unary::simplify() {
    auto [new_expr, type] = expr->simplify();
    if (new_expr)
        expr = std::move(new_expr);
    if (type == -1)
        return {std::make_unique<Constant>(evaluate()), -1};
    return {nullptr, 1};
}


//-------------//
//----OTHER----//
//-------------//
bool is_number(std::string source) {
    if (source.length() == 0) return false;
    int dot_count = 0;
    bool im = (source.back() == 'i');
    if (im) source.pop_back();
    for (char c : source) {
        if (!(('0' <= c && c <= '9') || c == '.' || c == ',')) return false;
        if (c == '.' || c == ',') ++dot_count;
    }
    return dot_count <= 1;
}
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
size_t find_close(std::string source) {
    int cnt = 1;
    for (size_t i = 0; i < source.length(); ++i) {
        if (source[i] == '(') ++cnt;
        if (source[i] == ')') --cnt;
        if (cnt == 0) return i;
    }
    return std::string::npos;
}
size_t find_operator(std::string source) {
    size_t f = 0, i = std::string::npos, j = std::string::npos, k = std::string::npos;
    while (f < source.length()) {
        if (source[f] == '+' || source[f] == '-') i = f;
        if (source[f] == '*' || source[f] == '/') j = f;
        if (source[f] == '^') k = f;
        if (source[f] == '(') {
            f += find_close(source.substr(f + 1)) + 1;
            if (f == std::string::npos)
                throw std::runtime_error(std::string("Expected a ')': ") + source.substr(f));
        }
        ++f;
    }
    return (i == std::string::npos ? (j == std::string::npos ? k : j) : i);
}
std::string delete_zeros(std::string s) {
    int min_sz = 1 + (!s.empty() && s[0] == '-');
    while (s.length() > min_sz && s.back() == '0')
        s.pop_back();
    if (s.back() == '.')
        s.pop_back();
    return s;
}

namespace std {
    std::string to_string(std::complex<long double> val) {
        std::string real = delete_zeros(std::to_string(val.real()));
        std::string imag = delete_zeros(std::to_string(abs(val.imag())));
        if (val.imag() == 0) {
            if (real[0] == '-') return '(' + real + ')';
            return real;
        }
        if (val.real() == 0) {
            if (val.imag() < 0) return "(-" + (val.imag() == 1 ? "" : imag) + "i)";
            return (val.imag() == 1 ? "" : imag) + 'i';
        }
        char op = (val.imag() < 0 ? '-' : '+');
        return '(' + real + ' ' + op + ' ' + (val.imag() == 1 ? "" : imag) + "i)";
    }
}


/*
int main() {
    std::string s = "ln(x^2)";
    std::cout << s << "\n";
    std::unique_ptr<Expression> k = Expression::create(s);
    std::cout << "expr: " << k->to_string() << "\n";
    std::cout << "diff: " << k->differentiate('x')->to_string() << "\n";
}
*/