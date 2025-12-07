# Simulated Annealing
This is my own implementation of the [Simulated Annealing](https://en.wikipedia.org/wiki/Simulated_annealing) heuristic optimization algorithm for my own educational purposes.

This implementation allows you to provide a temperature decay (or default), acceptance probability (or default), neighbor generator and (obviously) the energy function to minimize.

This implementation also supports a "reheating" mechanism where, if the optimizer does not converge after reaching ~0 temperature, it will reset the temperature back to the initial temperature up to a specified number of times.

I use minimal heap allocations; the only heap usage is to allocate three re-usable buffers for the current state, best state and proposed next state each equal to the size of the given user data.

More information/documentation is provided in the [header file](include/optimizer.h).

## Example Usage
Examples are provided in the [examples](src/examples/) directory to see how to use it + some additional parameters to play with.

### Multivariate Optimization
Below is a screenshot from the example for multivariate optimization, finding the minimum of an elliptoid surface (the true minimum is `(X=3, Y=-5)`):

<img width="370" height="237" alt="image" src="https://github.com/user-attachments/assets/a41c78cc-3f1b-464c-b8ef-6e59e46df902" />

### Multimodal Optimization and Escaping Local Minima
Below is an example of optimizing a multimodal function starting at its local minima to show that it can escape and potentially find the global.

See below function being minimized (X is bound to `[-10, 10]`)

<img width="1426" height="662" alt="image" src="https://github.com/user-attachments/assets/d8e0f2ff-0896-4460-8f67-4131d252ca09" />


We start the optimization at `x=0.0` (almost the exact local minima) and would ideally find `x=4.96714`:
<img width="367" height="216" alt="image" src="https://github.com/user-attachments/assets/e2de60b8-dd5d-45ec-a5fb-912974668feb" />

## Building
This is a typical CMake project, so you must have `cmake` installed. The library, `simanneal` is compiled as a shared library. Build it as follows:
* `mkdir build && cd build`
* `cmake -DDEBUG=OFF -DEXAMPLES=ON ..`
    * `EXAMPLES` is a flag of whether or not to compile the [examples](src/examples/)
* `make`
