#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

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

struct SystemState {
  int* memoryPtr;
  int memoryLength;
  int accumulator = 0;
  int programCounter = 0;
};

int assembleProgram(int memory[], int memoryLength, std::vector<std::string>* inputData, int inputDataLength) {
  //Create variable sized storage for tokens
  std::vector<std::vector<std::string>> codeVectors;

  //Split into code vectors
  for (int i = 0; i < inputDataLength; i++) {
    std::vector<std::string> codeVector;
    std::string inputLine = (*inputData)[i];

    int tokenIndex = -1;
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
    if (tokenIndex != -1) {
      codeVectors.push_back(codeVector);
    }
  }

  //Get the new size of the program, after splitting into vectors
  int programLength = codeVectors.size();

  //Fail if memory won't be able to hold the program
  if (programLength > memoryLength) {
    std::cerr << "Memory is not large enough to store program" << std::endl;
    return -1;
  }

  //Save label addresses and remove
  std::map<std::string, int> labelIndexMap;
  for (int i = 0; i < programLength; i++) {
    std::vector<std::string> codeVector = codeVectors[i];

    //Save label index and remove label
    if (!mnemonicOpcodeMap.contains(codeVector[0])) {
      labelIndexMap[codeVector[0]] = i;
      codeVectors[i].erase(codeVectors[i].begin());
    }
  }

  //Convert opcodes, operands and labels into 'machine code'
  for (int i = 0; i < programLength; i++) {
    std::vector<std::string> codeVector = codeVectors[i];

    //Skip any empty lines due to labels
    int tokenCount = codeVector.size();
    if (tokenCount == 0) {
      continue;
    }

    //Replace labels with addresses
    for (unsigned int tokenIndex = 0; tokenIndex < codeVector.size(); tokenIndex++) {
      if (labelIndexMap.contains(codeVector[tokenIndex])) {
        codeVector[tokenIndex] = std::to_string(labelIndexMap[codeVector[tokenIndex]]);
      }
    }

    if (!mnemonicOpcodeMap.contains(codeVector[0])) {
      std::cerr << "Unrecognised instruction '" << codeVector[0] << "'" << std::endl;
      return -1;
    }

    //Convert mnemonic to an opcode, then retrieve the operand
    int opcode = mnemonicOpcodeMap[codeVector[0]];
    int operand = 0;
    //Skip operand for I/O and halt instructions
    if (((opcode / 100) != 9) and (codeVector[0] != std::string("HLT"))) {
      //Check the operand is present (optional for DAT)
      if (tokenCount < 2) {
        if (codeVector[0] != std::string("DAT")) {
          std::cerr << "Missing operand for instruction '" << codeVector[0] << "'" << std::endl;
          return -1;
        }
      } else {
        operand = std::stoi(codeVector[1]);
      }
    }

    //Save instruction to memory
    int result = opcode + operand;
    memory[i] = result;
  }

  return programLength;
}

namespace instructions {
  void add(SystemState* systemState, int operand) {
    systemState->accumulator += (systemState->memoryPtr)[operand];
  }

  void subtract(SystemState* systemState, int operand) {
    systemState->accumulator -= (systemState->memoryPtr)[operand];
  }

  void store(SystemState* systemState, int operand) {
    (systemState->memoryPtr)[operand] = systemState->accumulator;
  }

  void load(SystemState* systemState, int operand) {
    systemState->accumulator = (systemState->memoryPtr)[operand];
  }

  void branchAlways(SystemState* systemState, int operand) {
    systemState->programCounter = operand;
  }

  void branchZero(SystemState* systemState, int operand) {
    if (systemState->accumulator == 0) {
      systemState->programCounter = operand;
    }
  }

  void branchPositive(SystemState* systemState, int operand) {
    if (systemState->accumulator > 0) {
      systemState->programCounter = operand;
    }
  }

  void inputOutput(SystemState* systemState, int operand) {
    if (operand == 1) { //Input
      std::cin >> systemState->accumulator;
    } else if (operand == 2) { //Output
      std::cout << systemState->accumulator << std::endl;
    }
  }
}

typedef void (*instructionPtrType)(SystemState*, int);
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

int main(int argc, char* argv[]) {
  int memoryLength = 100;
  int memory[memoryLength];
  std::memset(&memory, 0, memoryLength * sizeof(int));

  if (memoryLength != 100) {
    std::cerr << "Memory set to non-standard value, behaviour may be altered" << std::endl;
  }

  std::string filePath;
  if (argc >= 2) {
    filePath = std::string(argv[1]);
  } else {
    std::cerr << "No input file specified" << std::endl;
    return -1;
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
    return -1;
  }

  int programLength = assembleProgram(&memory[0], memoryLength, &fileData, inputLength);

  if (programLength == -1) {
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

  //Setup simulator state
  SystemState systemState;
  systemState.memoryPtr = &memory[0];
  systemState.memoryLength = memoryLength;

  //Run until encountering opcode 0 (HLT)
  while (true) {
    //Get opcode and operand
    int opcode = (memory[systemState.programCounter] / 100) * 100;
    int operand = memory[systemState.programCounter] - opcode;

    //Increment the program counter
    systemState.programCounter++;

    //Stop execution if told to halt
    if (opcode == 0) {
      break;
    }

    //Retreive and execute 'instruction'
    if (opcodeFunctionMap.contains(opcode)) {
      instructionPtrType handler = opcodeFunctionMap[opcode];
      handler(&systemState, operand);
    } else {
      std::cerr << "Unknown opcode '" << opcode << "'" << std::endl;
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}

/* TODO:
 - Check memory bounds in instructions
 - Manually apply overflow and underflow for LMC memory limits
 - Apply overflow and underflow to operand values when assembling
 - Handle unknown opcode in i/o instruction
 - Check for unknown labels in the operand

 - Document the instructions
 - Add examples
 - Add self-tests
*/
