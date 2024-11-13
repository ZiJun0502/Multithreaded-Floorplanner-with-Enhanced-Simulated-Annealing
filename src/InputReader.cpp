#include "InputReader.h"

void read_input(int argc, char *argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " [α value] [input.block name] [input.net name] [output name]\n";
        return;
    }

    alpha = atof(argv[1]);  
    input_block = argv[2];
    input_net = argv[3];
    output_file = argv[4];

    std::ifstream block_file(input_block), nets_file(input_net);
    std::string temp;
    std::string block_name;
    std::string term_name, term_marker;

    block_file >> temp >> outline_w >> outline_h;  
    block_file >> temp >> num_blocks;  
    block_file >> temp >> num_terms;  

    int block_w, block_h, _id = 0;
    for (int i = 0 ; i < num_blocks ; i++) {
        block_file >> block_name >> block_w >> block_h;
        Block blk(_id, block_w, block_h, block_name);
        blocks.push_back(blk);
        name_to_blockID[block_name] = _id++;
    }

    int term_x, term_y;
    _id = 0;
    for (int i = 0; i < num_terms; ++i) {
        block_file >> term_name >> term_marker >> term_x >> term_y;
        Terminal term(_id, term_x, term_y, term_name);
        terms.push_back(term);
        name_to_termID[term_name] = _id++;
    }
    block_file.close();

    int num_nets, degree;
    nets_file >> temp >> num_nets;
    for (int i = 0; i < num_nets; ++i) {
        nets_file >> temp >> degree;
        std::string net_name = "n" + std::to_string(i + 1);
        std::vector<int> blk_conns;
        std::vector<int> term_conns;
        for (int j = 0; j < degree; ++j) {
            nets_file >> temp;
            if (name_to_blockID.count(temp)) {
                blk_conns.push_back(name_to_blockID[temp]);
            } else if (name_to_termID.count(temp)) {
                term_conns.push_back(name_to_termID[temp]);
            }
        }
        Net net(i, degree, net_name, blk_conns, term_conns);
        nets.push_back(net);
    }
    nets_file.close();
}
