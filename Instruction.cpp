#include "Instruction.h"
#include "VMContext.h"
#include <iostream>
#include <cstdlib>

// ======================================================
// [Helper] 공통 검증 및 값 추출기
// ======================================================
namespace InstHelper {

    // 에러 출력 및 종료
    void error(const char* name, unsigned char flag) {
        std::cerr << "[Error] " << name << ": 잘못된 피연산자 타입입니다. (Flag: "
            << (int)flag << ")" << std::endl;
        exit(1);
    } 

    // [Group A] 이항 연산용 (MOV, ADD, SUB...)
    // 허용: 00(REG_REG), 01(REG_VAL)
    // 역할: 두 번째 피연산자(Source)의 실제 값을 가져옴
    unsigned char getBinarySrc(VMContext& ctx, const char* name, unsigned char flag, unsigned char src) {
        if (flag == VMDefs::FLAG_REG_REG) return ctx.getRegisterValue(src);
        if (flag == VMDefs::FLAG_REG_VAL) return src;
        error(name, flag);
        return 0;
    }

    // Operand 타입 구분: [Register] ONLY
    void validateRegOnly(const char* name, unsigned char flag) {
        if (flag != VMDefs::FLAG_REG_ONLY) {
            error(name, flag);
        }
    }

    // Operand 타입 구분: [Register] OR [Value]
    unsigned char getTargetValue(VMContext& ctx, const char* name, unsigned char flag, unsigned char operand) {
        if (flag == VMDefs::FLAG_REG_ONLY) return ctx.getRegisterValue(operand);
        if (flag == VMDefs::FLAG_VAL_ONLY) return operand;
        error(name, flag);
        return 0;
    }

    // overflow check(ADD)
    bool checkAddOverflow(int8_t a, int8_t b, int8_t res) {
        return (a > 0 && b > 0 && res < 0) || (a < 0 && b < 0 && res > 0);
    }

    //overflow check(SUB)
    bool checkSubOverflow(int8_t a, int8_t b, int8_t res) {
        return (a > 0 && b < 0 && res < 0) || (a < 0 && b > 0 && res > 0);
    }

}

// ======================================================
// 1. 이항 연산 명령어 (MOV, ADD, SUB, MUL, CMP)
// ======================================================

void OpMOV::execute(VMContext& context) {
    unsigned char srcVal = InstHelper::getBinarySrc(context, "MOV", m_flag, m_src);
    context.setRegisterValue(m_dest, srcVal);
}

void OpADD::execute(VMContext& context) {
    unsigned char val1 = context.getRegisterValue(m_dest);
    unsigned char val2 = InstHelper::getBinarySrc(context, "ADD", m_flag, m_src);

    uint16_t result = (uint16_t)val1 + (uint16_t)val2;
    context.setRegisterValue(m_dest, (unsigned char)result);

    context.setRegisterValue(VMDefs::REG_ZF, ((unsigned char)result == 0) ? 1 : 0);
    context.setRegisterValue(VMDefs::REG_CF, (result > 255) ? 1 : 0);

    
    bool ovf = InstHelper::checkAddOverflow((int8_t)val1, (int8_t)val2, (int8_t)result);
    context.setRegisterValue(VMDefs::REG_OF, ovf ? 1 : 0);
}

void OpSUB::execute(VMContext& context) {
    unsigned char val1 = context.getRegisterValue(m_dest);
    unsigned char val2 = InstHelper::getBinarySrc(context, "SUB", m_flag, m_src);

    uint16_t result = (uint16_t)val1 - (uint16_t)val2;
    context.setRegisterValue(m_dest, (unsigned char)result);

    context.setRegisterValue(VMDefs::REG_ZF, ((unsigned char)result == 0) ? 1 : 0);
    context.setRegisterValue(VMDefs::REG_CF, (val1 < val2) ? 1 : 0);

  
    bool ovf = InstHelper::checkSubOverflow((int8_t)val1, (int8_t)val2, (int8_t)result);
    context.setRegisterValue(VMDefs::REG_OF, ovf ? 1 : 0);
}

void OpMUL::execute(VMContext& context) {
    unsigned char val1 = context.getRegisterValue(m_dest);
    unsigned char val2 = InstHelper::getBinarySrc(context, "MUL", m_flag, m_src);

    uint16_t result = (uint16_t)val1 * (uint16_t)val2;
    context.setRegisterValue(m_dest, (unsigned char)result);
}

void OpCMP::execute(VMContext& context) {
    unsigned char val1 = context.getRegisterValue(m_dest);
    unsigned char val2 = InstHelper::getBinarySrc(context, "CMP", m_flag, m_src);

    int16_t diff = (int8_t)val1 - (int8_t)val2;

    context.setRegisterValue(VMDefs::REG_ZF, (diff == 0) ? 1 : 0);
    context.setRegisterValue(VMDefs::REG_CF, (diff >= 1) ? 1 : 0);
    context.setRegisterValue(VMDefs::REG_OF, (diff <= -1) ? 1 : 0);
}


// ======================================================
// 2. 스택 명령어 (PUSH, POP)
// ======================================================

void OpPUSH::execute(VMContext& context) {
    unsigned char val = InstHelper::getTargetValue(context, "PUSH", m_flag, m_dest);
    context.pushStack(val);
}

void OpPOP::execute(VMContext& context) {
    InstHelper::validateRegOnly("POP", m_flag);

    unsigned char val = context.popStack();
    context.setRegisterValue(m_dest, val);
}


// ======================================================
// 3. 분기 및 출력 명령어 (JMP, BE, BNE, PRINT)
// ======================================================

void OpJMP::execute(VMContext& context) {
    unsigned char target = InstHelper::getTargetValue(context, "JMP", m_flag, m_dest);
    context.setRegisterValue(VMDefs::REG_PC, target);
}

void OpBE::execute(VMContext& context) {
    unsigned char target = InstHelper::getTargetValue(context, "BE", m_flag, m_dest);

    if (context.getRegisterValue(VMDefs::REG_ZF) == 1) {
        context.setRegisterValue(VMDefs::REG_PC, target);
    }
}

void OpBNE::execute(VMContext& context) {
    unsigned char target = InstHelper::getTargetValue(context, "BNE", m_flag, m_dest);

    if (context.getRegisterValue(VMDefs::REG_ZF) != 1) {
        context.setRegisterValue(VMDefs::REG_PC, target);
    }
}

void OpPRINT::execute(VMContext& context) {
    unsigned char val = InstHelper::getTargetValue(context, "PRINT", m_flag, m_dest);
    std::cout << (int)val << std::endl;
}