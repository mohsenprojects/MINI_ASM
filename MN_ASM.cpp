// in the name of god

#include <iostream>
#include <vector>
#include <string>
#include <map>

#include "Source.h"
#include "Source1.h"

using namespace std;

#define print(x) cout<<(x)<<endl;
#define printn(x) cout<<(x);
#define input(x) getline(cin, x)

int main() {

    //===============================================================||                        ||==========================================================//
    //====================================================||      all vectors need in compail time need      ||=================================================//
    //===============================================================||                        ||==========================================================//

    //code parts

    vector<string> code_lines;
    vector<string> segment_data_asm_output;
    vector<string> segment_bss_asm_output;
    vector<string> segment_text_asm_output;
    vector<string> _functions_asm_output;
    vector<string> externs_asm_output;
    vector<string> _loop_functions_asm_output;
    vector<string> _ife_functions_asm_output;

    vector<string> final_asm_output;

    segment_data_asm_output.push_back("segment .data");
    segment_bss_asm_output.push_back("segment .bss");
    segment_text_asm_output.push_back("segment .text");
    segment_text_asm_output.push_back("    global _main");
    segment_text_asm_output.push_back("_main:");

    // vaariable registers

    map<string, int_variable_infos> int_vars;

    //===============================================================||                        ||==========================================================//
    //=======================================================||      code line's input while loop      ||=======================================================//
    //===============================================================||                        ||==========================================================//

    while (true) {

        string __save__;

        printn(">>>");

        input(__save__);

        if (__save__ == "RUN" || __save__ == "run") {
            break;
        }

        code_lines.push_back(__save__);
    }

    //===============================================================||                        ||==========================================================//
    //==========================================================||      code line's check loop      ||==========================================================//
    //===============================================================||                        ||==========================================================//

    // the first code check loop

    for (string line : code_lines) {

        if (checkline_is_space(line)) {
            continue;
        }

        vector<string> line_tokens = split_on_whitespace(line);

        // ________________________ integer variables registers ________________________ 
        if (
            line_tokens.size() >= 2 &&
            (line_tokens[0] == "int8" || line_tokens[0] == "int16" || line_tokens[0] == "int32" || line_tokens[0] == "int64" || line_tokens[0] == "int")
            ) {
            string var_type;
            if (line_tokens[0] == "int8") var_type = "int8";
            else if (line_tokens[0] == "int16") var_type = "int16";
            else if (line_tokens[0] == "int32" || line_tokens[0] == "int") var_type = "int32";
            else if (line_tokens[0] == "int64") var_type = "int64";

            string var_name = "";

            if (line_tokens[1] != "stk" && line_tokens[1] != "stack" && line_tokens[1] != "dt" && line_tokens[1] != "data" && line_tokens[1] != "heap" && line_tokens[1] != "h") {
              
                for (char a : line_tokens[1]) {
                    if (a == '=') {
                        break;
                    }
                    else {
                        if (a != ' ') {
                            var_name += a; 
                        }
                    }
                }
            }
            else {
                for (char a : line_tokens[2]) {
                    if (a == '=') {
                        break;
                    }
                    else {
                        if (a != ' ') {
                            var_name += a;
                        }
                    }
                }
            }
            // پیدا کردن محل '='
            int eq_pos = 0;
            int cnt = 0;
            for (char c : line) {
                if (c == '=') {
                    eq_pos = cnt;
                    break;
                }
                else {
                    cnt++;
                }
            }
            
            // مقدار متغیر از بعد '=' تا انتها
            string var_value = "";
            for (int i = 0; i <= line.size(); i++ ) {
                if (i > eq_pos) {
                    var_value += line[i];
                }
            }
            var_value = remove_space(var_value);

            bool n_calc = false;
            for (char c : var_value) {
                if (c == '+' || c == '-' || c == '*' || c == '/') {
                    n_calc = true;
                    break;
                }
            }

            vector<string> opration_methods = div_string_by_char(var_value, '+', '-', '*', '/');

            int bitsize = 0;
            if (var_type == "int64") bitsize = 64;
            else if (var_type == "int32") bitsize = 32;
            else if (var_type == "int16") bitsize = 16;
            else if (var_type == "int8") bitsize = 8;

            def_location location = def_location::stack;
            if (line_tokens[2] == "stack" || line_tokens[2] == "stk") {
                location = def_location::stack;
            }
            else if (line_tokens[2] == "data" || line_tokens[2] == "d") {
                location = def_location::data;
            }
            else if (line_tokens[2] == "heap" || line_tokens[2] == "h") {
                location = def_location::heap;
            }

            //struct int_variable_infos {
             //   std::string whole_value;
             //   bool need_calculate;
             //   int bitsize;
             //   std::vector<std::string> sum_parts;
             //   opration parts
             //   def_location location;
             //   int stacklocation;
             //   bool have_var_val;
            //};

            vector<char>op_parts;
            for (char a : var_value) {
                if (a == '+' || a == '-' || a == '*' || a == '/') {
                    op_parts.push_back(a);
                }
            }

            int_vars[var_name] = {
                var_value,
                n_calc,
                bitsize,
                opration_methods,
                op_parts,
                location
            };

            // مقداردهی صحیح مقدار عددی متغیر (همیشه محاسبه شود)
            //int_vars[var_name].whole_value = to_string(eval_simple_expr(var_value, int_vars));
            //int_vars[var_name].need_calculate = false;
        }
    }

    int total_bits = 0;

    for (const auto& pair : int_vars) {
        const string& vn = pair.first;
        const int_variable_infos& v_i = pair.second;

        if (v_i.location == def_location::stack) {
            total_bits += v_i.bitsize;
        }
    }
    if(total_bits != 0) total_bits /= 8;

    string convertor = to_string(total_bits);
    
    if (total_bits != 0) {
        segment_text_asm_output.push_back("    sub rsp, " + convertor);
    }

    int stkcntr = 0;

    for (const auto& pair : int_vars) {

        bool ch = false;

        const int_variable_infos counter = pair.second;
        string name = pair.first;

        

        for (char c : counter.whole_value) {

            if (c != '1' && c != '2' && c != '3' && c != '4' && c != '5' && c != '6' && c != '7' && c != '8' && c != '9' && c != '0' && c != '+' && c != '-' && c != '/' && c != '*' && c != ' ') {

                ch = true;
                break;
            }
        }

        

        int_vars[name].have_var_val = ch;

    }

    for (const auto& pair : int_vars) {
        const int_variable_infos& counter = pair.second;
        string name = pair.first;

        if (counter.location == def_location::stack) {

            if (counter.bitsize == 64) {

                if (!counter.need_calculate) {
                    segment_text_asm_output.push_back("    mov qword ptr [rsp+" + to_string(stkcntr) + "], " + counter.whole_value);
                }
                else {
                    segment_text_asm_output.push_back("    mov qword ptr [rsp+" + to_string(stkcntr) + "], " + counter.sum_parts[0]);
                    int ccc = 0;
                    int ccs = 1;
                    for (string str : counter.sum_parts) {
                        if (counter.oprations[ccc] == '+') {
                            segment_text_asm_output.push_back("    mov rax, " + counter.sum_parts[ccs]);
                            segment_text_asm_output.push_back("    add qword ptr [rsp+" + to_string(stkcntr) + "], rax");
                            ccc++;
                            ccs++;
                        }
                        else if (counter.oprations[ccc] == '-') {
                            segment_text_asm_output.push_back("    mov rax, " + counter.sum_parts[ccs]);
                            segment_text_asm_output.push_back("    sub qword ptr [rsp+" + to_string(stkcntr) + "], rax");
                            ccc++;
                            ccs++;
                        }
                        else if (counter.oprations[ccc] == '*') {
                            segment_text_asm_output.push_back("    mov rax, " + counter.sum_parts[ccs]);
                            segment_text_asm_output.push_back("    imul qword ptr [rsp+" + to_string(stkcntr) + "], rax");
                            ccc++;
                            ccs++;
                        }
                        else if (counter.oprations[ccc] == '/') {
                            segment_text_asm_output.push_back("    mov rax, " + counter.sum_parts[ccs]);
                            segment_text_asm_output.push_back("    idiv qword ptr [rsp+" + to_string(stkcntr) + "], rax");
                            ccc++;
                            ccs++;
                        }

                    }
                }
                int_vars[name].stacklocation = stkcntr;
                stkcntr += 8;
            }
            else if (counter.bitsize == 32) {

                if (counter.need_calculate == false) {
                    segment_text_asm_output.push_back("    mov dword ptr [rsp+" + to_string(stkcntr) + "], " + counter.whole_value);
                }
                else {
                    segment_text_asm_output.push_back("    mov dword ptr [rsp+" + to_string(stkcntr) + "], 0");
                    for (string str : counter.sum_parts) {
                        segment_text_asm_output.push_back("    mov eax, " + str);
                        segment_text_asm_output.push_back("    add dword ptr[rsp+" + to_string(stkcntr) + "], eax");
                    }
                }
                int_vars[name].stacklocation = stkcntr;
                stkcntr += 4;
            }
            else if (counter.bitsize == 16) {

                if (counter.need_calculate == false) {
                    segment_text_asm_output.push_back("    mov word ptr [rsp+" + to_string(stkcntr) + "], " + counter.whole_value);
                }
                else {
                    segment_text_asm_output.push_back("    mov word ptr [rsp+" + to_string(stkcntr) + "], 0");
                    for (string str : counter.sum_parts) {
                        segment_text_asm_output.push_back("    mov ax, " + str);
                        segment_text_asm_output.push_back("    add word ptr[rsp+" + to_string(stkcntr) + "], ax");
                    }
                }
                int_vars[name].stacklocation = stkcntr;
                stkcntr += 2;
            }
            else if (counter.bitsize == 8) {

                if (counter.need_calculate == false) {
                    segment_text_asm_output.push_back("    mov byte ptr [rsp+" + to_string(stkcntr) + "], " + counter.whole_value);
                }
                else {
                    segment_text_asm_output.push_back("    mov byte ptr [rsp+" + to_string(stkcntr) + "], 0");
                    for (string str : counter.sum_parts) {
                        segment_text_asm_output.push_back("    mov al, " + str);
                        segment_text_asm_output.push_back("    add byte ptr[rsp+" + to_string(stkcntr) + "], al");
                    }
                }
                int_vars[name].stacklocation = stkcntr;
                stkcntr += 1;
            }
        }
        else if (counter.location == def_location::data) {
            if (counter.bitsize == 64 && counter.have_var_val == false) {
                segment_data_asm_output.push_back("    " + name + " dq " + counter.whole_value);
            }
            else if (counter.bitsize == 32 && counter.have_var_val == false) {
                segment_data_asm_output.push_back("    " + name + " dd " + counter.whole_value);
            }
            else if (counter.bitsize == 16 && counter.have_var_val == false) {
                segment_data_asm_output.push_back("    " + name + " dw " + counter.whole_value);
            }
            else if (counter.bitsize == 8 && counter.have_var_val == false) {
                segment_data_asm_output.push_back("    " + name + " db " + counter.whole_value);
            }
        }
    }

	//segment_text_asm_output.push_back("    ");
	segment_text_asm_output.push_back("    ret");

    for (string a : externs_asm_output) {
        final_asm_output.push_back(a);
    }
    for (string a : segment_data_asm_output) {
        final_asm_output.push_back(a);
    }
    for (string a : segment_bss_asm_output) {
        final_asm_output.push_back(a);
    }
    for (string a : segment_text_asm_output) {
        final_asm_output.push_back(a);
    }
    for (string a : _functions_asm_output) {
        final_asm_output.push_back(a);
    }
    for (string a : _loop_functions_asm_output) {
        final_asm_output.push_back(a);
    }
    for (string a : _ife_functions_asm_output) {
        final_asm_output.push_back(a);
    }

    for (string line : final_asm_output) {
        print(line);
    }

    print("\n\nprogram ended sucsfully .");
    return 0;
}