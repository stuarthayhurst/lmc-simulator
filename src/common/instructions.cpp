#include <iostream>
#include <map>

#include "state.hpp"
#include "instructions.hpp"

namespace instructions {
  namespace {
    static int applyOverflow(int value) {
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
      std::cerr << "ERROR: Unknown opcode '" << 900 + operand << "'" << " at address " \
                << systemState->programCounter - 1 << std::endl;
      return -1;
    }

    return 0;
  }
}

namespace instructions {
  const std::map<int, InstructionHandler> opcodeHandlerMap = {
    {100, instructions::add},
    {200, instructions::subtract},
    {300, instructions::store},
    {500, instructions::load},
    {600, instructions::branchAlways},
    {700, instructions::branchZero},
    {800, instructions::branchPositive},
    {900, instructions::inputOutput}
  };

  const std::map<std::string, int> mnemonicOpcodeMap = {
    {"DAT", 000},
    {"HLT", 000},
    {"ADD", 100},
    {"SUB", 200},
    {"STA", 300},
    {"LDA", 500},
    {"BRA", 600},
    {"BRZ", 700},
    {"BRP", 800},
    {"INP", 901},
    {"OUT", 902}
  };

  const std::map<std::string, bool> mnemonicOperandMap = {
    {"DAT", true},
    {"HLT", false},
    {"ADD", true},
    {"SUB", true},
    {"STA", true},
    {"LDA", true},
    {"BRA", true},
    {"BRZ", true},
    {"BRP", true},
    {"INP", false},
    {"OUT", false}
  };
}
