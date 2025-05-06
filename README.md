## Компиляция MinGW-w64 (g++):
### Через g++ вручную:
  g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude -o task src/main.cpp src/Client.cpp src/Table.cpp src/Event.cpp src/Utils.cpp
  
## Запуск программы:
  .\task.exe .\tests\<input_test.txt>

## Структура проекта:
- `main.cpp` — основной файл.
- `Client.cpp`, `Client.h` — логика работы с клиентами.
- `Table.cpp`, `Table.h` — логика обработки игровых столов.
- `Event.cpp`, `Event.h` — структура и обработка событий.
- `Utils.cpp`, `Utils.h` — вспомогательные функции.
- `input.txt` — пример входных данных.

## Требования:
- Компилятор MinGW-w64 (g++)
- Поддержка C++17 и выше
