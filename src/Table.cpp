#include "Table.h"
#include "Utils.h"

using namespace std;

Table::Table(int id) : id(id) {}

bool Table::occupied() const {
    return isOccupied;
}

int Table::getId() const {
    return id;
}

int Table::getRevenue() const {
    return revenue;
}

int Table::getOccupiedMinutes() const {
    return totalOccupiedMinutes;
}

void Table::addSession(int from, int to, int hourlyRate) {
    
    if ((to - from) % 60 != 0){
        revenue += ((to - from) / 60 + 1) * hourlyRate;
    }
    else {
        revenue += (to - from) / 60 * hourlyRate;
    }
    totalOccupiedMinutes += (to - from);
}