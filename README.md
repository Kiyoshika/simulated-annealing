# Simulated Annealing
This is my own implementation of the [Simulated Annealing](https://en.wikipedia.org/wiki/Simulated_annealing) heuristic optimization algorithm for my own educational purposes.

This implementation allows you to provide a temperature decay (or default), acceptance probability (or default), neighbor generator and (obviously) the energy function to minimize.

This implementation also supports a "reheating" mechanism where, if the optimizer does not converge after reaching ~0 temperature, it will reset the temperature back to the initial temperature up to a specified number of times.

More information/documentation is provided in the [header file](include/optimizer.h).

## Example Usage
Examples are provided in the [examples](src/examples/) directory to see how to use it + some additional parameters to play with.

Below is a screenshot from the example for multivariate optimization, finding the minimum of an elliptoid surface:

## Building
This is a typical CMake project, so you must have `cmake` installed. The library, `simanneal` is compiled as a shared library. Build it as follows:
* `mkdir build && cd build`
* `cmake -DDEBUG=OFF -DEXAMPLES=ON ..`
    * `EXAMPLES` is a flag of whether or not to compile the [examples](src/examples/)
