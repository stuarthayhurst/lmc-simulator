#include <cstdlib>

#include "instructions.hpp"

typedef int (*instructionPtrType)(SystemState*, int);
std::map<int, instructionPtrType> opcodeFunctionMap = {
  {100, instructions::add},
  {200, instructions::subtract},
  {300, instructions::store},
  {500, instructions::load},
  {600, instructions::branchAlways},
  {700, instructions::branchZero},
  {800, instructions::branchPositive},
  {900, instructions::inputOutput}
};

namespace {
  int checkMemoryAddress(SystemState* systemState, int address) {
    if (address >= systemState->memoryLength) {
      std::cerr << "ERROR: Cannot access memory address '" << address << "'" << std::endl;
      return -1;
    }
    return 0;
  }

  //Empty system state
  SystemState systemState;
}

int* setupSimulator(int memoryLength) {
  //Create simulator memory
  systemState.memoryPtr = (int*)std::malloc(memoryLength * sizeof(int));
  systemState.memoryLength = memoryLength;

  //Warn on non-100 memory sizes
  if (systemState.memoryLength != 100) {
    std::cerr << "WARNING: Memory set to non-standard value, behaviour may be altered" << std::endl;
  }

  return systemState.memoryPtr;
}

void destroySimulator() {
  std::free(systemState.memoryPtr);
}
