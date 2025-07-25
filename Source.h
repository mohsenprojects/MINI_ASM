#pragma once
#include <string>

/**
 * @brief بررسی می‌کند آیا خط فقط شامل فضای خالی است یا نه
 * @param line خط ورودی
 * @return true اگر خط فقط فضای خالی باشد، false در غیر این صورت
 */
bool checkline_is_space(const std::string& line);

/**
 * @brief محتوای داخل پرانتز را استخراج می‌کند
 * @param pline خط ورودی
 * @return محتوای داخل پرانتز
 */
std::string check_prantez(const std::string& pline);

/**
 * @brief رشته را بر اساس کاراکتر مشخصی برش می‌دهد
 * @param cut کاراکتر برش
 * @param line خط ورودی
 * @param mode حالت برش (1: بعد از cut، 2: قبل از cut، دیگر موارد: حذف همه cutها)
 * @return رشته برش خورده
 */
std::string cut_string(char cut, const std::string& line, int mode = 0);