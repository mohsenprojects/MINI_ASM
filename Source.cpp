#include "Source.h"
#include <cctype>

bool checkline_is_space(const std::string& line) {
    for (char chr : line) {
        if (!isspace(static_cast<unsigned char>(chr))) {
            return false;
        }
    }
    return true;
}

std::string check_prantez(const std::string& pline) {
    bool checkprantez = false;
    std::string out;

    for (char c : pline) {
        if (c == '(' && !checkprantez) {
            checkprantez = true;
            continue;
        }

        if (c == ')' && checkprantez) {
            break;
        }

        if (checkprantez) {
            out += c;
        }
    }

    return out;
}

std::string cut_string(char cut, const std::string& line, int mode) {
    std::string result;
    bool found = false;
    result.reserve(line.size()); // بهینه‌سازی تخصیص حافظه

    for (char c : line) {
        if (mode == 1) { // حالت حفظ بعد از اولین برخورد (شامل خود cut)
            if (found || c == cut) {
                result += c;
                if (c == cut) found = true;
            }
        }
        else if (mode == 2) { // حالت حفظ قبل از اولین برخورد (بدون cut)
            if (c == cut) {
                found = true;
                break;
            }
            result += c;
        }
        else { // حالت پیش‌فرض: حذف همه موارد کاراکتر cut
            if (c != cut) {
                result += c;
            }
        }
    }

    return result;
}