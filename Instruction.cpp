#include "Instruction.h"
#include "VMContext.h"
#include <iostream> // PRINT 명령어를 위해
#include <cstdint> // int16_t, uint16_t를 위해

// --- 헬퍼 함수 선언 (파일 내부에서만 사용) ---
// Flag에 따라 Source 피연산자의 실제 값을 가져옵니다.
unsigned char getSourceValue(VMContext& context, unsigned char flag, unsigned char src) {
    // Flag 00 (Reg, Reg) 또는 10 (Reg Only)
    if (flag == VMDefs::FLAG_REG_REG || flag == VMDefs::FLAG_REG_ONLY) {
        return context.getRegisterValue(src); // src는 레지스터 ID
    }
    // Flag 01 (Reg, Val) 또는 11 (Val Only)
    if (flag == VMDefs::FLAG_REG_VAL || flag == VMDefs::FLAG_VAL_ONLY) {
        return src; // src는 값 자체
    }
    return 0; // 오류
}


// --- 1. OpMOV --- 
void OpMOV::execute(VMContext& context) {
    unsigned char value = 0;
    // Flag 00: Reg -> Reg
    if (m_flag == VMDefs::FLAG_REG_REG) {
        value = context.getRegisterValue(m_src);
    }
    // Flag 01: Val -> Reg
    else if (m_flag == VMDefs::FLAG_REG_VAL) {
        value = m_src; // m_src가 값(Value)
    }
    context.setRegisterValue(m_dest, value);
}

// --- 2. OpADD --- 
void OpADD::execute(VMContext& context) {
    unsigned char val1 = context.getRegisterValue(m_dest);
    unsigned char val2 = getSourceValue(context, m_flag, m_src);

    // 8비트 unsigned 덧셈의 Carry와 Overflow를 감지하기 위해 16비트 사용
    uint16_t result = (uint16_t)val1 + (uint16_t)val2;

    // 8비트 결과 저장
    context.setRegisterValue(m_dest, (unsigned char)result);

    // 플래그 설정
    // Zero Flag
    context.setRegisterValue(VMDefs::REG_ZF, ((unsigned char)result == 0) ? 1 : 0);
    // Carry Flag (8비트를 넘어 9비트로 자리올림 발생)
    context.setRegisterValue(VMDefs::REG_CF, (result > 255) ? 1 : 0);
    // Overflow Flag (부호있는 덧셈 기준)
    // (A+B)의 부호가 A, B와 다를 때. (A,B 양수 -> 결과 음수) or (A,B 음수 -> 결과 양수)
    int8_t s_val1 = (int8_t)val1;
    int8_t s_val2 = (int8_t)val2;
    int8_t s_result = (int8_t)result;
    if ((s_val1 > 0 && s_val2 > 0 && s_result < 0) || (s_val1 < 0 && s_val2 < 0 && s_result > 0)) {
        context.setRegisterValue(VMDefs::REG_OF, 1);
    } else {
        context.setRegisterValue(VMDefs::REG_OF, 0);
    }
}

// --- 3. OpSUB ---
void OpSUB::execute(VMContext& context) {
    unsigned char val1 = context.getRegisterValue(m_dest);
    unsigned char val2 = getSourceValue(context, m_flag, m_src);

    uint16_t result = (uint16_t)val1 - (uint16_t)val2; // 뺄셈

    context.setRegisterValue(m_dest, (unsigned char)result);

    // 플래그 설정 [cite: 45] (ADD와 유사)
    context.setRegisterValue(VMDefs::REG_ZF, ((unsigned char)result == 0) ? 1 : 0);
    // Carry Flag (Borrow와 동일, val1 < val2 일 때 발생)
    context.setRegisterValue(VMDefs::REG_CF, (val1 < val2) ? 1 : 0);
    // Overflow Flag
    int8_t s_val1 = (int8_t)val1;
    int8_t s_val2 = (int8_t)val2;
    int8_t s_result = (int8_t)result;
    if ((s_val1 > 0 && s_val2 < 0 && s_result < 0) || (s_val1 < 0 && s_val2 > 0 && s_result > 0)) {
        context.setRegisterValue(VMDefs::REG_OF, 1);
    } else {
        context.setRegisterValue(VMDefs::REG_OF, 0);
    }
}

// --- 4. OpMUL --- 
void OpMUL::execute(VMContext& context) {
    unsigned char val1 = context.getRegisterValue(m_dest);
    unsigned char val2 = getSourceValue(context, m_flag, m_src);

    uint16_t result = (uint16_t)val1 * (uint16_t)val2; // 곱셈

    // 8비트 결과 저장
    context.setRegisterValue(m_dest, (unsigned char)result);
    
    // PDF에 MUL의 플래그 설정 규칙이 없으므로 생략 (필요시 ADD/SUB처럼 추가)
}

// --- 5. OpCMP --- 
void OpCMP::execute(VMContext& context) {
    unsigned char val1 = context.getRegisterValue(m_dest);
    unsigned char val2 = getSourceValue(context, m_flag, m_src);

    // "내부적으로 SUB 연산"
    // PDF의 CMP 설명이 부호있는 정수(signed) 기준 (e.g., -1 이하)
    int16_t s_val1 = (int8_t)val1;
    int16_t s_val2 = (int8_t)val2;
    int16_t sub_result = s_val1 - s_val2;

    // 플래그 설정 (PDF 명세서 그대로 구현)
    // 값이 0이면 ZF를 1로
    context.setRegisterValue(VMDefs::REG_ZF, (sub_result == 0) ? 1 : 0);
    // 값이 1 이상이면 CF를 1로
    context.setRegisterValue(VMDefs::REG_CF, (sub_result >= 1) ? 1 : 0);
    // 값이 -1 이하면 OF를 1로
    context.setRegisterValue(VMDefs::REG_OF, (sub_result <= -1) ? 1 : 0);
}

// --- 6. OpPUSH --- 
void OpPUSH::execute(VMContext& context) {
    // Flag가 10(2진수) = 0x02. 레지스터의 '값'을 PUSH.
    if (m_flag == 0x02) {
        unsigned char value = context.getRegisterValue(m_dest);
        context.pushStack(value);
    }
    // Flag가 11(2진수) = 0x03. '값' 자체를 PUSH.
    else if (m_flag == 0x03) {
        
        context.pushStack(m_dest);
    }
}

// --- 7. OpPOP ---
void OpPOP::execute(VMContext& context) {
    unsigned char value = context.popStack();
    // Flag 10 (Reg Only) 
    context.setRegisterValue(m_dest, value);
}

// --- 8. OpJMP --- 
void OpJMP::execute(VMContext& context) {
    // Flag 10 (Reg) 또는 11 (Val) 
    unsigned char new_pc = getSourceValue(context, m_flag, m_src);
    context.setRegisterValue(VMDefs::REG_PC, new_pc);
}

// --- 9. OpBE --- 
void OpBE::execute(VMContext& context) {
    // "ZF가 1이라면 점프"
    if (context.getRegisterValue(VMDefs::REG_ZF) == 1) {
        unsigned char new_pc = getSourceValue(context, m_flag, m_src);
        context.setRegisterValue(VMDefs::REG_PC, new_pc);
    }
}

// --- 10. OpBNE --- 
void OpBNE::execute(VMContext& context) {
    // "ZF가 1이 아니라면 점프"
    if (context.getRegisterValue(VMDefs::REG_ZF) != 1) {
        unsigned char new_pc = getSourceValue(context, m_flag, m_src);
        context.setRegisterValue(VMDefs::REG_PC, new_pc);
    }
}

// --- 11. OpPRINT ---
void OpPRINT::execute(VMContext& ctx) {
    // Flag가 11 m_dest값 출력
    if (m_flag == 0x03) {
        std::cout << (int)m_dest << std::endl; 
    }
    // Flag가 10 '레지스터'의 값을 출력
    else if (m_flag == 0x02) {
        std::cout << (int)ctx.getRegisterValue(m_dest) << std::endl;
    }
}