## lmc-simulator
  - Assemble and execute assembly of the Little Man Computer instruction set

## Requirements:
  - A compiler supporting `c++-20`
    - Override the default by setting the `CXX` environment variable, if required

## Building:
  - `make`: Builds the simulator
    - Supports multiple threads with `-j[THREAD COUNT]`
    - Use `make -j$(nproc)` to build with all available threads
  - `make clean`: Removes the compiled simulator, if present
  - `DEBUG=[true/false]`: Environment variable to enable debug symbols
  - `BUILD_DIR`: Environment variable to configure built object output

## Usage:
  - `./simulator [FILE] [MEMORY SIZE]`
    - For example, `./simulator programs/power.asm`
    - Or to limit the memory size to 50, `./simulator programs/power.asm 50`
  - Set `DEBUG="true"` as an environment variable, to enable debug output
    - Additionally includes debug symbols, retains the frame pointer and uses sanitisers
    - `DEBUG="true" ./simulator [FILE]`

## Sample programs:
  - Some sample programs are kept in `programs/`
    - `multiply.asm`: Multiply 2 inputs together
    - `power.asm`: Raise the first input to the power of the second
    - `fibonacci.asm`: Output the first `n` terms of the Fibonacci sequence
    - `iteration.asm`: Iterate over a group of addresses
    - `binary.asm`: Convert an integer from input into 8-bit binary
    - `leapyear.asm`: Check if a year from 1 to 999 was a leap year

## Contributing:
  - Feel free to submit pull requests!
  - Contributed code must fit the style of the codebase so far
  - Sample assembly is also welcome, as long as it's under `programs/`
