#include <iostream>

struct SystemState {
  int* memoryPtr;
  int memoryLength;
  int accumulator = 0;
  int programCounter = 0;
};

namespace instructions {
  namespace {
    int applyOverflow(int value) {
      //If the value is above or below 999 or -999, wrap to other side of the interval
      if (value > 999) {
        value -= 999;
        value = -999 + (value - 1);
      } else if (value < -999) {
        value += 999;
        value = 999 + (value + 1);
      }

      return value;
    }
  }

  int add(SystemState* systemState, int operand) {
    systemState->accumulator += (systemState->memoryPtr)[operand];
    systemState->accumulator = applyOverflow(systemState->accumulator);
    return 0;
  }

  int subtract(SystemState* systemState, int operand) {
    systemState->accumulator -= (systemState->memoryPtr)[operand];
    systemState->accumulator = applyOverflow(systemState->accumulator);
    return 0;
  }

  int store(SystemState* systemState, int operand) {
    (systemState->memoryPtr)[operand] = systemState->accumulator;
    return 0;
  }

  int load(SystemState* systemState, int operand) {
    systemState->accumulator = (systemState->memoryPtr)[operand];
    return 0;
  }

  int branchAlways(SystemState* systemState, int operand) {
    systemState->programCounter = operand;
    return 0;
  }

  int branchZero(SystemState* systemState, int operand) {
    if (systemState->accumulator == 0) {
      systemState->programCounter = operand;
    }
    return 0;
  }

  int branchPositive(SystemState* systemState, int operand) {
    if (systemState->accumulator >= 0) {
      systemState->programCounter = operand;
    }
    return 0;
  }

  int inputOutput(SystemState* systemState, int operand) {
    if (operand == 1) { //Input
      std::cin >> systemState->accumulator;
    } else if (operand == 2) { //Output
      std::cout << systemState->accumulator << std::endl;
    } else {
      std::cerr << "ERROR: Unknown opcode '" << 900 + operand << "'" << std::endl;
      return -1;
    }
    return 0;
  }
}
