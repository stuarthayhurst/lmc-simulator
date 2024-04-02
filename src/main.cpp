#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "assembler.hpp"
#include "simulator.hpp"

int main(int argc, char* argv[]) {
  //Default to 100 memory addresses, allow overriding memory size
  int memoryLength = 100;
  if (argc >= 3) {
    memoryLength = std::strtol(argv[2], nullptr, 10);
  }

  //Setup simulator with configured number of memory addresses
  int* memoryPtr = setupSimulator(memoryLength);
  if (memoryPtr == nullptr) {
    std::cerr << "ERROR: Failed to allocate memory" << std::endl;
    return EXIT_FAILURE;
  }

  std::string filePath;
  if (argc >= 2) {
    filePath = std::string(argv[1]);
  } else {
    std::cerr << "ERROR: No input file specified" << std::endl;
    destroySimulator();
    return EXIT_FAILURE;
  }

  if (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h") {
    std::cout << "Usage: simulator [FILE] [MEMORY SIZE]" << std::endl;
    std::cout << " - [MEMORY SIZE] is optional" << std::endl;
    destroySimulator();
    return EXIT_SUCCESS;
  }

  //Read assembly file in
  std::vector<std::string> fileData;
  std::string lineData;
  std::ifstream input(filePath.c_str());
  if (input.is_open()) {
    while (std::getline(input, lineData)) {
      fileData.push_back(lineData);
    }
    input.close();
  } else {
    std::cerr << "ERROR: Input file doesn't exist" << std::endl;
    destroySimulator();
    return EXIT_FAILURE;
  }

  int inputLength = fileData.size();
  if (inputLength == 0) {
    std::cerr << "ERROR: Input file specified is empty" << std::endl;
    destroySimulator();
    return EXIT_FAILURE;
  }

  //Actually assemble the program, save the size
  int programLength = assembleProgram(memoryPtr, memoryLength, &fileData, inputLength);

  if (programLength == -1) {
    std::cerr << "ERROR: Failed to assemble '" << filePath << "'" << std::endl;
    destroySimulator();
    return EXIT_FAILURE;
  }

  //Set debug mode from environment
  bool debug = (std::getenv("DEBUG") != NULL) ?
               (std::getenv("DEBUG") == std::string("true")) : false;

  //Output 'machine code' if in debug mode
  if (debug) {
    printMemory(programLength);
  }

  //Run until encountering opcode 0 (HLT) or an error
  bool success = true, finished = false;
  while (success && !finished) {
    success = executeNextInstruction(&finished);
  }

  //Clean up and exit
  destroySimulator();
  return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
