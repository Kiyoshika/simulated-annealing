#ifndef SA_OPTIMIZER_H
#define SA_OPTIMIZER_H

#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "status.h"

typedef struct Optimizer {
    double (*temperature_decay)(double T);
    void (*generate_neighbor)(const void* current_state, void* next_state);
    double (*acceptance_proba)(double e_next_state, double e_current_state, double T);
    double (*energy)(const void* state);
    void* current_state;
    double T;
    bool verbose;
    size_t verbose_iterations;
    size_t max_reheat_count;
    size_t convergence_iterations;
} Optimizer;

typedef struct BestState {
    void* state;
    double energy;
    bool converged;
} BestState;

/**
 * Initialize optimizer with the initial temperature T and various functions:
 * - temperature_decay: Function to return the next value of the temperature given the current temperature. Pass NULL to use the default: T(t) = t/1.05
 * - generate_neighbor: A function that takes the current state and generates the next state (must write into new_state parameter assuming it has been allocated accordingly.)
 * - acceptance_proba: Probability acceptance function given the current state and temperature T. Pass NULL to use the default exp(-(e_new - e_current)/T).
 * - energy: The function to minimize; returns the "energy" of the current state.
 *
 * Optimizer is ran until temperature is <= 0.
 *
 * Returns:
 * - SA_STATUS_OK on success
 * - SA_STATUS_BAD_ARG if any invalid arguments were passed
 */
enum SA_Status
Optimizer_init(struct Optimizer* opt,
               double (*temperature_decay)(double T),
               void (*generate_neighbor)(const void* current_state, void* next_state),
               double (*acceptance_proba)(double e_next_state, double e_current_state, double T),
               double (*energy)(const void* state));

/**
 * Toggle verbose output of the optimizer.
 * If true, will print the current temperature and energy every [iterations]-th iteration.
 *
 * E.g., Optimizer_set_verbose(opt, true, 100); will print the energy and temperature every 100th iteration.
 */
void
Optimizer_set_verbose(struct Optimizer* opt,
                      bool verbose,
                      size_t iterations);

/**
 * Set the reheat count for the optimizer (default 0).
 * Reheating the optimizer means if the temperature reaches 0 but best_state.converged == false,
 * it will reset the temperature back to the initial temperature and keep optimizing until either
 * it "converges" or the max reheat count is reached.
 */
void
Optimizer_set_max_reheats(struct Optimizer* opt,
                          size_t reheat_count);

/**
 * Set the number of convergence iterations (default 100)
 * If optimizer has not improved energy in X iterations, label it as "converged"
 * If the optimizer has converged, it will return early.
 */
void
Optimizer_set_convergence_iterations(struct Optimizer* opt,
                                     size_t convergence_iterations);

/**
 * Start minimizing provided energy function and return the best state and energy found.
 * User provides a valid allocated pointer initial_state of size state_size_bytes.
 *
 * If allocation fails upfront to hold the initial_state, a null state is returned
 * in BestState struct with -1.0 energy.
 *
 * Returns:
 * - A BestState struct with the current best state found so far with the lowest energy. BestState.converged is true if the energy has not changed in the last 100 iterations; otherwise false.
 */
BestState
Optimizer_optimize(struct Optimizer* opt,
                   double initial_T,
                   const void* initial_state,
                   size_t state_size_bytes);

/**
 * Release the resources allocated from the optimizer (only if Optimizer_optimize was called)
 */
void
Optimizer_free(struct Optimizer* opt);

#endif
