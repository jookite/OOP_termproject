#ifndef INSTRUCTION_H
#define INSTRUCTION_H

// VMContext.h를 직접 include하는 대신 전방 선언(forward declaration)을 사용합니다.
// C++에서 순환 참조(circular dependency)를 방지하기 위한 표준적인 기법입니다.
class VMContext;

/**
 * @brief 프로젝트 명세서의 모든 상수 정의
 */
namespace VMDefs {
    // 2-3. 구현해야 할 레지스터 (8bit ID)
    const unsigned char REG_R0 = 0x01;
    const unsigned char REG_R1 = 0x02;
    const unsigned char REG_R2 = 0x03;
    const unsigned char REG_PC = 0x04; // 다음 실행할 코드의 *위치* (인덱스)
    const unsigned char REG_SP = 0x05; // 스택 포인터
    const unsigned char REG_BP = 0x06; // 베이스 포인터
    const unsigned char REG_ZF = 0x07; // Zero Flag
    const unsigned char REG_CF = 0x08; // Carry Flag
    const unsigned char REG_OF = 0x09; // Overflow Flag

    // 2-4. 구현해야 할 명령어 (6bit Opcode)
    const unsigned char OP_MOV = 0x01;   // 000001
    const unsigned char OP_ADD = 0x02;   // 000010
    const unsigned char OP_SUB = 0x03;   // 000011
    const unsigned char OP_MUL = 0x04;   // 000100
    const unsigned char OP_CMP = 0x05;   // 000101
    const unsigned char OP_PUSH = 0x06;  // 000110
    const unsigned char OP_POP = 0x07;   // 000111
    const unsigned char OP_JMP = 0x08;   // 001000
    const unsigned char OP_BE = 0x09;    // 001001
    const unsigned char OP_BNE = 0x0A;   // 001010
    const unsigned char OP_PRINT = 0x0B; // 001011

    // 2-5. Flag (2bit) [cite: 67-71]
    const unsigned char FLAG_REG_REG = 0x00;  // 00: Src(Reg), Dest(Reg)
    const unsigned char FLAG_REG_VAL = 0x01;  // 01: Dest(Reg), Src(Val)
    const unsigned char FLAG_REG_ONLY = 0x02; // 10: 단일 Reg Operand
    const unsigned char FLAG_VAL_ONLY = 0x03; // 11: 단일 Val Operand
}

/**
 * @brief [Design Pattern] Command (Interface)
 * 모든 명령어 클래스의 기반이 되는 추상 클래스입니다.
 * 'execute' 메서드를 순수 가상 함수로 정의합니다.
 */
class Instruction {
public:
    // 생성자: 파서(VMContext::parseInstruction)가 파싱한 정보를 저장합니다.
    Instruction(unsigned char flag, unsigned char src, unsigned char dest)
        : m_flag(flag), m_src(src), m_dest(dest) {
    } // [cite: 113-115]

// 가상 소멸자 (상속을 위한 필수 요소)
    virtual ~Instruction() {}

    /**
     * @brief [Command Pattern] execute 메서드
     * 이 명령어가 실제로 수행해야 할 동작을 정의합니다.
     * VM의 상태(VMContext)를 참조(&)로 받아 직접 수정합니다.
     */
    virtual void execute(VMContext& context) = 0;

protected:
    unsigned char m_flag; 
    unsigned char m_src;  
    unsigned char m_dest; 
};


// --- 11개의 모든 구체적인 명령어 클래스 선언 ---
// [cite: 120-137]
// 각 클래스는 Instruction 인터페이스를 구현(상속)합니다.

class OpMOV : public Instruction {
public:
    OpMOV(unsigned char flag, unsigned char src, unsigned char dest) : Instruction(flag, src, dest) {}
    virtual void execute(VMContext& context); // [cite: 48]
};

class OpADD : public Instruction {
public:
    OpADD(unsigned char flag, unsigned char src, unsigned char dest) : Instruction(flag, src, dest) {}
    virtual void execute(VMContext& context); // [cite: 48]
};

class OpSUB : public Instruction {
public:
    OpSUB(unsigned char flag, unsigned char src, unsigned char dest) : Instruction(flag, src, dest) {}
    virtual void execute(VMContext& context); // [cite: 48]
};

class OpMUL : public Instruction {
public:
    OpMUL(unsigned char flag, unsigned char src, unsigned char dest) : Instruction(flag, src, dest) {}
    virtual void execute(VMContext& context); // [cite: 48]
};

class OpCMP : public Instruction {
public:
    OpCMP(unsigned char flag, unsigned char src, unsigned char dest) : Instruction(flag, src, dest) {}
    virtual void execute(VMContext& context); // [cite: 48]
};

class OpPUSH : public Instruction {
public:
    OpPUSH(unsigned char flag, unsigned char src, unsigned char dest) : Instruction(flag, src, dest) {}
    virtual void execute(VMContext& context); // [cite: 48]
};

class OpPOP : public Instruction {
public:
    OpPOP(unsigned char flag, unsigned char src, unsigned char dest) : Instruction(flag, src, dest) {}
    virtual void execute(VMContext& context); // [cite: 48]
};

class OpJMP : public Instruction {
public:
    OpJMP(unsigned char flag, unsigned char src, unsigned char dest) : Instruction(flag, src, dest) {}
    virtual void execute(VMContext& context); // [cite: 48]
};

class OpBE : public Instruction {
public:
    OpBE(unsigned char flag, unsigned char src, unsigned char dest) : Instruction(flag, src, dest) {}
    virtual void execute(VMContext& context); // [cite: 48]
};

class OpBNE : public Instruction {
public:
    OpBNE(unsigned char flag, unsigned char src, unsigned char dest) : Instruction(flag, src, dest) {}
    virtual void execute(VMContext& context); // [cite: 51]
};

class OpPRINT : public Instruction {
public:
    OpPRINT(unsigned char flag, unsigned char src, unsigned char dest) : Instruction(flag, src, dest) {}
    virtual void execute(VMContext& context); // [cite: 51]
};

#endif // INSTRUCTION_H