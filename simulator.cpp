#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>
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

//TODO - replace test program
std::string rawInput[] = {
  "num1 DAT 1",
  "num2 DAT 2",
  "LDA num2",
  "ADD num1",
  "OUT",
  "HLT"
};

bool assembleProgram(int memory[], std::string inputData[], int inputDataLength) {
  //Create variable sized storage for tokens
  std::vector<std::string> codeVectors[inputDataLength];

  //Split into code vectors
  for (int i = 0; i < inputDataLength; i++) {
    std::string inputLine = inputData[i];
    int stringLength = inputLine.size();
    int tokenCount = 0;
    codeVectors[i].push_back(std::string(""));

    //Iterate over each letter in the line of code
    for (int letterIndex = 0; letterIndex < stringLength; letterIndex++) {
      //Create a new token on each space, otherwise add to the current token
      if (inputLine[letterIndex] == ' ') {
        tokenCount++;
        codeVectors[i].push_back(std::string(""));
      } else {
        codeVectors[i][tokenCount] += inputLine[letterIndex];
      }
    }
  }

  //Save label addresses and remove
  std::map<std::string, int> labelIndexMap;
  for (int i = 0; i < inputDataLength; i++) {
    std::vector<std::string> codeVector = codeVectors[i];

    //Save label index and remove label
    if (!mnemonicOpcodeMap.contains(codeVector[0])) {
      labelIndexMap[codeVector[0]] = i;
      codeVectors[i].erase(codeVectors[i].begin());
    }
  }

  //Convert opcodes, operands and labels into 'machine code'
  for (int i = 0; i < inputDataLength; i++) {
    std::vector<std::string> codeVector = codeVectors[i];

    //Replace labels with addresses
    for (unsigned int tokenIndex = 0; tokenIndex < codeVector.size(); tokenIndex++) {
      if (labelIndexMap.contains(codeVector[tokenIndex])) {
        codeVector[tokenIndex] = std::to_string(labelIndexMap[codeVector[tokenIndex]]);
      }
    }

    //Convert mnemonic to an opcode, then retrieve the operand
    int opcode = mnemonicOpcodeMap[codeVector[0]];
    int operand = 0;
    if (opcode <= 900 and codeVector[0] != std::string("HLT")) {
      operand = std::stoi(codeVector[1]);
    }

    //Save instruction to memory
    int result = opcode + operand;
    memory[i] = result;
  }

  return true;
}

int main() {
  int memoryLength = 100;
  int memory[memoryLength];
  std::memset(&memory, 0, memoryLength * sizeof(int));

  int programLength = sizeof(rawInput) / sizeof(rawInput[0]);
  bool success = assembleProgram(&memory[0], &rawInput[0], programLength);

  if (!success) {
    return EXIT_FAILURE;
  }

  for (int i = 0; i < programLength; i++) {
    std::cout << memory[i] << std::endl;
  }


  //Create computer data
  //Start execution


  return EXIT_SUCCESS;
}

/* TODO:
 - Add different exit codes
 - Handle extra whitespace
 - Handle blank lines
 - Support reading from files
 - Computer implementation
 - Check operands are present before accessing

 - Document the instructions
 - Add examples
 - Add self-tests
*/

/* Valid forms:
  label opcode
  label opcode operand
  label opcode label
  opcode
  opcode operand
  opcode label
*/

//Variables for each register
//Keep CPU data in a struct, passed to each instruction's function

//Load instruction, incremement program counter (at the correct point), execute instruction
//Helper to get an opcode from an instruction
//Opcode to function pointer table
