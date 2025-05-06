#pragma once
#include <string>
#include <vector>
#include "Event.h"

class Client {
private:
    std::string name;
    bool isInClub = false;
    int tableNumber = -1;
    std::string seatedSince = "00:00";
    std::vector<Event> eventHistory;
public:
    Client() = default;
    Client(const std::string& name);

    //Геттеры
    std::string getName() const { return name; }
    bool getIsInClub() const { return isInClub; }
    int getTableNumber() const { return tableNumber; }
    std::string getSeatedSince() const { return seatedSince; }
    const std::vector<Event>& getEventHistory() const { return eventHistory; }

    // Сеттеры
    void setIsInClub(bool value) { isInClub = value; }
    void setTableNumber(int table) { tableNumber = table; }
    void setSeatedSince(const std::string& time) { seatedSince = time; }


    void addEvent(const Event& event);
};