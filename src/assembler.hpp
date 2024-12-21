#ifndef ASSEMBLER
#define ASSEMBLER

#include <string>
#include <vector>

#include "common/state.hpp"

int assembleProgram(SystemState* systemState, std::vector<std::string>* inputData,
                    int inputDataLength);

#endif
