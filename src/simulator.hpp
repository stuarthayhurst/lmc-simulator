#ifndef SIMULATOR
#define SIMULATOR

#include "common/state.hpp"

SystemState* createSimulator(int memoryLength);
void destroySimulator(SystemState* systemState);
void printMemory(SystemState* systemState, int size);
bool executeNextInstruction(SystemState* systemState, bool* finished);

#endif
