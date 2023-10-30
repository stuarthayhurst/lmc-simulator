#include <cstdlib>
#include <cstring>
#include <iostream>

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
  std::memset(systemState.memoryPtr, 0, memoryLength * sizeof(int));
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

//Print n memory locations
void printMemory(int size) {
  for (int i = 0; i < size; i++) {
    std::cout << systemState.memoryPtr[i] << std::endl;
  }
  std::cout << std::endl;
}

bool executeNextInstruction(bool* success) {
    //Check memory address is within bounds
    if (checkMemoryAddress(&systemState, systemState.programCounter) == -1) {
      *success = false;
      return false;
    }

    //Get opcode and operand
    int operand = systemState.memoryPtr[systemState.programCounter] % 100;
    int opcode = systemState.memoryPtr[systemState.programCounter] - operand;

    //Increment the program counter
    systemState.programCounter++;

    //Stop execution if told to halt
    if (opcode == 0) {
      //Success, program ended
      *success = true;
      return false;
    }

    //Check operand address is within bounds (exception for I/O, as it's not an address)
    if (opcode != 900 && checkMemoryAddress(&systemState, operand) == -1) {
      std::cout << "B" << std::endl;
      *success = true;
      return false;
    }

    //Retreive and execute 'instruction'
    if (opcodeFunctionMap.contains(opcode)) {
      instructionPtrType handler = opcodeFunctionMap[opcode];
      if (handler(&systemState, operand) == -1) {
        *success = false;
        return false;
      }
    } else {
      std::cerr << "ERROR: Unknown opcode '" << opcode << "'" << std::endl;
      *success = false;
      return false;
    }

  //Success, continue execution
  return true;
}
