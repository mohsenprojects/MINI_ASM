#pragma once
#include <vector>
#include <string>
#include <map>
#ifndef SOURCE1_H  
#define SOURCE1_H  

enum class def_location { data, stack , heap};

struct int_variable_infos {
    std::string whole_value;
    bool need_calculate;
    int bitsize;
    std::vector<std::string> sum_parts;
    std::vector<char> oprations;
    def_location location;
    int stacklocation;
    bool have_var_val;
};

#endif // SOURCE1_H

std::vector<std::string> split_on_whitespace(const std::string& line);
std::string remove_space(const std::string& str);
std::vector<std::string> div_string_by_char(std::string input, char one, char two, char three, char four);
int eval_simple_expr(const std::string& expr, const std::map<std::string, int_variable_infos>& int_vars);