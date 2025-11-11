#include "VMContext.h"
#include <iostream> // (std::cout, std::cerr, std::endl)
#include <iomanip>  // (std::hex, std::setw, std::setfill)
#include <fstream>  // (std::ifstream)
#include <bitset>   // <-- [추가] 2진수 출력을 위한 std::bitset

/**
 * [cite_start]@brief VMContext 생성자
 * 모든 레지스터와 256바이트 스택을 0으로 초기화합니다.
 */
VMContext::VMContext() {
    // 9개의 레지스터 모두 0으로 초기화
    m_registers[VMDefs::REG_R0] = 0;
    m_registers[VMDefs::REG_R1] = 0;
    m_registers[VMDefs::REG_R2] = 0;
    m_registers[VMDefs::REG_PC] = 0;
    m_registers[VMDefs::REG_SP] = 0; // SP는 0에서 시작 (stack[0]부터 채움)
    m_registers[VMDefs::REG_BP] = 0;
    m_registers[VMDefs::REG_ZF] = 0;
    m_registers[VMDefs::REG_CF] = 0;
    m_registers[VMDefs::REG_OF] = 0;

    // 256바이트 스택 공간 확보 (SP가 8비트이므로) 
    m_stack.resize(256, 0);
}

/**
 * @brief 바이너리 파일을 읽어 m_code에 로드
 */
bool VMContext::loadCode(const std::string& filename) {
    // 바이너리 모드(ios::binary)로 파일 열기
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl; // <-- std::cout -> std::cerr
        return false;
    }

    // 파일 크기 확인
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    // m_code 벡터 크기 조절
    m_code.resize(size);
    std::cout << typeid(m_code).name() << std::endl;

    // 파일 내용을 m_code 벡터로 한 번에 읽기
    if (!file.read((char*)m_code.data(), size)) {
        std::cerr << "Error: Could not read file " << filename << std::endl; // <-- std::cout -> std::cerr
        file.close();
        return false;
    }

    /*// --- [수정] 출력 루프 ---
    for (size_t i = 0; i < m_code.size(); ++i) {

        // 1. 16진수 출력
        std::cout << "Hex: " << std::hex << std::setfill('0') << std::setw(2)
            << static_cast<int>(m_code[i]);

        // 2. 8비트 2진수 출력
        std::cout << " | Binary: " << std::bitset<8>(m_code[i]) << " ";
        std::cout << std::endl;

        // 3. 4바이트(32비트 명령어) 단위로 줄바꿈
        if ((i + 1) % 4 == 0) {
            std::cout << std::endl;
        }
    }
    */// --- 여기까지 ---

    // 파일 크기가 4바이트 배수가 아닐 경우 마지막 줄바꿈 처리
    if (m_code.size() % 4 != 0) {
        std::cout << std::endl;
    }

    // [중요] std::cout 형식을 다시 10진수로 복원
    std::cout << std::dec;

    file.close();
    return true;
}

/**
 * @brief VM 실행 루프 (가장 핵심)
 */
void VMContext::runCode() {
    // PC(Program Counter) 레지스터에 대한 참조(alias)
    unsigned char& pc = m_registers[VMDefs::REG_PC];
    pc = 0; // 실행은 항상 0번 인덱스부터 시작

    // PC가 코드의 끝에 도달할 때까지 반복
    while ((pc * 4) + 4 <= m_code.size()) {

        // 1. Fetch (4바이트를 읽어 32비트 정수로 조합)
// 1. Fetch (4바이트를 읽어 32비트 정수로 조합)
        uint32_t instructionWord = 0;
        instructionWord |= (uint32_t)m_code[pc * 4 + 0] << 24;
        instructionWord |= (uint32_t)m_code[pc * 4 + 1] << 16;
        instructionWord |= (uint32_t)m_code[pc * 4 + 2] << 8;
        instructionWord |= (uint32_t)m_code[pc * 4 + 3] << 0;

        /*// --- [추가된 디버깅 코드] ---
        std::cout << "--- [DEBUG] Fetch at PC = " << (int)pc << " ---" << std::endl;
        std::cout << "  Byte 0 (m_code[" << pc * 4 + 0 << "]): " << std::hex << std::setw(2) << std::setfill('0')
            << (int)m_code[pc * 4 + 0] << " | " << std::bitset<8>(m_code[pc * 4 + 0]) << std::endl;
        std::cout << "  Byte 1 (m_code[" << pc * 4 + 1 << "]): " << std::hex << std::setw(2) << std::setfill('0')
            << (int)m_code[pc * 4 + 1] << " | " << std::bitset<8>(m_code[pc * 4 + 1]) << std::endl;
        std::cout << "  Byte 2 (m_code[" << pc * 4 + 2 << "]): " << std::hex << std::setw(2) << std::setfill('0')
            << (int)m_code[pc * 4 + 2] << " | " << std::bitset<8>(m_code[pc * 4 + 2]) << std::endl;
        std::cout << "  Byte 3 (m_code[" << pc * 4 + 3 << "]): " << std::hex << std::setw(2) << std::setfill('0')
            << (int)m_code[pc * 4 + 3] << " | " << std::bitset<8>(m_code[pc * 4 + 3]) << std::endl;
        std::cout << "  [!!] Combined (Little-Endian): 0x" << std::hex << std::setw(8) << std::setfill('0')
            << instructionWord << std::endl;
        std::cout << "  [!!] Combined (Binary):       " << std::bitset<8>(instructionWord) << std::endl;
        std::cout << "-----------------------------------" << std::endl;
        std::cout << std::dec;
        */// --- [디버깅 코드 끝] ---


        // JMP/BE/BNE 등으로 PC가 변경되었는지 확인하기 위해 현재 PC 저장
        unsigned char old_pc = pc;

        // 2. Decode (32비트 단어를 Instruction 객체로 파싱)
        Instruction* command = parseInstruction(instructionWord);

        // 3. Execute
        if (command != nullptr) {
            command->execute(*this); // [Command Pattern] 실행!
            delete command; // [중요] 동적 할당된 객체 해제 (메모리 누수 방지)
        }
        else {
            // 알 수 없는 Opcode...
            std::cerr << "Unknown opcode at PC=" << (int)pc << std::endl;
            break; // 실행 중지
        }

        // 4. PC Update
        if (pc == old_pc) {
            pc++; // 다음 명령어로 이동 (PC 1 증가)
        }
    }
}

/**
 * @brief [Factory Method] 32비트 명령어를 파싱하여 Command 객체 생성
 */
Instruction* VMContext::parseInstruction(uint32_t word) {
    // 2-5. [cite_start]명령어 해석 (비트 연산) 
    unsigned char opcode = (word >> 26) & 0x3F;// Opcode (6bit)
    unsigned char flag = (word >> 24) & 0x03; // Flag (2bit)
        unsigned char src = (word >> 8) & 0xFF; // Source (8bit)
            unsigned char dest = (word >> 0) & 0xFF; // Destination (8bit

                /*//--- [추가된 디버깅 코드] ---
                std::cout << "  --- [DEBUG] Parsing Instruction ---" << std::endl;
                std::cout << "    Full Word (Hex):    0x" << std::hex << std::setw(8) << std::setfill('0') << word << std::endl;
                std::cout << "    Full Word (Binary): " << std::bitset<8>(word) << std::endl;
                std::cout << "    ---------------------------------" << std::endl;
                // static_cast<int>는 unsigned char를 (문자가 아닌) 숫자로 출력하기 위함
                std::cout << "    Parsed Opcode: 0x" << std::hex << std::setw(2) << (int)opcode
                    << " (" << std::bitset<8>(opcode) << ")" << std::endl;
                std::cout << "    Parsed Flag:   " << std::bitset<8>(flag) << std::endl;
                std::cout << "    Parsed Source: 0x" << std::hex << std::setw(2) << (int)src
                    << " (Dec: " << std::dec << (int)src << ")" << std::endl;
                std::cout << "    Parsed Dest:   0x" << std::hex << std::setw(2) << (int)dest
                    << " (Dec: " << std::dec << (int)dest << ")" << std::endl;
                std::cout << "    ==> Entering switch to create object..." << std::endl;
                std::cout << "  -----------------------------------" << std::endl;
                std::cout << std::dec; // [중요] 출력 형식을 다시 10진수로 복원
                */// --- [디버깅 코드 끝] ---

                // Opcode에 따라 적절한 Command 객체를 생성(new)하여 반환 
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
                    return nullptr; // 알 수 없는 Opcode
                }
}

// --- 레지스터 및 스택 헬퍼 함수 구현 ---

unsigned char VMContext::getRegisterValue(unsigned char regID) {
    if (m_registers.count(regID)) {
        return m_registers[regID];
    }
    return 0;
}

void VMContext::setRegisterValue(unsigned char regID, unsigned char value) {
    if (m_registers.count(regID)) {
        m_registers[regID] = value;
    }
}

void VMContext::pushStack(unsigned char value) {
    unsigned char& sp = m_registers[VMDefs::REG_SP];
    m_stack[sp] = value; // SP가 가리키는 곳에 값 저장
    sp++; // SP 1 증가 (다음 빈 곳을 가리킴)
}

unsigned char VMContext::popStack() {
    unsigned char& sp = m_registers[VMDefs::REG_SP];
    sp--; // SP 1 감소 (마지막으로 넣은 값을 가리킴)
    return m_stack[sp];
}

unsigned char VMContext::getStackValue(unsigned char index) {
    if (index < m_stack.size()) {
        return m_stack[index];
    }
    return 0; // 범위 초과
}