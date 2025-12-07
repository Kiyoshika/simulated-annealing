#include "optimizer.h"
#include <stdio.h>
#include <time.h>

/**
 * This example program shows optimization of a multimodal function
 * using simulated annealing. Starting at the origin, it is very easy
 * for this to get trapped in a local minima, so we should expect our
 * algorithm to find the global minima (it is still trivial enough
 * for global optimization.)
 *
 * It uses the default acceptance probability and temperature decay.
 * Feel free to play around with these settings.
 */

typedef struct Point
{
    double x;
    double y;
} Point;

// utility to generate a uniform [-1, 1] random double for neighbor generation
double rand_unif()
{
    double coin_toss = (double)rand() / (double)RAND_MAX;
    double sample = (double)rand() / (double)RAND_MAX;
    return coin_toss > 0.5 ? sample : -1.0 * sample;
}

double energy(const void* state)
{
    double x = *(double*)state;
    
    // multimodal function with a local minimal exactly at the origin and global minima
    // around x=4.967
    return -1.0 * (
        (3.0 * exp(-1.0 * x * x)) + 
        (5.0 * exp((-1.0/2.0) * (x - 5.0) * (x - 5.0))) +
        exp(-4.0 * (x - 2.0) * (x - 2.0)));
}

// generate neighbor by moving X and Y in a very small random direction
void generate_neighbor(const void* current_state, void* next_state)
{
    const double* current = (const double*)current_state;
    double* next = (double*)next_state;

    *next = *current + rand_unif();

    // bind x to [-10, 10]
    if (*next < -10.0)
        *next = -10.0;

    if (*next > 10.0)
        *next = 10.0;
}

int main()
{
    // random seed for neighbor function
    srand(time(NULL));

    SA_Optimizer opt;

    enum SA_Status status = SA_Optimizer_init(
        &opt,
        NULL,  // default temperature decay
        &generate_neighbor, 
        NULL,  // default acceptance probability function
        &energy);

    if (status != SA_STATUS_OK)
    {
        fprintf(stderr, "Something failed when initializing optimizer.\n");
        return -1;
    }

    // verbose output, print every 100 iterations
    SA_Optimizer_set_verbose(&opt, true, 100);

    // reheat optimizer up to a maximum of 3 times
    SA_Optimizer_set_max_reheats(&opt, 3);

    // if no improvement after 20 iterations, mark it converged (returns early)
    SA_Optimizer_set_convergence_iterations(&opt, 25);

    // intial state, start at the origin (there is a local minima here)
    double initial_state = 0.0;

    status = SA_Optimizer_optimize(
        &opt, 
        100.0, // initial temperature T
        &initial_state, 
        sizeof(initial_state));

    if (status != SA_STATUS_OK)
    {
        fprintf(stderr, "SA_Optimizer had an issue during optimization.\n");
        return -1;
    }

    const double* best = opt.best_state.state;
    if (!best)
    {
        fprintf(stderr, "SA_Optimizer failed to run.\n");
        return -1;
    }

    printf("Best X: %f\n", *best);
    printf("Energy: %f\n", opt.best_state.energy);
    printf("Converged: %s\n", opt.best_state.converged ? "true" : "false");

    // cleanup
    SA_Optimizer_free(&opt);
    
    return 0;
}
