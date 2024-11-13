#include "Globals.h"

double alpha = 0.0;
const char* input_block;
const char* input_net;
const char* output_file;
int outline_w, outline_h;
int num_blocks, num_terms, num_nets;
// std::vector<int> randoms;
std::vector<Block> blocks;
std::vector<Terminal> terms;
std::vector<Net> nets;
std::unordered_map<std::string, int> name_to_blockID;
std::unordered_map<std::string, int> name_to_termID;
Logger logger(LogLevel::INFO);  // Set the initial log level to INFO
std::unordered_map<unsigned long, int> thread_id_map; 
int intRand(int min, int max) {
    static __thread std::mt19937* generator = nullptr;
    if (!generator) {
        pthread_t thread_id = pthread_self();
        int id = thread_id_map[thread_id];
        int seed = 30 + id;
        logger.info("Thread %d: initializing random generator with seed: %d\n", id, seed);
        generator = new std::mt19937(seed);
    }
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(*generator);
}