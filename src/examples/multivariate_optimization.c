#include "optimizer.h"
#include <stdio.h>
#include <time.h>

/**
 * This example program shows optimization of a multivariate function
 * using simulated annealing.
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
    const Point* p = (const Point*)state;
    
    // elliptic paraboloid: x^2/a^2 + y^2/b^2
    // a = 5, b = 7
    //
    // true minimum (for reference) would be x = 3, y = -5
    return (((p->x - 3.0) * (p->x - 3.0)) / 25.0) + (((p->y + 5.0) * (p->y + 5.0)) / 49.0);
}

// generate neighbor by moving X and Y in a small random direction
void generate_neighbor(const void* current_state, void* next_state)
{
    const Point* current = (const Point*)current_state;
    Point* next = (Point*)next_state;

    next->x = current->x + rand_unif();
    next->y = current->y + rand_unif();
}

int main()
{
    // random seed for neighbor function
    srand(time(NULL));

    Optimizer opt;

    enum SA_Status status = Optimizer_init(
        &opt,
        10.0,  // initial temperature
        NULL,  // default temperature decay
        &generate_neighbor, 
        NULL,  // default acceptance probability function
        &energy);

    switch (status)
    {
        case SA_STATUS_BAD_ARG:
            fprintf(stderr, "Passed bad argument to optimizer.\n");
            return -1;
        case SA_STATUS_OK:
            break;
    }

    // intial state, start at the origin
    Point initial_state = (Point){
        .x = 0.0,
        .y = 0.0
    };

    BestState best_state = Optimizer_optimize(&opt, &initial_state, sizeof(initial_state));

    const Point* best = best_state.state;
    if (!best)
    {
        fprintf(stderr, "Optimizer failed to run.\n");
        return -1;
    }

    printf("Best X: %f\n", best->x);
    printf("Best Y: %f\n", best->y);
    printf("Energy: %f\n", best_state.energy);
    printf("Converged: %s\n", best_state.converged ? "true" : "false");

    // cleanup
    Optimizer_free(&opt);
    
    return 0;
}
