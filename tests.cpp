#include "tests.hpp"

#include <iostream>

bool equal(T a, T b) {
    return abs(a - b) < 1e-4;
}

Test::Test(std::string name, std::string expr_, T x_, T res_, std::string type_)
    : name(name), expr(expr_), x(x_), res(res_), type(type_) {}

bool Test::test() const {
    T result = 0;
    if (type == "eval") result = Expression::create(expr)->evaluate({{'x', x}});
    else result = Expression::create(expr)->differentiate('x')->evaluate({{'x', x}});
    if (equal(result, res)) {
        std::cout << "OK\n";
        return true;
    }
    std::cout << "FAIL\n";
    return false;
}
void Test::show() const {
    std::unique_ptr<Expression> e = Expression::create(expr);
    std::unique_ptr<Expression> der = e->differentiate('x');
    T result = (type == "eval" ? e->evaluate({{'x', x}}) : der->evaluate({{'x', x}}));
    std::cout << name << ": " << "\n";
    if (type == "eval") std::cout << "    source expression: " << expr << "    via x = " << x << "\n";
    else                std::cout << "    source expression: (" << expr << ")'    via x = " << x << "\n";
    std::cout << "    converted expression: " << e->to_string() << "\n";
    if (type == "diff") std::cout << "    calculated derivative: " << der->to_string() << "\n";
    std::cout << "    expected result:   " << res << "\n";
    std::cout << "    calculated result: " << result << "\n    ";
    test();
}

void test_all() {
    bool all_passed = true;
    for (int i = 0; i < 5; ++i) {
        std::cout << tests[i].name << ": ";
        if (!tests[i].test()) all_passed = false;
    }
    if (all_passed) std::cout << "All tests passed\n";
}


int main() {
    std::cout << "'test all' to run all tests\n'test n' to show n-th test\n'exit' to exit test program\n";
    std::string line;
    while (getline(std::cin, line)) {
        size_t sep = line.find(' ');
        std::string command1 = line.substr(0, sep);
        if (command1 == "test") {
            std::string command2 = (sep == std::string::npos ? "" : line.substr(sep + 1));
            if (command2 == "all") {
                test_all();
                continue;
            }
            bool is_number = true;
            for (auto c : command2) {
                if (c < '0' || c > '9') {
                    is_number = false;
                    break;
                }
            }
            if (!is_number || command2 == "") {
                std::cout << "Invalid test index. Had to be either 'all' or a number.\n";
                continue;
            }
            int n = std::stoi(command2);
            if (n <= tests.size() && n >= 1)
                tests[n - 1].show();
            else
                std::cout << "No such test found\n";
        } else if (line == "exit") {
            break;
        } else {
            std::cout << "Unknown command\n";
        }
    }
}