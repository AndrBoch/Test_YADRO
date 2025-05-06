#pragma once
#include <string>

enum class EventType {
    ClientArrived = 1,
    ClientSeated = 2,
    ClientWaiting = 3,
    ClientLeft = 4,
    ForcedLeave = 11,
    SeatedFromQueue = 12,
    ErrorEvent = 13
};

class Event {
private:
    std::string time;
    EventType type;
    std::string clientName;
    int tableNumber = 0;
    std::string errorMessage;

public:
    // Конструктор
    Event(std::string time, EventType type, std::string clientName, int tableNumber = 0);

    // Геттеры
    std::string getTime() const { return time; }
    EventType getType() const { return type; }
    std::string getClientName() const { return clientName; }
    int getTableNumber() const { return tableNumber; }

};