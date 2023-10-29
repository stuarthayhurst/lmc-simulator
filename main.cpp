#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include "simulator.hpp"

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
      if (inputLine[letterIndex] == ' ' || inputLine[letterIndex] == '\t') {
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

int main(int argc, char* argv[]) {
  //Setup simulator with 100 memory locations
  int memoryLength = 100;
  int* memoryPtr = setupSimulator(memoryLength);

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
  if (input.is_open()) {
    while (std::getline(input, lineData)) {
      fileData.push_back(lineData);
    }
  } else {
    std::cerr << "ERROR: Input file doesn't exist" << std::endl;
    return EXIT_FAILURE;
  }

  int inputLength = fileData.size();
  if (fileData.size() == 0) {
    std::cerr << "ERROR: Input file specified is empty" << std::endl;
    return EXIT_FAILURE;
  }

  int programLength = assembleProgram(memoryPtr, memoryLength, &fileData, inputLength);

  if (programLength == -1) {
    std::cerr << "ERROR: Failed to assemble '" << filePath << "'" << std::endl;
    return EXIT_FAILURE;
  }

  //Set debug mode from environment
  bool debug = (std::getenv("DEBUG") != NULL) ?
               (std::getenv("DEBUG") == std::string("true")) : false;

  //Output 'machine code' if in debug mode
  if (debug) {
    printMemory(programLength);
  }

  bool success = true;
  //Run until encountering opcode 0 (HLT) or an error
  while (executeNextInstruction(&success));

  //Clean up and exit
  destroySimulator();
  return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
