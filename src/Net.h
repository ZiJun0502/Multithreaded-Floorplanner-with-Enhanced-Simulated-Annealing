#ifndef NET_H
#define NET_H

#include <string>

class Net {
public:
    int id, degree;
    std::string name;
    std::vector<int> blk_conns;
    std::vector<int> term_conns;
    // Net(int id, int degree, std::string name);
    Net(int id, int degree, std::string name, std::vector<int> blk_conns, std::vector<int> term_conns): 
        id(id), degree(degree), name(name), blk_conns(blk_conns), term_conns(term_conns) {}
};

#endif  // NET_H
