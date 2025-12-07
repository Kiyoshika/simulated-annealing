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
               double initial_T,
               double (*temperature_decay)(double T),
               void (*generate_neighbor)(const void* current_state, void* next_state),
               double (*acceptance_proba)(double e_next_state, double e_current_state, double T),
               double (*energy)(const void* state));

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
                   const void* initial_state,
                   size_t state_size_bytes);

/**
 * Release the resources allocated from the optimizer (only if Optimizer_optimize was called)
 */
void
Optimizer_free(struct Optimizer* opt);

#endif
