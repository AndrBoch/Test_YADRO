#pragma once
#include <string>

class Table {
private:
    int id;
    int revenue = 0;
    int totalOccupiedMinutes = 0;
    std::string occupiedSince;
    bool isOccupied = false;


public:
    Table(int id);
    bool occupied() const;
    int getId() const;
    int getRevenue() const;
    int getOccupiedMinutes() const;
    void addSession(int from, int to, int hourlyRate);
};