#include <iostream>
#include <string>
#include "VMContext.h" // VMContext 클래스 헤더

/**
 * @brief 프로그램 메인 함수
 * @param argc 인자 개수
 * @param argv 인자 배열 (argv[0] = 프로그램 이름, argv[1] = 파일 경로)
 * @return int 
 */
int main(int argc, char* argv[]) { 
    // 1. 바이너리 파일 입력 확인 
    // 프로그램 실행 시 파일 이름이 주어지지 않으면 (argc < 2)
    if (argc < 2) {
        // "파일이 입력되지 않았음" 출력
        std::cerr << "출력: 파일이 입력되지 않았음" << std::endl;
        std::cerr << "사용법: " << argv[0] << " <lab_file.bin>" << std::endl;
        return 1; // 종료
    }

    std::string filename = argv[1];

    // 2. VMContext 객체 생성
    VMContext vm;

    // 3. 파일 로드 (파일 內 값에 따른 명령어 처리 - 준비)
    if (!vm.loadCode(filename)) {
        std::cerr << "오류: 파일을 열 수 없습니다 - " << filename << std::endl;
        return 1;
    }

    // 4. 코드 실행 (파일 內 값에 따른 명령어 처리 - 시작)
    vm.runCode();

    // 5. 종료
    return 0;
}