#ifndef VMCONTEXT_H
#define VMCONTEXT_H

#include <vector>
#include <map>
#include <string>
#include <cstdint>
#include "Instruction.h"

class VMContext {
public:
    VMContext();
    ~VMContext() {}

    bool loadCode(const std::string& filename);
    void runCode();

    unsigned char getRegisterValue(unsigned char regID);
    void setRegisterValue(unsigned char regID, unsigned char value);

    void pushStack(unsigned char value);
    unsigned char popStack();
    unsigned char getStackValue(unsigned char index);

private:
    Instruction* parseInstruction(uint32_t instructionWord);

    std::map<unsigned char, unsigned char> m_registers;
    std::vector<unsigned char> m_stack;
    std::vector<unsigned char> m_code;
};

#endif
