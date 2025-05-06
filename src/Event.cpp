#include "Event.h"

using namespace std;

Event::Event(string time, EventType type, string clientName, int tableNumber)
    : time(move(time)), type(type), clientName(move(clientName)), tableNumber(tableNumber) {}
