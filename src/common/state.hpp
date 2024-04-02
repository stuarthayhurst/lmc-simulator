#ifndef SYSTEMSTATE
#define SYSTEMSTATE

struct SystemState {
  int* memoryPtr;
  int memoryLength;
  int accumulator;
  int programCounter;
};

#endif
