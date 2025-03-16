#include "Expression.hpp"

#include <iostream>

std::pair<std::string, T> get_var(std::string source) {
    size_t pos = source.find('=');
    std::string name = source.substr(0, pos);
    std::string strval = source.substr(pos + 1);
    if (!is_name(name))
        throw std::runtime_error(std::string("Invalid variable name: ") + name);
    if (!is_number(strval))
        throw std::runtime_error(std::string("Invalid variable value: ") + strval);
    T val = to_number<T>(strval);
    return {name, val};

}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: ./differentiator --eval \"expr\" x=.. y=..\n";
        std::cerr << "       ./differentiator --diff \"expr\" --by x\n";
        return 1;
    }

    std::string mode = argv[1];

    if (mode == "--eval") {
        std::unique_ptr<Expression> expr = Expression::create(argv[2]);
        std::map<std::string, T> vars;
        for (int i = 3; i < argc; ++i) {
            vars.insert(get_var(argv[i]));
        }
        T result = expr->evaluate(vars);
        std::cout << result << "\n";
    } else if (mode == "--diff") {
        if (std::string(argv[3]) != "--by") {
            std::cerr << "Unknown command: " << argv[3] << std::endl;
            return 1;
        }
        if (argc < 5) {
            std::cerr << "usage: ./differentiator --diff \"expr\" --by x\n";
            return 1;
        }
        std::unique_ptr<Expression> expr = Expression::create(argv[2]);
        std::unique_ptr<Expression> diff = expr->differentiate(argv[4]);
        simplify(diff);
        std::string result = diff->to_string();
        if (!result.empty() && result[0] == '(' && find_close(result.substr(1)) == result.length() - 2)
            result = result.substr(1, result.size() - 1);
        std::cout << result << "\n";
    } else {
        std::cerr << "Unknown mode: " << mode << std::endl;
        return 1;
    }
}