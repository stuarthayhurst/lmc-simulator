#include <iostream>

#include "common/state.hpp"
#include "common/instructions.hpp"

namespace {
  static int checkMemoryAddress(SystemState* systemState, int address) {
    if (address >= systemState->memoryLength) {
      std::cerr << "ERROR: Cannot access memory address '" << address << "'" << std::endl;
      return -1;
    }
    return 0;
  }
}

SystemState* createSimulator(int memoryLength) {
  //Create simulator
  SystemState* systemState = new (std::nothrow) SystemState;
  if (systemState == nullptr) {
    return nullptr;
  }

  //Initialise system state values
  systemState->accumulator = 0;
  systemState->programCounter = 0;
  systemState->memoryLength = memoryLength;

  //Create simulator memory
  systemState->memoryPtr = new (std::nothrow) int[systemState->memoryLength]{0};
  if (systemState->memoryPtr == nullptr) {
    return nullptr;
  }

  //Warn on non-100 memory sizes
  if (systemState->memoryLength != 100) {
    std::cerr << "WARNING: Memory set to non-standard value, behaviour may be altered" \
              << std::endl;
  }

  return systemState;
}

void destroySimulator(SystemState* systemState) {
  delete [] systemState->memoryPtr;
  delete systemState;
}

//Print n memory locations
void printMemory(SystemState* systemState, int size) {
  for (int i = 0; i < size; i++) {
    std::cout << systemState->memoryPtr[i] << std::endl;
  }
  std::cout << std::endl;
}

/*
 - Execute the next instruction in memory
 - Returns true if the instruction executed successfully
 - Writes true to finished if the program finished naturally (HLT instruction)
*/
bool executeNextInstruction(SystemState* systemState, bool* finished) {
    //Check memory address is within bounds
    if (checkMemoryAddress(systemState, systemState->programCounter) == -1) {
      //Invalid address, end program early
      *finished = false;
      return false;
    }

    //Get opcode and operand
    int operand = systemState->memoryPtr[systemState->programCounter] % 100;
    int opcode = systemState->memoryPtr[systemState->programCounter] - operand;

    //Increment the program counter
    systemState->programCounter++;

    //Stop execution if told to halt
    if (opcode == 0) {
      //Success, program ended
      *finished = true;
      return true;
    }

    //Check operand address is within bounds (exception for I/O, as it's not an address)
    if (opcode != 900 && checkMemoryAddress(systemState, operand) == -1) {
      //Invalid address, end program early
      std::cerr << "ERROR: Invalid operand '" << operand << "' at address " \
                << systemState->programCounter - 1 << std::endl;
      *finished = false;
      return false;
    }

    //Retreive and execute 'instruction'
    if (instructions::opcodeHandlerMap.contains(opcode)) {
      InstructionHandler handler = instructions::opcodeHandlerMap.at(opcode);
      if (handler(systemState, operand) == -1) {
        //Failed to execute, end program early
        *finished = false;
        return false;
      }
    } else {
      //Failed to execute, end program early
      std::cerr << "ERROR: Unknown opcode '" << opcode << "' at address " \
                << systemState->programCounter - 1 << std::endl;
      *finished = false;
      return false;
    }

  //Success, continue execution
  *finished = false;
  return true;
}
