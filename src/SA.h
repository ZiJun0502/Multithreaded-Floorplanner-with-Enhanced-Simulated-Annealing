#ifndef SA_H
#define SA_H

#include <cmath>
#include <chrono>
#include <cstdlib>
#include <functional>

#include "Globals.h"
#include "BSTree.h"

class SimulatedAnnealing {
private:
    double start_temperature;
    double end_temperature;
    double cooling_rate;
    double alpha;
    int max_iterations;
    bool auto_select_parameters;
    std::function<long long(BSTree*, double)> cost_function;
public:
    int thread_id;
    BSTree state;
    BSTree new_state;
    SimulatedAnnealing(BSTree *tree, double alpha, bool auto_select_parameters, int thread_id):
        start_temperature(10000000.0),
        end_temperature(5.0),
        cooling_rate(0.9999),
        alpha(alpha),
        max_iterations(150000),
        thread_id(thread_id),
        auto_select_parameters(auto_select_parameters) {
            logger.debug("initializing SA\n");
            this->state = BSTree(*tree);
            this->new_state = BSTree(*tree);
        };
    ~SimulatedAnnealing() {}
    // Function to run simulated annealing
    BSTree run(bool outline_check=false);
    BSTree run_episode(bool outline_check, std::chrono::steady_clock::time_point starting_time);
    BSTree run_all(bool outline_check, std::chrono::steady_clock::time_point starting_time);
    void set_parameter(double start_temperature, double end_temperature, double cooling_rate, int max_iterations, std::function<long long(BSTree*, double alpha)> cost_function) {
        this->start_temperature = start_temperature;
        this->end_temperature = end_temperature;
        this->cooling_rate = cooling_rate;
        this->max_iterations = max_iterations;
        this->cost_function = cost_function;
        
        if (auto_select_parameters) {
            this->start_temperature = this->find_starting_temperature(cost_function, 0.8, 20, 10.0);
        }
        printf("Initial temperature: %f\n", this->start_temperature);
    }
    void set_state(BSTree tree) { 
        this->state = tree; 
        this->new_state = tree; 
    }
    // Function to calculate cost
    static double get_hpwl(BSTree *tree, bool verbose=false);
    static double calculateCost_0(BSTree *state, double alpha);
    static double calculateCost_1(BSTree *state, double alpha);
    static int calculateReqCost(BSTree *state, double alpha);
    
    // Function to check if we accept new solution
    static bool accept(double old_cost, double new_cost, double temperature, BSTree *tree = nullptr);
    // Calculate good starting temerature
    std::vector<std::pair<double, double>> generate_positive_transitions(int num_samples);
    double CHI_T(std::vector<std::pair<double, double>> &S, double T);
    double find_starting_temperature(std::function<long long(BSTree*, double alpha)> costFunction, 
                                     double chi_0, int num_samples = 100, double tolerance = 1e-4);
    // static bool accept(double old_cost, double new_cost, double temperature);
};

#endif
