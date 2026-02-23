#pragma once

#include <optional>
#include <sstream>
#include <string>

struct Date {
    unsigned int year, month, day;

    friend bool operator<(const Date &lhs, const Date &rhs) {
        return std::tie(lhs.year, lhs.month, lhs.day) < std::tie(rhs.year, rhs.month, rhs.day);
    }
};

inline bool isLeapYear(unsigned int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

inline unsigned int daysInMonth(unsigned int year, unsigned int month) {
    static const unsigned int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (month == 2 && isLeapYear(year)) return 29;

    return days[month - 1];
}

// YYYY-MM-DD
inline std::optional<Date> parseDate(const std::string &str) {
    Date d;
    char dash1, dash2;

    std::istringstream iss(str);
    if (!(iss >> d.year >> dash1 >> d.month >> dash2 >> d.day)) return std::nullopt;

    if (dash1 != '-' || dash2 != '-') return std::nullopt;

    if (d.month < 1 || d.month > 12) return std::nullopt;

    if (d.day < 1 || d.day > daysInMonth(d.year, d.month)) return std::nullopt;

    return d;
}
