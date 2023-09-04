## lmc-simulator
  - Assemble and execute assembly of the Little Man Computer instruction set

## Requirements:
  - A compiler supporting `c++-20`
  - Override the default by setting the `CXX` environment variable

## Building:
  - `make`: Builds the simulator
  - `make clean`: Removes the compiled simulator, if present
  - `DEBUG=[true/false]`: Environment variable to enable debug symbols
  - `FAST=[true/false]`: Environment variable to enable more optimisation flags

## Usage:
  - `./simulator [FILE]`
   - For example, `./simulator programs/power.asm`
  - Set `DEBUG="true"` as an environment variable, to enable debug output
    - `DEBUG="true" ./simulator [FILE]`

## Sample programs:
  - Some sample programs are kept in `programs/`
    - `multiply.asm`: Multiply 2 inputs together
    - `power.asm`: Raise the first input to the power of the second
    - `fibonacci.asm`: Output the first `n` terms of the Fibonacci sequence
    - `iteration.asm`: Iterate over a group of addresses

## Contributing:
  - Feel free to submit pull requests!
  - Contributed code must fit the style of the codebase so far
  - Sample assembly is also welcome, as long as it's under `programs/`
