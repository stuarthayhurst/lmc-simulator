#ifndef INSTRUCTIONS
#define INSTRUCTIONS

#include <map>

#include "state.hpp"

typedef int (*InstructionHandler)(SystemState*, int);

namespace instructions {
  int add(SystemState* systemState, int operand);
  int subtract(SystemState* systemState, int operand);
  int store(SystemState* systemState, int operand);
  int load(SystemState* systemState, int operand);
  int branchAlways(SystemState* systemState, int operand);
  int branchZero(SystemState* systemState, int operand);
  int branchPositive(SystemState* systemState, int operand);
  int inputOutput(SystemState* systemState, int operand);
}

namespace instructions {
  extern std::map<int, InstructionHandler> opcodeHandlerMap;
  extern std::map<std::string, int> mnemonicOpcodeMap;
  extern std::map<std::string, bool> mnemonicOperandMap;
}

#endif
