#ifndef BLOCK_H
#define BLOCK_H

#include <string>
#include <cstdlib>
#include <ctime>

class Block {
public:
    int id, w, h, x, y;
    long long area;
    std::string name;
    // Block(int id, int w, int h, std::string name);
    Block(int id, int w, int h, std::string name) : id(id), w(w), h(h), x(-1), y(-1), area(w*h), name(name) {}
    Block(const Block &other): id(other.id), w(other.w), h(other.h), x(other.x), y(other.y), 
          area(other.area), name(other.name) {}

    void rotate() {
        std::swap(this->w, this->h);
    }
};

#endif  // BLOCK_H
