#ifndef VMCONTEXT_H
#define VMCONTEXT_H

#include <vector>
#include <map>
#include <string>
#include <cstdint> // uint32_t를 사용하기 위해
#include "Instruction.h" // Instruction 클래스와 상수 정의 포함

/**
 * @brief [Class] VMContext
 * 가상 머신의 실행 상태(레지스터, 스택, 코드)를 모두 관리합니다.
 * [Command Pattern] 'Receiver' (명령어의 실제 수신자) 및
 * 'Invoker' (runCode에서 명령 객체를 호출)의 역할을 수행합니다.
 */
class VMContext {
public:
    // 생성자: 모든 레지스터와 스택을 초기화합니다.
    VMContext();
    // 소멸자
    ~VMContext() {}

    // --- Core VM Operations ---
    /**
     * @brief lab{0-4}.bin 파일을 읽어 m_code 벡터에 저장
     */
    bool loadCode(const std::string& filename); //  (수정됨)

    /**
     * @brief m_code에 저장된 명령어를 하나씩 해석하고 실행
     */
    void runCode();

    // --- Register Access ---
    /**
     * @brief ID에 해당하는 레지스터 값을 반환
     */
    unsigned char getRegisterValue(unsigned char regID);

    /**
     * @brief ID에 해당하는 레지스터 값을 설정
     */
    void setRegisterValue(unsigned char regID, unsigned char value);

    // --- Stack Access ---
    /**
     * @brief 스택에 값을 Push하고 SP를 증가
     */
    void pushStack(unsigned char value);

    /**
     * @brief 스택에서 값을 Pop하고 SP를 감소
     */
    unsigned char popStack();

    /**
     * @brief 스택의 특정 인덱스 값을 반환 (PDF 요구사항)
     */
    unsigned char getStackValue(unsigned char index);

private:
    /**
     * @brief 32비트 명령어 코드를 파싱하여 적절한 Instruction 객체 생성
     *
     */
    Instruction* parseInstruction(uint32_t instructionWord);

    // 9개의 레지스터(R0~OF)를 map으로 관리 
    // ID(0x01 ~ 0x09)를 키로 사용하여 접근
    std::map<unsigned char, unsigned char> m_registers;

    // 스택 메모리 (8비트 SP, 최대 256바이트) 
    std::vector<unsigned char> m_stack;

    // 바이너리 파일에서 읽어온 원본 명령어 바이트 스트림
    std::vector<unsigned char> m_code;
};

#endif // VMCONTEXT_H