#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "common/state.hpp"
#include "common/instructions.hpp"

namespace {
  struct CodeData {
    int lineNumber;
    std::string instruction;
    std::string operandLabel = "NONE";
    int operandData = 0;
  };
}

namespace {
  static std::vector<std::string> splitLine(std::string lineText) {
    std::vector<std::string> codeVector;
    bool lastCharacterWasSpace = true;
    int tokenIndex = -1;

    //Split the line of code into tokens
    int stringLength = lineText.size();
    for (int letterIndex = 0; letterIndex < stringLength; letterIndex++) {
      //Create a new token on each character group
      if (lineText[letterIndex] == ' ' || lineText[letterIndex] == '\t') {
        lastCharacterWasSpace = true;
      } else {
        if (lastCharacterWasSpace) {
          if (lineText[letterIndex] == '#') {
            break;
          }
          tokenIndex++;
          codeVector.push_back(std::string(""));
          lastCharacterWasSpace = false;
        }

        //Add the character to the current token
        codeVector[tokenIndex] += lineText[letterIndex];
      }
    }

    //Return either the parsed line or nothing
    if (tokenIndex == -1) {
      codeVector.clear();
    }
    return codeVector;
  }

  /*
   - Convert a vector of lines of code into tokens
   - Build a label address map at the same time
   - Returns true if successful
  */
  static bool tokeniseCode(std::vector<std::string>* code, std::vector<CodeData>* tokens,
                           std::unordered_map<std::string, int>* labelAddressMap) {
    std::vector<std::string> pendingLabels;
    int lineAddress = 0;

    /*
     - Generate a token for each line of code, skipping comments and whitespace
     - Add label declarations to pendingLabels, and convert them to an address
       when a code line is found
    */
    for (unsigned int i = 0; i < code->size(); i++) {
      //Split the line into words, skip comments
      std::vector<std::string> lineVector = splitLine((*code)[i]);
      if (lineVector.size() == 0) {
        continue;
      }

      //If the first word isn't an instruction, assume it's a label
      if (!instructions::mnemonicOpcodeMap.contains(lineVector[0])) {
        //Use the next instruction as the label's target, which could be the current line
        pendingLabels.push_back(lineVector[0]);
        lineVector.erase(lineVector.begin());
        if (lineVector.size() == 0) {
          continue;
        }
      }

      //Fill in pending labels
      for (unsigned int labelIndex = 0; labelIndex < pendingLabels.size(); labelIndex++) {
        (*labelAddressMap)[pendingLabels[labelIndex]] = lineAddress;
      }
      pendingLabels.clear();

      //If the next word still isn't an instruction, give up
      if (!instructions::mnemonicOpcodeMap.contains(lineVector[0])) {
        std::cerr << "ERROR: Expected an instruction on line " << i + 1 \
                  << ", got '" << lineVector[0] << "' instead" << std::endl;
        return false;
      }

      //Add an entry for the processed data and the instruction
      tokens->emplace_back();
      (*tokens)[lineAddress].instruction = lineVector[0];
      (*tokens)[lineAddress].lineNumber = i + 1;

      //Check operand is present and save, if required
      bool operandPresent = (lineVector.size() > 1) ? true : false;
      if (instructions::mnemonicOperandMap.at(lineVector[0])) {
        //Requires an operand, none found
        if (!operandPresent) {
          std::cerr << "ERROR: Missing operand for instruction '" << lineVector[0] \
                    << "' on line " << i + 1 << std::endl;
          return false;
        }

        (*tokens)[lineAddress].operandLabel = lineVector[1];
      }

      lineAddress++;
    }

    //Warn about remaining unresolved labels
    for (unsigned int i = 0; i < pendingLabels.size(); i++) {
      std::cerr << "WARNING: Couldn't give label '" << pendingLabels[i] \
                << "' a line number, ignoring" << std::endl;
    }
    pendingLabels.clear();

    return true;
  }
}

/*
 - Assemble a program into memory
 - Only modifies the memory, doesn't modify the system state otherwise
 - Returns the size of the assembled program
*/
int assembleProgram(SystemState* systemState, std::vector<std::string>* inputData) {
  //Variable length storage for processed data
  std::vector<CodeData> processedData;

  //Track label addresses and pending labels
  std::unordered_map<std::string, int> labelAddressMap = {{"NONE", 0}};

  //Generate tokens and label address map
  if (!tokeniseCode(inputData, &processedData, &labelAddressMap)) {
    return -1;
  }

  //Get the final size of the program
  int programLength = processedData.size();

  //Fail if memory won't be able to hold the program
  if (programLength > systemState->memoryLength) {
    std::cerr << "ERROR: Memory isn't large enough to store program (" << programLength \
              << " > " << systemState->memoryLength << ")" << std::endl;
    return -1;
  }

  //Convert processed data into 'machine code'
  for (unsigned int i = 0; i < processedData.size(); i++) {
    //Convert mnemonic to an opcode
    int opcode = instructions::mnemonicOpcodeMap.at(processedData[i].instruction);

    //Convert labels to addresses and data
    int operand = 0;
    if (labelAddressMap.contains(processedData[i].operandLabel)) {
      operand = labelAddressMap[processedData[i].operandLabel];
    } else if (processedData[i].instruction == "DAT") {
      //DAT can have a label or data as an operand
      try {
        processedData[i].operandData = std::stoi(processedData[i].operandLabel);
        processedData[i].operandLabel = "NONE";
      } catch (...) {
        std::cerr << "ERROR: Operand '" << processedData[i].operandLabel \
                  << "' isn't a label or numerical on line " \
                  << processedData[i].lineNumber << std::endl;
        return -1;
      }
    } else {
      std::cerr << "ERROR: Undefined label '" << processedData[i].operandLabel \
                << "' on line " << processedData[i].lineNumber << std::endl;
      return -1;
    }

    //Check operand is 3 digits or less
    int operandData = processedData[i].operandData;
    if (operandData > 999 or operandData < -999) {
      std::cerr << "ERROR: Operand '" << operandData \
                << "' must be between 999 and -999 on line " \
                << processedData[i].lineNumber << std::endl;
      return -1;
    }

    //Write the result to memory
    systemState->memoryPtr[i] = opcode + operand + processedData[i].operandData;
  }

  return programLength;
}
