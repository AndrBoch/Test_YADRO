#include <iostream>
#include "Event.h"
#include "Client.h"
#include "Table.h"
#include "Utils.h"
#include <fstream>
#include <string>
#include <sstream>
#include <regex>
#include <map>
#include <unordered_map>
#include <vector>
#include <algorithm> // для std::find_if
#include <queue>


using namespace std;

// Проверка: строка в формате HH:MM
bool isValidTime(const string& time) {
    return regex_match(time, regex(R"(^\d{2}:\d{2}$)"));
}

// Преобразование строки в int с проверкой
bool tryParseInt(const string& s, int& result) {
    try {
        size_t pos;
        result = stoi(s, &pos);
        return pos == s.length();
    } catch (...) {
        return false;
    }
}
int convertTimeToMinutes(const string& timeStr) {
    int hours, minutes;
    char colon;
    istringstream(timeStr) >> hours >> colon >> minutes;
    return hours * 60 + minutes; // Преобразуем время в минуты
}
bool isValidClientName(const string& name) {
    return regex_match(name, regex(R"(^[a-z0-9_-]+$)"));
}
int main(int argc, char* argv[]) {

    if (argc != 2) {
        cerr << "Usage: task <input_file>" << endl;
        return 1;
    }

    string filename = argv[1];
    ifstream fin(filename);

    if (!fin.is_open()) {
        cerr << "Ошибка: не удалось открыть файл " << filename << endl;
        return 1;
    }

    string line;
    int lineNumber = 1;
    // --- 1. Чтение количества столов ---
    getline(fin, line);
    int tableCount;
    if (!tryParseInt(line, tableCount) || tableCount <= 0) {
        cerr << line << endl;
        return 1;
    }
    ++lineNumber;

    // --- 2. Чтение времени начала и конца работы ---
    getline(fin, line);
    istringstream ss2(line);
    string openTime, closeTime;
    ss2 >> openTime >> closeTime;
    if (!isValidTime(openTime) || !isValidTime(closeTime)) {
        cerr << line << endl;
        return 1;
    }
    ++lineNumber;

    // --- 3. Чтение стоимости часа ---
    getline(fin, line);
    int hourlyRate;
    if (!tryParseInt(line, hourlyRate) || hourlyRate <= 0) {
        cerr << line << endl;
        return 1;
    }
    ++lineNumber;
  
    //////////////////////////////////////////////////////////////////////////////////////////////////
    map<string, Client> clients;
    vector<Event> events;
    unordered_map<int, string> tableOccupancy;
    deque<string> waitingQueue;
    vector<Table> tables;

    for (int i = 0; i < tableCount; ++i) {
        tables.emplace_back(i + 1);
    }

    while (getline(fin, line)) {
        ++lineNumber;
        if (line.empty()) {
            cerr << line << endl;
            return 1;
        }
           

        istringstream ss(line);
        string time, typeStr, name;
        int tableNumber = -1;
        

        ss >> time >> typeStr >> name;
        if (!isValidTime(time)) {
            cerr << line << endl;
            return 1;
        }
        if (!isValidClientName(name)) {
            cerr << line << endl;
            return 1;
        }

        int typeInt;
        if (!tryParseInt(typeStr, typeInt)) {
            cerr << line << endl;
            return 1;
        }
        if (typeInt < 1 || typeInt > 4) {
            cerr << line << endl;
            return 1;
        }
        EventType type = static_cast<EventType>(typeInt);

        if (clients.find(name) == clients.end()) {
            clients[name] = Client(name);
        }

        //clients[name].addEvent(event);

        // === Обработка событий ===
        Client& client = clients[name];

        // ID 1 - клиент пришел
        if (type == EventType::ClientArrived) {

            Event ArrivedEvent(time, type, name, tableNumber);
            events.push_back(ArrivedEvent);

            if (client.getIsInClub()) {
                Event Errorevent(time, EventType::ErrorEvent,"YouShallNotPass", tableNumber);
                events.push_back(Errorevent);
                continue;
            }
            else if (convertTimeToMinutes(time)<convertTimeToMinutes(openTime)) {
                Event Errorevent(time, EventType::ErrorEvent, "NotOpenYet", tableNumber);
                events.push_back(Errorevent);
                continue;
            }
            client.setIsInClub(true);
            client.setTableNumber(-1);
        }
        // ID 2 - клиент сел за стол 
        else if (type == EventType::ClientSeated) {
            string tableStr;
            if (!(ss >> tableStr) || !tryParseInt(tableStr, tableNumber) || tableNumber < 1 || tableNumber > tableCount) {
                cerr << line << endl;
                return 1;
            }
            
            Event SeatedEvent(time, type, name, tableNumber);
            events.push_back(SeatedEvent);

            if (!client.getIsInClub()) {
                Event Errorevent(time, EventType::ErrorEvent, "ClientUnknown", tableNumber);
                events.push_back(Errorevent);
                continue;
            }

            auto it = tableOccupancy.find(tableNumber);
            if (it != tableOccupancy.end() && it->second != name) {
                Event Errorevent(time, EventType::ErrorEvent, "PlaceIsBusy", tableNumber);
                events.push_back(Errorevent);
                continue;
            }

            // Освободим старый стол, если был
            if (client.getTableNumber() != -1 && client.getTableNumber() != tableNumber) {

                int seatedTable = client.getTableNumber();
                if (seatedTable != -1) {
                    tables[seatedTable - 1].addSession(Utils::toMinutes(client.getSeatedSince()), Utils::toMinutes(time), hourlyRate);
                }

                tableOccupancy.erase(client.getTableNumber());
            }

            client.setTableNumber(tableNumber);
            client.setSeatedSince(time);
            //tables[tableNumber - 1].setStartTime(Utils::toMinutes(time));
            tableOccupancy[tableNumber] = name;

            
        }
        // ID 3 - клиент ожидает
        else if (type==EventType::ClientWaiting) {

            Event WaitingEvent(time, type, name, tableNumber);
            events.push_back(WaitingEvent);

            if (tableOccupancy.size() < tableCount) {
                Event Errorevent(time, EventType::ErrorEvent, "ICanWaitNoLonger!", tableNumber);
                events.push_back(Errorevent);
                continue;
            }
            if (find(waitingQueue.begin(), waitingQueue.end(), name) == waitingQueue.end()) {
                if ((int)waitingQueue.size() >= tableCount) {
                    // Очередь переполнена — клиент уходит
                    Event leaveEvent(time, EventType::ForcedLeave, name, -1);
                    events.push_back(leaveEvent);
                    //client.addEvent(leaveEvent);
                    client.setIsInClub(false);
                } else {
                    waitingQueue.push_back(name); 
                }
                continue;
            }
            
        }
        // ID 4 - клиент ушел
        else if (type == EventType::ClientLeft) {

            Event LeftEvent(time, type, name, tableNumber);
            events.push_back(LeftEvent);

            if (!client.getIsInClub()) {
                Event Errorevent(time, EventType::ErrorEvent, "ClientUnknown", tableNumber);
                events.push_back(Errorevent);
                continue;
            }

            client.setIsInClub(false);
            int tabllv = -1;

            // Освободить стол, если сидел
            if (client.getTableNumber() != -1) {
                tableOccupancy.erase(client.getTableNumber());
                tabllv = client.getTableNumber();
                int seatedTable = client.getTableNumber();
                if (seatedTable != -1) {
                    tables[seatedTable - 1].addSession(Utils::toMinutes(client.getSeatedSince()), Utils::toMinutes(time), hourlyRate);
                }
                client.setTableNumber(-1);
            }
            if (tableOccupancy.size()<tableCount && !waitingQueue.empty() ){

                string nextClientName = waitingQueue.front(); 
                waitingQueue.pop_front();
                
                clients[nextClientName].setTableNumber(tabllv);
                clients[nextClientName].setSeatedSince(time);
                tableOccupancy[tabllv] = nextClientName;

                Event SEvent(time, EventType::ClientSeated, nextClientName, tabllv);
                events.push_back(SEvent);
                //clients[nextClientName].addEvent(SEvent);
            }
        }
    }
    ///////////////////////////////////////////////////////
    // Вывод: ID 11 - Клиент ушел после закрытия клуба
    vector<string> stillInClub;
    for (const auto& pair : clients) {
        const Client& client = pair.second;
        if (client.getIsInClub()) {
            stillInClub.push_back(client.getName());
        }
    }

    sort(stillInClub.begin(), stillInClub.end());

    for (const string& name : stillInClub) {
        Event leaveEvent(closeTime, EventType::ForcedLeave, name, -1);
        events.push_back(leaveEvent);

        const Client& client = clients[name];
        int seatedTable = client.getTableNumber();
        if (seatedTable != -1) {
            tables[seatedTable - 1].addSession(Utils::toMinutes(client.getSeatedSince()), Utils::toMinutes(closeTime), hourlyRate);
        }
    }
    ////////////////////////////////////////////////////////////////////////
    cout << openTime << endl;
    for (const Event& e : events) {
        cout << e.getTime() << " " << static_cast<int>(e.getType()) << " " << e.getClientName();
        if (e.getType() == EventType::ClientSeated) {
            cout << " " << e.getTableNumber();
        }
        cout << endl;
    }
    cout << closeTime << endl;
    for (const auto& table : tables) {
        cout << table.getId() << " "
                  << table.getRevenue() << " "
                  << Utils::fromMinutes(table.getOccupiedMinutes()) << "\n";
    }
    
    return 0;
}
