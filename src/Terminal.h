#ifndef TERMINAL_H
#define TERMINAL_H

#include <string>

class Terminal {
public:
    int id, x, y;
    std::string name;
    // Terminal(int id, int x, int y, std::string name);
    Terminal(int id, int x, int y, std::string name) : id(id), x(x), y(y), name(name) {}
};

#endif  // TERMINAL_H
