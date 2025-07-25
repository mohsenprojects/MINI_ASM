#include "Source1.h"
#include <cctype>
#include <algorithm>
#include <iterator>
#include <stack>
#include <sstream>

std::vector<std::string> split_on_whitespace(const std::string& line) {
    std::vector<std::string> tokens;
    std::string current_token;

    for (char c : line) {
        if (!isspace(c)) {  // اگر کاراکتر فضای خالی نبود، به توکن فعلی اضافه کن
            current_token += c;
        }
        else {  // اگر فضای خالی بود، توکن فعلی را ذخیره کن (اگر خالی نبود)
            if (!current_token.empty()) {
                tokens.push_back(current_token);
                current_token.clear();
            }
        }
    }

    // اگر بعد از پایان حلقه، توکنی باقی مانده بود، آن را اضافه کن
    if (!current_token.empty()) {
        tokens.push_back(current_token);
    }

    return tokens;
}

std::string remove_space(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    std::copy_if(str.begin(), str.end(), std::back_inserter(result),
        [](char c) { return !isspace(static_cast<unsigned char>(c)); });
    return result;
}

std::vector<std::string> div_string_by_char(std::string input, char one, char two, char three, char four) {
    std::string save_in = "";
    std::vector<std::string> _return;

    for (char c : input) {
        if (c == one || c == two || c == three || c == four) {
            if (!save_in.empty()) {
                _return.push_back(save_in);
                save_in = "";
            }
        }
        else {
            save_in += c;
        }
    }
    if (!save_in.empty()) {
        _return.push_back(save_in);
    }
    return _return;
}

int eval_simple_expr(const std::string& expr, const std::map<std::string, int_variable_infos>& int_vars) {
    std::istringstream iss(expr);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }

    std::stack<int> values;
    std::stack<char> ops;

    auto apply_op = [](int a, int b, char op) -> int {
        switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return a / b;
        }
        return 0;
        };

    auto get_value = [&](const std::string& t) -> int {
        if (isdigit(t[0]) || (t[0] == '-' && t.size() > 1)) {
            return std::stoi(t);
        }
        else if (int_vars.count(t)) {
            return std::stoi(int_vars.at(t).whole_value);
        }
        else {
            return 0;
        }
        };

    int idx = 0;
    values.push(get_value(tokens[idx++]));
    while (idx < tokens.size()) {
        char op = tokens[idx++][0];
        int val = get_value(tokens[idx++]);
        if (op == '*' || op == '/') {
            int prev = values.top(); values.pop();
            values.push(apply_op(prev, val, op));
        }
        else {
            ops.push(op);
            values.push(val);
        }
    }

    while (!ops.empty()) {
        int b = values.top(); values.pop();
        int a = values.top(); values.pop();
        char op2 = ops.top(); ops.pop();
        values.push(apply_op(a, b, op2));
    }

    return values.top();
}