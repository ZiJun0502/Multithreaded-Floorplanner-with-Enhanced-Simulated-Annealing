#ifndef GLOBALS_H
#define GLOBALS_H

#include <vector>
#include <unordered_map>
#include <string>
#include <stdexcept> 
#include <random>
#include <time.h>
#include <thread>
#include <unistd.h>
#include <sys/types.h>

#include "Block.h"
#include "Terminal.h"
#include "Net.h"
#include "Logger.h"

extern double alpha;
extern int outline_w, outline_h;
extern int num_blocks, num_terms, num_nets;
// extern std::vector<int> randoms;
extern std::vector<Block> blocks;
extern std::vector<Terminal> terms;
extern std::vector<Net> nets;
extern std::unordered_map<std::string, int> name_to_blockID;
extern std::unordered_map<std::string, int> name_to_termID;
extern std::unordered_map<unsigned long, int> thread_id_map; 

extern const char* input_block;
extern const char* input_net;
extern const char* output_file;

extern Logger logger;  // Set the initial log level to INFO
int intRand(int min = 0, int max = 2147483647);

#endif
