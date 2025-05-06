#pragma once
#include <string>

namespace Utils {
    int toMinutes(const std::string& hhmm);
    std::string fromMinutes(int minutes);   
}
