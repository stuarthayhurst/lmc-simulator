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

  std::string filePath;
  if (argc >= 2) {
    filePath = std::string(argv[1]);
  } else {
    std::cerr << "ERROR: No input file specified" << std::endl;
    destroySimulator();
    return EXIT_FAILURE;
  }

  //Read assembly file in
  std::vector<std::string> fileData;
  std::string lineData;
  std::ifstream input(filePath.c_str());
  if (input.is_open()) {
    while (std::getline(input, lineData)) {
      fileData.push_back(lineData);
    }
  } else {
    std::cerr << "ERROR: Input file doesn't exist" << std::endl;
    destroySimulator();
    return EXIT_FAILURE;
  }

  int inputLength = fileData.size();
  if (fileData.size() == 0) {
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
  bool success = true;
  while (executeNextInstruction(&success));

  //Clean up and exit
  destroySimulator();
  return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
