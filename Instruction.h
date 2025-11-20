#ifndef INSTRUCTION_H
#define INSTRUCTION_H

class VMContext;  // forward declaration

namespace VMDefs {
    // 2-3. Registers
    const unsigned char REG_R0 = 0x01;
    const unsigned char REG_R1 = 0x02;
    const unsigned char REG_R2 = 0x03;
    const unsigned char REG_PC = 0x04;
    const unsigned char REG_SP = 0x05;
    const unsigned char REG_BP = 0x06;
    const unsigned char REG_ZF = 0x07;
    const unsigned char REG_CF = 0x08;
    const unsigned char REG_OF = 0x09;

    // 2-4. Opcodes
    const unsigned char OP_MOV = 0x01;
    const unsigned char OP_ADD = 0x02;
    const unsigned char OP_SUB = 0x03;
    const unsigned char OP_MUL = 0x04;
    const unsigned char OP_CMP = 0x05;
    const unsigned char OP_PUSH = 0x06;
    const unsigned char OP_POP = 0x07;
    const unsigned char OP_JMP = 0x08;
    const unsigned char OP_BE = 0x09;
    const unsigned char OP_BNE = 0x0A;
    const unsigned char OP_PRINT = 0x0B;

    // 2-5. Flags
    const unsigned char FLAG_REG_REG = 0x00;  // 두 피연산자 모두 레지스터
    const unsigned char FLAG_REG_VAL = 0x01;  // dest = Reg, src = imm
    const unsigned char FLAG_REG_ONLY = 0x02; // 단일 Reg
    const unsigned char FLAG_VAL_ONLY = 0x03; // 단일 imm
}

// Command 인터페이스
class Instruction {
public:
    Instruction(unsigned char flag, unsigned char src, unsigned char dest)
        : m_flag(flag), m_src(src), m_dest(dest) {
    }
    virtual ~Instruction() {}

    virtual void execute(VMContext& context) = 0;

protected:
    unsigned char m_flag;
    unsigned char m_src;
    unsigned char m_dest;
};

// 각 구체 명령어들
class OpMOV : public Instruction {
public:
    OpMOV(unsigned char flag, unsigned char src, unsigned char dest)
        : Instruction(flag, src, dest) {
    }
    void execute(VMContext& context) override;
};

class OpADD : public Instruction {
public:
    OpADD(unsigned char flag, unsigned char src, unsigned char dest)
        : Instruction(flag, src, dest) {
    }
    void execute(VMContext& context) override;
};

class OpSUB : public Instruction {
public:
    OpSUB(unsigned char flag, unsigned char src, unsigned char dest)
        : Instruction(flag, src, dest) {
    }
    void execute(VMContext& context) override;
};

class OpMUL : public Instruction {
public:
    OpMUL(unsigned char flag, unsigned char src, unsigned char dest)
        : Instruction(flag, src, dest) {
    }
    void execute(VMContext& context) override;
};

class OpCMP : public Instruction {
public:
    OpCMP(unsigned char flag, unsigned char src, unsigned char dest)
        : Instruction(flag, src, dest) {
    }
    void execute(VMContext& context) override;
};

class OpPUSH : public Instruction {
public:
    OpPUSH(unsigned char flag, unsigned char src, unsigned char dest)
        : Instruction(flag, src, dest) {
    }
    void execute(VMContext& context) override;
};

class OpPOP : public Instruction {
public:
    OpPOP(unsigned char flag, unsigned char src, unsigned char dest)
        : Instruction(flag, src, dest) {
    }
    void execute(VMContext& context) override;
};

class OpJMP : public Instruction {
public:
    OpJMP(unsigned char flag, unsigned char src, unsigned char dest)
        : Instruction(flag, src, dest) {
    }
    void execute(VMContext& context) override;
};

class OpBE : public Instruction {
public:
    OpBE(unsigned char flag, unsigned char src, unsigned char dest)
        : Instruction(flag, src, dest) {
    }
    void execute(VMContext& context) override;
};

class OpBNE : public Instruction {
public:
    OpBNE(unsigned char flag, unsigned char src, unsigned char dest)
        : Instruction(flag, src, dest) {
    }
    void execute(VMContext& context) override;
};

class OpPRINT : public Instruction {
public:
    OpPRINT(unsigned char flag, unsigned char src, unsigned char dest)
        : Instruction(flag, src, dest) {
    }
    void execute(VMContext& context) override;
};

#endif // INSTRUCTION_H
