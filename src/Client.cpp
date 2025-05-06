#include "Client.h"

Client::Client(const std::string& name)
    : name(name) {}

void Client::addEvent(const Event& event) {
    eventHistory.push_back(event);
}