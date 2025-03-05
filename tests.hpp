#include "Expression.hpp"

#include <vector>

class Test {
public:
    std::string name;
    std::string expr;
    T x;
    T res;
    std::string type;
    ~Test() = default;
    Test(std::string, std::string, T, T, std::string);
    bool test() const;
    void show() const;
};

std::vector<Test> tests = {
    {"TEST1", "sin(x * 5) + ln(x ^ 2)", 5, 3.08652407477, "eval"},
    {"TEST2", "cos(x / 5) - exp(2 ^ x)", 0.5, -3.1182462135, "eval"},
    {"TEST3", "sin(x * cos(x * 2))", 2, 0.61824308331, "diff"},
    {"TEST4", "exp(ln(x^2))", 1, 2, "diff"},
    {"TEST5", "sin(x) * exp(y)", 0.5, 0.54030230586, "diff"}
};
