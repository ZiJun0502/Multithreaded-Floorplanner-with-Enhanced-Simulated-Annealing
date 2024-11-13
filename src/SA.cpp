#include "SA.h"

BSTree SimulatedAnnealing::run_all(bool outline_check, std::chrono::steady_clock::time_point starting_time) {
    BSTree all_best_state(this->state);
    BSTree base_state(this->state);
    double all_best_cost, epoch_best_cost, episode_cost, solution_cost, base_cost;
    base_cost = cost_function(&this->state, this->alpha);
    all_best_cost = epoch_best_cost = episode_cost = base_cost;

    auto iteration_start_time = std::chrono::steady_clock::now();
    auto iteration_end_time = std::chrono::steady_clock::now();
    std::chrono::duration<double> last_epoch_duration = iteration_end_time - iteration_start_time;
    auto ending_time = starting_time + std::chrono::seconds(290);
    int epoch = 0, episode = 0;

    while (iteration_start_time + 1.1*last_epoch_duration < ending_time) {
        BSTree solution = this->run(true);
        solution_cost = cost_function(&solution, this->alpha);
        if (solution_cost < all_best_cost) {
            all_best_state = solution;
            all_best_cost = solution_cost;
        }
        if (solution_cost < episode_cost) {
            episode_cost = solution_cost;
            logger.info("Improved, restart SA with new best state\n");
            this->set_state(solution);
        } else {
            logger.info("Not improving, restart SA with base state\n");
            this->set_state(base_state);
            epoch++;
            episode_cost = base_cost;
        }
        iteration_end_time = std::chrono::steady_clock::now();
        last_epoch_duration = (iteration_end_time - iteration_start_time);
        iteration_start_time = std::chrono::steady_clock::now();
        set_parameter(10000000.0, 5.0, 0.9999, 150000, calculateCost_1);
        logger.info("Epoch %d takes: %.3fs, best cost: %.2f, epoch cost: %.2f\n", epoch, last_epoch_duration.count(), all_best_cost, epoch_best_cost);
        std::chrono::duration<double> remaining_time = ending_time - iteration_start_time;
        logger.info("Time remining: %.0f\n", (remaining_time.count()));
    }
    return all_best_state;

}
BSTree SimulatedAnnealing::run(bool outline_check) {
    // Initialize the solution
    logger.debug("Copying state\n");
    double temperature = this->start_temperature;
    double current_cost = cost_function(&this->state, this->alpha);  // Initial cost calculation
    // double copy_cost = cost_function(&this->new_state, this->alpha);
    double best_cost = current_cost;
    logger.info("Initital cost: %f, width: %d, height: %d\n", current_cost, this->state.w, this->state.h);
    // printf("copy cost: %f, width: %d, height: %d\n", copy_cost, this->new_state->w, this->new_state->h);
    BSTree best_state(this->state);
    if (!best_state.isIdentical(best_state.root, this->state.root)) {
        throw std::runtime_error("Best Tree is NOT equal\n"); // Raise an exception
    }
    int iteration = 0;
    std::vector<int> perturb_cnt(3);
    // std::cout << "Original state\n";

    while (iteration < max_iterations) {
        // Perform a perturbation (rotate, swap, or move module)
        // std::cout << "Asserting state\n";
        if (this->state.isIdentical(this->state.root, this->new_state.root)) {
            // std::cout << "Tree is equal\n";
        } else {
            // std::cout << "Tree is NOT equal\n";
            throw std::runtime_error("Tree is NOT equal\n"); // Raise an exception
        }
        // printf("Pertubing\n");
        int temp = this->new_state.perturb();
        perturb_cnt[temp]++;
        // printf("Generating plan\n");

        // std::string filename = std::to_string(thread_id) + std::string(".txt");
        // new_state.print_memaddr(filename.c_str());
        this->new_state.generate_floorplan();
        
        // Calculate new cost
        double new_cost = cost_function(&this->new_state, this->alpha);
    
        // Decide whether to accept or reject the new solution
        bool accepted = (outline_check && accept(current_cost, new_cost, temperature, &this->new_state)) || 
                        (!outline_check && accept(current_cost, new_cost, temperature));
        if (accepted) {
            current_cost = new_cost;  // Accept new solution
            this->state = BSTree(this->new_state);
            if (new_cost < best_cost) {

                best_state = BSTree(this->new_state);
                best_cost = new_cost;
                // logger.info("New best found, cost: %f, at iteration: %d, temp: %.2f\n", best_cost, iteration, temperature);
                if (std::abs(best_cost - 0.0f) < 1e-9) {
                    break;
                }
            }
        } else {
            this->new_state = BSTree(this->state);
        }
    
        // Decrease temperature according to the cooling schedule
        temperature *= cooling_rate;
        temperature = std::max(temperature, end_temperature);

        // Increment iteration counter
        iteration++;

        // Optional: Print progress for debugging
        // logger.info("Iteration: %d, current cost: %.0f, best cost: %.0f, temp: %.2f\n", iteration, current_cost, best_cost, temperature);
        // logger.info("\tPerturbation: %d, New cost: %.0f, accepted: %d\n", temp, new_cost, accepted);
        // std::cout << "Iteration: " << iteration << ", Current Cost: " << current_cost << ", Temperature: " << temperature << std::endl;
    }
    // std::cout << "" << current_cost << std::endl;
    double state_best_cost = cost_function(&best_state, this->alpha);
    if (state_best_cost != best_cost) {
        printf("state best cost: %.0f, best cost: %.0f", state_best_cost, best_cost);
        throw std::runtime_error("State have inconsistent cost\n");
    }
    // logger.info("Final Cost: %.3f\n", best_cost);
    // logger.info("Floorplan width: %d, height: %d\n", best_state.w, best_state.h);
    // logger.info("Outline width: %d, height: %d\n", outline_w, outline_h);
    // logger.info("Rotate: %d, move: %d, swap: %d\n", perturb_cnt[0], perturb_cnt[1], perturb_cnt[2]);
    return best_state;
}
double SimulatedAnnealing::get_hpwl(BSTree *tree, bool verbose) {
    long long hpwl = 0;
    for (Net &net : nets) {
        // center coordinates
        int x_max = -1, x_min = INT_MAX, y_max = -1, y_min = INT_MAX;
        if (verbose)
            printf("Net: %s, degree: %d\n", net.name.c_str(), net.degree);
        for (int block_id : net.blk_conns) {
            Block &block =  tree->blocks[block_id];
            int x = (block.x + block.w + block.x) >> 1, 
                y = (block.y + block.h + block.y) >> 1;
            x_max = std::max(x_max, x);
            x_min = std::min(x_min, x);
            y_max = std::max(y_max, y);
            y_min = std::min(y_min, y);
            if (verbose) {
                printf("\tConn: block: %s, (x, y): (%d, %d)\n", block.name.c_str(), x, y);
            }
        }
        if (verbose)
            printf("Block finish\n");
        for (int term_id : net.term_conns) {
            Terminal &term = terms[term_id];
            int x = term.x,
                y = term.y;
            x_max = std::max(x_max, x);
            x_min = std::min(x_min, x);
            y_max = std::max(y_max, y);
            y_min = std::min(y_min, y);
            if (verbose)
                printf("\tConn: term: %s, (x, y): (%d, %d)\n", term.name.c_str(), x, y);
        }
        if (verbose) {
            printf("Term finish\n");
            printf("\tx_max: %d, x_min: %d, y_max: %d, y_min: %d\n", x_max, x_min, y_max, y_min);
            
        }
        // break;
        hpwl += ((x_max - x_min) + (y_max - y_min));
    }
    return hpwl;
}
/*============================================
SA Stage 0 cost: fit all block inside outline
============================================*/
double SimulatedAnnealing::calculateCost_0(BSTree *state, double alpha) {
    double cost_exceeding_area = state->get_area_outside_outline();
    logger.debug("Cost exceeding area: %f\n", cost_exceeding_area);
    return cost_exceeding_area;
}
/*============================================
SA Stage 1 cost: minimize objective function, 
reject solutions that exceed outline.
============================================*/
double SimulatedAnnealing::calculateCost_1(BSTree *state, double alpha) {
    double hpwl = get_hpwl(state), area = (double) state->w * state->h;
    // double aspect_ratio = (double) state->w / (double) state->h,
    //        target_aspect_ratio = (double) outline_w / (double) outline_h;
    double cost_area = alpha * area,
           cost_hpwl = (1 - alpha) * hpwl,
        //    cost_non_flat = -std::abs((double) state->w / state->h - 1),
           cost_exceeding_area = state->get_area_outside_outline();
    double total_cost = cost_area + cost_hpwl + 100.0f * cost_exceeding_area;
    // cost_non_flat *= 0.05 * total_cost;
    // total_cost += cost_non_flat;
        //    cost_hpwl = 0,
        //    cost_aspect = 2*((1 - alpha) / 2 * std::abs(aspect_ratio - target_aspect_ratio) * area);
        //    cost_aspect = 0;
    // logger.debug("Cost area: %f, hpwl: %f, exceed: %f\n", cost_area, cost_hpwl);
    // printf("Cost real: %f, total: %f, area: %f, hpwl: %f, exceed: %f, non_flat: %f\n", cost_area + cost_hpwl, total_cost, cost_area, cost_hpwl, cost_exceeding_area, cost_non_flat);
    return total_cost;
}
int SimulatedAnnealing::calculateReqCost(BSTree *state, double alpha) {
    // Calculate and return the cost of the tree (e.g., wirelength, area)
    // 𝛼𝐴 + (1 − 𝛼)
    long long hpwl = get_hpwl(state), area = (long long) state->w * state->h;

    return (int) (alpha * area + (1 - alpha) * hpwl);  // Placeholder
}

bool SimulatedAnnealing::accept(double old_cost, double new_cost, double temperature, BSTree *state) {
    if (state && state->get_area_outside_outline() != 0) {
        return false;
    }
    if (new_cost < old_cost) {
        return true;
    }
    double acceptance_probability = std::exp((old_cost - new_cost) / temperature);
    int random_number = intRand();
    // randoms.push_back(random_number);
    return random_number / double(RAND_MAX) < acceptance_probability;
}


std::vector<std::pair<double, double>> SimulatedAnnealing::generate_positive_transitions(int num_samples) {
    std::vector<std::pair<double, double>> v;
    int cnt = 0;
    for (int i = 0 ; i < num_samples ;) {
        // printf("Initialize tree\n");
        BSTree tree(outline_w, outline_h, blocks);
        cnt++;
        double cost = cost_function(&tree, alpha);
        tree.perturb();
        tree.generate_floorplan();
        double new_cost = cost_function(&tree, alpha);
        if (new_cost > cost) {
            v.push_back({new_cost, cost});
            i++;
        }
        // printf("Perturb: %d, Original cost: %f, new cost: %f\n", chosen, cost, new_cost);
    }
    printf("\nSampled %d tree\n", cnt);
    return v;
}
double SimulatedAnnealing::CHI_T(std::vector<std::pair<double, double>> &S, double T) {
    double sum_min = 0, sum_max = 0;
    double max_term_first = -std::numeric_limits<double>::infinity();
    double max_term_second = -std::numeric_limits<double>::infinity();

    for (const auto &s : S) {
        max_term_first = std::max(max_term_first, -s.first / T);
        max_term_second = std::max(max_term_second, -s.second / T);
    }
    for (const auto &s : S) {
        sum_max += std::exp((-s.first / T) - max_term_first);
        sum_min += std::exp((-s.second / T) - max_term_second);
    }
    double log_sum_max = std::log(sum_max) + max_term_first;
    double log_sum_min = std::log(sum_min) + max_term_second;

    // printf("sum max: %f, sum_min: %f\n", sum_max, sum_min);
    return log_sum_max - log_sum_min;
}

double SimulatedAnnealing::find_starting_temperature(std::function<long long(BSTree*, double alpha)> cost_function, double chi_0, int num_samples, double tolerance) {
    // Generate positive transitions
    std::vector<std::pair<double, double>> positive_transitions = generate_positive_transitions(num_samples);
    // double chi_Tn = CHI_T(positive_transitions, 1e18);
    // Initial temperature guess
    double Tn = 1e18;
    double Tn_next = Tn;
    double ln_chi_0 = std::log(chi_0), ln_chi_Tn;
    for (int i = 0 ; i < 100 ; i++) {
        ln_chi_Tn = CHI_T(positive_transitions, Tn);
        // Update temperature using the iterative formula:
        Tn_next = Tn * (ln_chi_Tn / ln_chi_0);
        
        // Check convergence
        if (std::abs(Tn_next - Tn) < tolerance) {
            break;
        }
        if (std::isnan(Tn)) {
            for (auto s : positive_transitions) {
                std::cout << s.first << ' ' << s.second << '\n';
            }
            Tn = 10000000.0;
            break;
            // throw std::runtime_error("ERROR, NaN temperature\n"); // Raise an exception
        }
        // printf("Tn: %f, Tn+1: %f, ln(chi_Tn): %f, ln(chi_0): %f\n", Tn, Tn_next, ln_chi_Tn, ln_chi_0);
        // getchar();
        Tn = Tn_next;
    }
    
    return Tn_next;
}
