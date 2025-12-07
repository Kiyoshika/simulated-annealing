#include "optimizer.h"

/**
 * The default acceptance probability function, exp(-(e' - e)/T)
 */
static double
default_acceptance_proba(double e_next_state,
                         double e_current_state,
                         double T)
{
    if (e_next_state < e_current_state)
        return 1.0;

    return exp((-1.0 * (e_next_state - e_current_state)) / T);
}

/**
 * The default temperature decay function, which decays the temperature by 5%
 */
static double
default_temperature_decay(double T)
{
    return T/1.05;
}

enum SA_Status
SA_Optimizer_init(struct SA_Optimizer* opt,
               double (*temperature_decay)(double T),
               void (*generate_neighbor)(const void* current_state, void* next_state),
               double (*acceptance_proba)(double e_next_state, double e_current_state, double T),
               double (*energy)(const void* state))
{
    if (!opt) return SA_STATUS_BAD_ARG;

    // default settings
    opt->verbose = false;
    opt->verbose_iterations = 0;
    opt->max_reheat_count = 0;
    opt->convergence_iterations = 100;

    if (!temperature_decay)
        opt->temperature_decay = &default_temperature_decay;
    else
        opt->temperature_decay = temperature_decay;

    if (!generate_neighbor) return SA_STATUS_BAD_ARG;
    opt->generate_neighbor = generate_neighbor;

    if (!acceptance_proba)
        opt->acceptance_proba = &default_acceptance_proba;
    else
        opt->acceptance_proba = acceptance_proba;

    if (!energy) return SA_STATUS_BAD_ARG;
    opt->energy = energy;

    return SA_STATUS_OK;
}

void
SA_Optimizer_set_verbose(struct SA_Optimizer* opt,
                      bool verbose,
                      size_t iterations)
{
   if (!opt) return;
   opt->verbose = verbose;
   opt->verbose_iterations = iterations;
}

void
SA_Optimizer_set_max_reheats(struct SA_Optimizer* opt,
                          size_t reheat_count)
{
    if (!opt) return;
    opt->max_reheat_count = reheat_count;
}

void
SA_Optimizer_set_convergence_iterations(struct SA_Optimizer* opt,
                                     size_t convergence_iterations)
{
    if (!opt) return;
    opt->convergence_iterations = convergence_iterations;
}

SA_BestState
SA_Optimizer_optimize(struct SA_Optimizer* opt,
                   double initial_T,
                   const void* initial_state,
                   size_t state_size_bytes)
{
    srand(time(NULL));

    SA_BestState best_state = (SA_BestState){
        .state = NULL,
        .energy = -1.0,
        .converged = false
    };

    if (!opt || !initial_state || state_size_bytes == 0)
    {
        return best_state;
    }

    opt->current_state = calloc(1, state_size_bytes);
    void* next_state = calloc(1, state_size_bytes);
    if (!opt->current_state || !next_state) return best_state;

    memcpy(opt->current_state, initial_state, state_size_bytes);
    best_state.state = opt->current_state;
    best_state.energy = opt->energy(initial_state);

    size_t total_reheats = 0;

reheat:
    opt->T = initial_T;
    size_t iter_no_improvement = 0;
    size_t total_iterations = 0;
    while (opt->T > 1e-6)
    {
        opt->generate_neighbor(opt->current_state, next_state);
        double e_current_state = opt->energy(opt->current_state);
        double e_next_state = opt->energy(next_state);
        bool is_improvement = e_next_state < e_current_state;
        double accept_proba = opt->acceptance_proba(e_next_state, e_current_state, opt->T);
        double rand_unif = (double)rand() / (double)RAND_MAX;
        if (rand_unif < accept_proba)
        {
            memcpy(opt->current_state, next_state, state_size_bytes);
            best_state.state = opt->current_state;
            best_state.energy = e_next_state;
            iter_no_improvement = 0;
        }

        // technically this check will 
        if (!is_improvement)
        {
            iter_no_improvement++;
            if (iter_no_improvement >= opt->convergence_iterations)
            {
                best_state.converged = true;
                free(next_state);
                return best_state;
            }
        }

        if (opt->verbose && total_iterations % opt->verbose_iterations == 0)
            printf("Temperature: %f, Energy: %f\n", opt->T, best_state.energy);

        opt->T = opt->temperature_decay(opt->T);
    }

    if (!best_state.converged && total_reheats < opt->max_reheat_count)
    {
        total_reheats++;
        goto reheat;
    }

    free(next_state);
    return best_state;
}

void
SA_Optimizer_free(struct SA_Optimizer* opt)
{
    if (!opt) return;
    free(opt->current_state);
    opt->current_state = NULL;
}
