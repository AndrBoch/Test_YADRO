#include "Utils.h"
#include <sstream>
#include <iomanip>

using namespace std;

namespace Utils {

int toMinutes(const string& hhmm) {
    int hours, minutes;
    char colon;
    istringstream(hhmm) >> hours >> colon >> minutes;
    return hours * 60 + minutes;
}

string fromMinutes(int totalMinutes) {
    int hours = totalMinutes / 60;
    int minutes = totalMinutes % 60;
    ostringstream out;
    out << setw(2) << setfill('0') << hours << ":"
        << setw(2) << setfill('0') << minutes;
    return out.str();
}

}
