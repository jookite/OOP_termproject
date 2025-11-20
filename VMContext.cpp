#include "VMContext.h"
#include <iostream>
#include <fstream>
#include <iomanip>

// 생성자
VMContext::VMContext() {
    m_registers[VMDefs::REG_R0] = 0;
    m_registers[VMDefs::REG_R1] = 0;
    m_registers[VMDefs::REG_R2] = 0;
    m_registers[VMDefs::REG_PC] = 0;
    m_registers[VMDefs::REG_SP] = 0;
    m_registers[VMDefs::REG_BP] = 0;
    m_registers[VMDefs::REG_ZF] = 0;
    m_registers[VMDefs::REG_CF] = 0;
    m_registers[VMDefs::REG_OF] = 0;

    m_stack.resize(256, 0);
}

// 바이너리 코드 로드
bool VMContext::loadCode(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return false;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    m_code.resize(size);

    if (!file.read((char*)m_code.data(), size)) {
        std::cerr << "Error: Could not read file " << filename << std::endl;
        file.close();
        return false;
    }
    file.close();

    // 명령어 하나가 4바이트이므로 반드시 4의 배수여야 함
    if (m_code.size() % 4 != 0) {
        std::cerr << "Error: code size is not multiple of 4 bytes" << std::endl;
        return false;
    }

    return true;
}

// 실행 루프
void VMContext::runCode() {
    unsigned char& pc = m_registers[VMDefs::REG_PC];
    pc = 0;

    while ((pc * 4) + 4 <= m_code.size()) {
        uint32_t instructionWord = 0;
        instructionWord |= (uint32_t)m_code[pc * 4 + 0] << 24;
        instructionWord |= (uint32_t)m_code[pc * 4 + 1] << 16;
        instructionWord |= (uint32_t)m_code[pc * 4 + 2] << 8;
        instructionWord |= (uint32_t)m_code[pc * 4 + 3] << 0;

        unsigned char old_pc = pc;

        Instruction* command = parseInstruction(instructionWord);
        if (command != nullptr) {
            command->execute(*this);
            delete command;
        }
        else {
            std::cerr << "Unknown opcode at PC=" << (int)pc << std::endl;
            break;
        }

        if (pc == old_pc) {
            pc++;
        }
    }
}

// 32비트 단어를 Instruction 객체로 변환
Instruction* VMContext::parseInstruction(uint32_t word) {
    unsigned char opcode = (word >> 26) & 0x3F;
    unsigned char flag = (word >> 24) & 0x03;
    unsigned char src = (word >> 8) & 0xFF;
    unsigned char dest = (word >> 0) & 0xFF;

    switch (opcode) {
    case VMDefs::OP_MOV:   return new OpMOV(flag, src, dest);
    case VMDefs::OP_ADD:   return new OpADD(flag, src, dest);
    case VMDefs::OP_SUB:   return new OpSUB(flag, src, dest);
    case VMDefs::OP_MUL:   return new OpMUL(flag, src, dest);
    case VMDefs::OP_CMP:   return new OpCMP(flag, src, dest);
    case VMDefs::OP_PUSH:  return new OpPUSH(flag, src, dest);
    case VMDefs::OP_POP:   return new OpPOP(flag, src, dest);
    case VMDefs::OP_JMP:   return new OpJMP(flag, src, dest);
    case VMDefs::OP_BE:    return new OpBE(flag, src, dest);
    case VMDefs::OP_BNE:   return new OpBNE(flag, src, dest);
    case VMDefs::OP_PRINT: return new OpPRINT(flag, src, dest);
    default:
        return nullptr;
    }
}

// 레지스터/스택 헬퍼들
unsigned char VMContext::getRegisterValue(unsigned char regID) {
    if (m_registers.count(regID)) return m_registers[regID];
    return 0;
}

void VMContext::setRegisterValue(unsigned char regID, unsigned char value) {
    if (m_registers.count(regID)) m_registers[regID] = value;
}

void VMContext::pushStack(unsigned char value) {
    unsigned char& sp = m_registers[VMDefs::REG_SP];
    m_stack[sp] = value;
    sp++;
}

unsigned char VMContext::popStack() {
    unsigned char& sp = m_registers[VMDefs::REG_SP];
    sp--;
    return m_stack[sp];
}

unsigned char VMContext::getStackValue(unsigned char index) {
    if (index < m_stack.size()) return m_stack[index];
    return 0;
}
