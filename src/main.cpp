#include <pthread.h>
#include <chrono> 
#include "InputReader.h"
#include "BSTree.h"
#include "SA.h"
#include "Logger.h"
std::chrono::steady_clock::time_point start, end;
void write_output_rpt(BSTree *tree, std::chrono::duration<double> elapsed_time);
void write_drawing_txt(BSTree *tree, std::string txt_name);
struct ThreadData {
    int id;
    BSTree tree;
    BSTree best_solution;
    pthread_mutex_t* mutex;
};
void* run_simulated_annealing(void* arg) {
    // Unpack the data
    ThreadData* data = (ThreadData*) arg;
    pthread_mutex_t* mutex = data->mutex;
    // set pthread to id map 
    pthread_t thread_id = pthread_self();
    pthread_mutex_lock(mutex); 
    thread_id_map[thread_id] = data->id;
    intRand();
    pthread_mutex_unlock(mutex); 
    // data->best_solution = new BSTree();
    BSTree tree = data->tree;
    logger.debug("Tree address: %d\n", &tree);
    BSTree solution;

    // Initialize SA instance
    SimulatedAnnealing sa(&tree, 0.5, true, data->id);
    logger.debug("real sa initialized\n");
    // Phase 1
    sa.set_parameter(10000.0, 5.0, 0.99, 100000, sa.calculateCost_0);
    printf("Starting phase1\n");
    solution = sa.run(false);
    // // Phase 2
    sa.set_state(solution);
    // sa.set_parameter(10000.0, 5.0, 0.99, 100000, sa.calculateCost_0);
    sa.set_parameter(10000000.0, 5.0, 0.9999, 150000, sa.calculateCost_1);
    // std::string filename = std::to_string(data->id) + std::string(".txt");
    // sa.state.print_memaddr(filename.c_str());
    // solution = sa.run(false);
    printf("Starting phase2\n");
    solution = sa.run_all(true, start);

    // Compare solution with the current best and update the best solution if necessary
    data->best_solution = solution; // Update best solution

    return NULL;
}

int main(int argc, char *argv[]) {
    start = std::chrono::steady_clock::now();
    // std::srand(static_cast<unsigned int>(std::time(0)));  // Seed the random number generator
    // std::srand(static_cast<unsigned int>(21));  // Seed the random number generator

    read_input(argc, argv);
    // for (Terminal &term : terms) {
    //     std::cout << (term.name) << '\n';
    // }
    BSTree tree(outline_w, outline_h, blocks);

    // write_drawing_txt(&tree, "init_draw.txt");

    // logger.setLogLevel(LogLevel::DEBUG);
    logger.info("Number of blocks: %d, number of terms: %d, number of nets: %d\n", num_blocks, num_terms, num_nets);
    logger.info("Outline width: %d, height: %d\n", outline_w, outline_h);
    logger.info("Initial floorplan width: %d, height: %d\n", tree.w, tree.h);
    /*==================================
    Run SA
    ==================================*/
    int num_threads = 8;
    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    BSTree* best_solution = nullptr;

    // Launch threads
    for (int i = 0; i < num_threads; ++i) {
        printf("Setting thread %d\n", i);
        thread_data[i].id = i;
        thread_data[i].tree = BSTree(outline_w, outline_h, blocks);
        thread_data[i].mutex = &mutex;
        printf("Thread %d set\n", i);
        if (pthread_create(&threads[i], NULL, run_simulated_annealing, (void*)&thread_data[i])) {
            fprintf(stderr, "Error creating thread %d\n", i);
            return 1;
        }
    }

    // Wait for all threads to complete
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }
    int best_cost = INT_MAX;
    for (int i = 0 ; i < num_threads ; i++) {
        int thread_best_cost = SimulatedAnnealing::calculateReqCost(&(thread_data[i].best_solution), alpha);
        logger.info("Thread %d cost: %d\n", i, thread_best_cost);
        if (best_cost > thread_best_cost) {
            best_cost = thread_best_cost;
            best_solution = &thread_data[i].best_solution;
        }
    }
    // Now best_solution holds the best solution found by any thread
    if (best_solution) {
        printf("Best solution: cost = %d, width = %d, height = %d\n", best_cost, best_solution->w, best_solution->h);
    }
    // SimulatedAnnealing sa(tree, 0.5, true);
    // sa.set_parameter(10000.0, 5.0, 0.99, 100000, sa.calculateCost_0);
    // BSTree *solution = sa.run(false);
    // logger.info("Phase 1 finished: solution width: %d, height: %d\n", solution->w, solution->h);
    // sa.set_state(solution);
    // sa.set_parameter(10000000.0, 5.0, 0.9999, 150000, sa.calculateCost_1);
    // solution = sa.run_all(true, start);
    // logger.info("Phase 2 finished\n");

    end = std::chrono::steady_clock::now();
    write_output_rpt(best_solution, end - start);
    write_drawing_txt(best_solution, "draw.txt");
    // printf("Total random generated: %d\n", randoms.size());
    // for (int i = 0 ; i < 100 ; i++) {
    //     printf("%d ", randoms[i]);
    // }
    // printf("\n");
    return 0;
}

void write_output_rpt(BSTree *tree, std::chrono::duration<double> elapsed_time) {
    logger.info("Writing output report to: %s\n", output_file);
    int cost = SimulatedAnnealing::calculateReqCost(tree, alpha);
    int hpwl = SimulatedAnnealing::get_hpwl(tree, false);

    int area = tree->w * tree->h;
    int width = tree->w, height = tree->h;
    std::ofstream out(output_file);
    out << cost << '\n';
    out << hpwl << '\n';
    out << area << '\n';
    out << width << ' ' << height << '\n';
    out << elapsed_time.count() << '\n';
    for (Block &blk : tree->blocks) {
        out << blk.name << ' ' << blk.x << ' ' << blk.y << ' ' << blk.x + blk.w << ' ' << blk.y + blk.h << '\n';
    }
    out.close();
}
void write_drawing_txt(BSTree *tree, std::string txt_name) {
    logger.info("Writing drawing txt file\n");
    std::ofstream out(txt_name);
    out << tree->w << ' ' << tree->h << '\n';
    out << outline_w << ' ' << outline_h << '\n';
    out << num_blocks << '\n';
    for (Block &blk : tree->blocks) {
        out << blk.name << ' ' << blk.x << ' ' << blk.y << ' ' << blk.x + blk.w << ' ' << blk.y + blk.h << '\n';
    }
}// 