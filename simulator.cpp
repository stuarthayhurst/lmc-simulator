#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

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

std::map<std::string, int> mnemonicOpcodeMap = {
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

int assembleProgram(int memory[], int memoryLength, std::vector<std::string>* inputData, int inputDataLength) {
  //Create variable sized storage for tokens
  std::vector<std::vector<std::string>> codeVectors;

  //Split into code vectors
  for (int i = 0; i < inputDataLength; i++) {
    std::vector<std::string> codeVector;
    codeVector.push_back(std::to_string(i + 1));
    std::string inputLine = (*inputData)[i];

    int tokenIndex = 0;
    bool lastCharacterWasSpace = true;

    //Split the line of code into tokens
    int stringLength = inputLine.size();
    for (int letterIndex = 0; letterIndex < stringLength; letterIndex++) {
      //Create a new token on each character group
      if (inputLine[letterIndex] == ' ') {
        lastCharacterWasSpace = true;
      } else {
        if (lastCharacterWasSpace) {
          if (inputLine[letterIndex] == '#') {
            break;
          }
          tokenIndex++;
          codeVector.push_back(std::string(""));
          lastCharacterWasSpace = false;
        }

        //Add the character to the current token
        codeVector[tokenIndex] += inputLine[letterIndex];
      }
    }

    //Add the line to the other vectors if it's not empty
    if (tokenIndex != 0) {
      codeVectors.push_back(codeVector);
    }
  }

  //Get the new size of the program, after splitting into vectors
  int programLength = codeVectors.size();

  //Fail if memory won't be able to hold the program
  if (programLength > memoryLength) {
    std::cerr << "ERROR: Memory is not large enough to store program (" << programLength << " > " << memoryLength << ")" << std::endl;
    return -1;
  }

  //Save label addresses and remove
  std::map<std::string, int> labelIndexMap;
  for (int i = 0; i < programLength; i++) {
    std::vector<std::string> codeVector = codeVectors[i];

    //Save label index and remove label
    if (!mnemonicOpcodeMap.contains(codeVector[1])) {
      labelIndexMap[codeVector[1]] = i;
      codeVectors[i].erase(codeVectors[i].begin()++);
    }
  }

  //Convert opcodes, operands and labels into 'machine code'
  for (int i = 0; i < programLength; i++) {
    std::vector<std::string> codeVector = codeVectors[i];

    //Skip any empty lines due to labels (line numbers will remain)
    unsigned int tokenCount = codeVector.size();
    if (tokenCount == 1) {
      continue;
    }

    //Replace labels with addresses
    for (unsigned int tokenIndex = 1; tokenIndex < tokenCount; tokenIndex++) {
      if (labelIndexMap.contains(codeVector[tokenIndex])) {
        codeVector[tokenIndex] = std::to_string(labelIndexMap[codeVector[tokenIndex]]);
      }
    }

    //Check for unrecognised instructions
    if (!mnemonicOpcodeMap.contains(codeVector[1])) {
      std::cerr << "ERROR: Unrecognised instruction '" << codeVector[1] << "' on line " << codeVector[0] << std::endl;
      return -1;
    }

    //Convert mnemonic to an opcode, then retrieve the operand
    int opcode = mnemonicOpcodeMap[codeVector[1]];
    int operand = 0;
    //Skip operand for I/O and halt instructions
    if (((opcode / 100) != 9) and (codeVector[1] != std::string("HLT"))) {
      //Check the operand is present (optional for DAT)
      if (tokenCount < 3) {
        if (codeVector[1] != std::string("DAT")) {
          std::cerr << "ERROR: Missing operand for instruction '" << codeVector[1] << "' on line " << codeVector[0] << std::endl;
          return -1;
        }
      } else {
        try {
          operand = std::stoi(codeVector[2]);
        } catch (...) {
          std::cerr << "ERROR: Undefined label '" << codeVector[2] << "' on line " << codeVector[0] << std::endl;
          return -1;
        }

        //Check operand is 3 digits or less
        if (operand > 999 or operand < -999) {
          std::cerr << "ERROR: Operand '" << operand << "' must be between 999 and -999 on line " << codeVector[0] << std::endl;
          return -1;
        }
      }
    }

    //Save instruction to memory
    int result = opcode + operand;
    memory[i] = result;
  }

  return programLength;
}

int checkMemoryAddress(SystemState* systemState, int address) {
  if (address >= systemState->memoryLength) {
    std::cerr << "ERROR: Cannot access memory address '" << address << "'" << std::endl;
    return -1;
  }
  return 0;
}

int main(int argc, char* argv[]) {
  //Setup initial simulator state
  SystemState systemState;
  systemState.memoryLength = 100;

  //Create simulator memory
  int memory[systemState.memoryLength];
  systemState.memoryPtr = &memory[0];
  std::memset(&memory, 0, systemState.memoryLength * sizeof(int));

  if (systemState.memoryLength != 100) {
    std::cerr << "WARNING: Memory set to non-standard value, behaviour may be altered" << std::endl;
  }

  std::string filePath;
  if (argc >= 2) {
    filePath = std::string(argv[1]);
  } else {
    std::cerr << "ERROR: No input file specified" << std::endl;
    return EXIT_FAILURE;
  }

  //Read assembly file in
  std::vector<std::string> fileData;
  std::string lineData;
  std::ifstream input(filePath.c_str());
  while (std::getline(input, lineData)) {
    fileData.push_back(lineData);
  }

  int inputLength = fileData.size();
  if (fileData.size() == 0) {
    std::cerr << "ERROR: Input file specified is empty" << std::endl;
    return EXIT_FAILURE;
  }

  int programLength = assembleProgram(&memory[0], systemState.memoryLength, &fileData, inputLength);

  if (programLength == -1) {
    std::cerr << "ERROR: Failed to assemble '" << filePath << "'" << std::endl;
    return EXIT_FAILURE;
  }

  //Check if debug mode is set
  std::string debugKey;
  if (std::getenv("DEBUG") == NULL) {
    debugKey = std::string("false");
  } else {
    debugKey = std::getenv("DEBUG");
  }

  //Output 'machine code' if in debug mode
  if (debugKey == std::string("true")) {
    for (int i = 0; i < programLength; i++) {
      std::cout << memory[i] << std::endl;
    }
    std::cout << std::endl;
  }

  //Run until encountering opcode 0 (HLT)
  while (true) {
    //Check memory address is within bounds
    if (checkMemoryAddress(&systemState, systemState.programCounter) == -1) {
      return EXIT_FAILURE;
    }

    //Get opcode and operand
    int opcode = (memory[systemState.programCounter] / 100) * 100;
    int operand = memory[systemState.programCounter] - opcode;

    //Increment the program counter
    systemState.programCounter++;

    //Stop execution if told to halt
    if (opcode == 0) {
      break;
    }

    //Check operand address is within bounds
    if (checkMemoryAddress(&systemState, operand) == -1) {
      //Exception for I/O instruction, as the operand is technically part of the instruction
      if (operand / 100 != 9) {
        return EXIT_FAILURE;
      }
    }

    //Retreive and execute 'instruction'
    if (opcodeFunctionMap.contains(opcode)) {
      instructionPtrType handler = opcodeFunctionMap[opcode];
      if (handler(&systemState, operand) == -1) {
        return EXIT_FAILURE;
      }
    } else {
      std::cerr << "ERROR: Unknown opcode '" << opcode << "'" << std::endl;
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}
