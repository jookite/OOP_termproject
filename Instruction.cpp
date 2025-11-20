#include "Instruction.h"
#include "VMContext.h"
#include <iostream>
#include <cstdint>

// ------------------------------------------------------
// 공통 헬퍼 함수
// flag에 따라 "실제 값"을 가져온다.
//  - 두 피연산자 명령 (REG_REG, REG_VAL) : src 사용
//  - 단일 피연산자 명령 (REG_ONLY, VAL_ONLY) : dest 사용
// ------------------------------------------------------
static unsigned char getSourceValue(
    VMContext& context,
    unsigned char flag,
    unsigned char src,
    unsigned char dest)
{
    switch (flag) {
    case VMDefs::FLAG_REG_REG:   // src = 레지스터 ID
        return context.getRegisterValue(src);
    case VMDefs::FLAG_REG_VAL:   // src = 즉값
        return src;
    case VMDefs::FLAG_REG_ONLY:  // 단일 레지스터 (dest에 ID가 들어있음)
        return context.getRegisterValue(dest);
    case VMDefs::FLAG_VAL_ONLY:  // 단일 즉값 (dest 자체가 값)
        return dest;
    default:
        return 0;
    }
}

// --- 1. MOV ---
void OpMOV::execute(VMContext& context) {
    unsigned char value = 0;
    if (m_flag == VMDefs::FLAG_REG_REG) {      // Reg -> Reg
        value = context.getRegisterValue(m_src);
    }
    else if (m_flag == VMDefs::FLAG_REG_VAL) { // Val -> Reg
        value = m_src;
    }
    context.setRegisterValue(m_dest, value);
}

// --- 2. ADD ---
void OpADD::execute(VMContext& context) {
    unsigned char val1 = context.getRegisterValue(m_dest);
    unsigned char val2 = getSourceValue(context, m_flag, m_src, m_dest);

    uint16_t result = (uint16_t)val1 + (uint16_t)val2;
    context.setRegisterValue(m_dest, (unsigned char)result);

    context.setRegisterValue(VMDefs::REG_ZF, ((unsigned char)result == 0) ? 1 : 0);
    context.setRegisterValue(VMDefs::REG_CF, (result > 255) ? 1 : 0);

    int8_t s_val1 = (int8_t)val1;
    int8_t s_val2 = (int8_t)val2;
    int8_t s_result = (int8_t)result;
    if ((s_val1 > 0 && s_val2 > 0 && s_result < 0) ||
        (s_val1 < 0 && s_val2 < 0 && s_result > 0)) {
        context.setRegisterValue(VMDefs::REG_OF, 1);
    }
    else {
        context.setRegisterValue(VMDefs::REG_OF, 0);
    }
}

// --- 3. SUB ---
void OpSUB::execute(VMContext& context) {
    unsigned char val1 = context.getRegisterValue(m_dest);
    unsigned char val2 = getSourceValue(context, m_flag, m_src, m_dest);

    uint16_t result = (uint16_t)val1 - (uint16_t)val2;
    context.setRegisterValue(m_dest, (unsigned char)result);

    context.setRegisterValue(VMDefs::REG_ZF, ((unsigned char)result == 0) ? 1 : 0);
    context.setRegisterValue(VMDefs::REG_CF, (val1 < val2) ? 1 : 0);

    int8_t s_val1 = (int8_t)val1;
    int8_t s_val2 = (int8_t)val2;
    int8_t s_result = (int8_t)result;
    if ((s_val1 > 0 && s_val2 < 0 && s_result < 0) ||
        (s_val1 < 0 && s_val2 > 0 && s_result > 0)) {
        context.setRegisterValue(VMDefs::REG_OF, 1);
    }
    else {
        context.setRegisterValue(VMDefs::REG_OF, 0);
    }
}

// --- 4. MUL ---
void OpMUL::execute(VMContext& context) {
    unsigned char val1 = context.getRegisterValue(m_dest);
    unsigned char val2 = getSourceValue(context, m_flag, m_src, m_dest);

    uint16_t result = (uint16_t)val1 * (uint16_t)val2;
    context.setRegisterValue(m_dest, (unsigned char)result);
    // 명세에 MUL 플래그 정의 없으면 그대로 둠
}

// --- 5. CMP ---
void OpCMP::execute(VMContext& context) {
    unsigned char val1 = context.getRegisterValue(m_dest);
    unsigned char val2 = getSourceValue(context, m_flag, m_src, m_dest);

    int16_t s_val1 = (int8_t)val1;
    int16_t s_val2 = (int8_t)val2;
    int16_t sub_result = s_val1 - s_val2;

    context.setRegisterValue(VMDefs::REG_ZF, (sub_result == 0) ? 1 : 0);
    context.setRegisterValue(VMDefs::REG_CF, (sub_result >= 1) ? 1 : 0);
    context.setRegisterValue(VMDefs::REG_OF, (sub_result <= -1) ? 1 : 0);
}

// --- 6. PUSH ---
void OpPUSH::execute(VMContext& context) {
    if (m_flag == VMDefs::FLAG_REG_ONLY) { // 레지스터 값 PUSH
        unsigned char value = context.getRegisterValue(m_dest);
        context.pushStack(value);
    }
    else if (m_flag == VMDefs::FLAG_VAL_ONLY) { // 즉값 PUSH
        context.pushStack(m_dest);
    }
}

// --- 7. POP ---
void OpPOP::execute(VMContext& context) {
    unsigned char value = context.popStack();
    context.setRegisterValue(m_dest, value);
}

// --- 8. JMP ---
void OpJMP::execute(VMContext& context) {
    unsigned char new_pc = getSourceValue(context, m_flag, m_src, m_dest);
    context.setRegisterValue(VMDefs::REG_PC, new_pc);
}

// --- 9. BE (branch if equal, ZF==1) ---
void OpBE::execute(VMContext& context) {
    if (context.getRegisterValue(VMDefs::REG_ZF) == 1) {
        unsigned char new_pc = getSourceValue(context, m_flag, m_src, m_dest);
        context.setRegisterValue(VMDefs::REG_PC, new_pc);
    }
}

// --- 10. BNE (branch if not equal, ZF!=1) ---
void OpBNE::execute(VMContext& context) {
    if (context.getRegisterValue(VMDefs::REG_ZF) != 1) {
        unsigned char new_pc = getSourceValue(context, m_flag, m_src, m_dest);
        context.setRegisterValue(VMDefs::REG_PC, new_pc);
    }
}

// --- 11. PRINT ---
void OpPRINT::execute(VMContext& ctx) {
    if (m_flag == VMDefs::FLAG_VAL_ONLY) {       // 즉값 출력
        std::cout << (int)m_dest << std::endl;
    }
    else if (m_flag == VMDefs::FLAG_REG_ONLY) {  // 레지스터 값 출력
        std::cout << (int)ctx.getRegisterValue(m_dest) << std::endl;
    }
}
