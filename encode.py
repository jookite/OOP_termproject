import sys

instructions = {
    "MOV"   : 0b000001, 
    "ADD"   : 0b000010,
    "SUB"   : 0b000011,
    "MUL"   : 0b000100,
    "CMP"   : 0b000101,
    "PUSH"  : 0b000110,
    "POP"   : 0b000111,
    "JMP"   : 0b001000,
    "BE"    : 0b001001,
    "BNE"   : 0b001010,
    "PRINT" : 0b001011
}

registers = {
    "R0" : 0b00000001,
    "R1" : 0b00000010,
    "R2" : 0b00000011,
    "PC" : 0b00000100,
    "SP" : 0b00000101,
    "BP" : 0b00000110,
    "ZF" : 0b00000111,
    "CF" : 0b00001000,
    "OF" : 0b00001001,
}

flags = {
    "bothReg" : 0b00,
    "bothIMM" : 0b01,
    "oneReg"  : 0b10,
    "oneIMM"  : 0b11
}

def decode_instruction(instruction):
    temp = instruction.replace(",", "").replace('\n', '').split(" ")
    src = None
    dest = None
    flag = None

    if len(temp) == 2: # 단일 operand만을 받는 명령어
        opcode = temp[0]
        dest = temp[1]
        if dest in registers:
            flag = flags["oneReg"]
        elif dest not in registers:
            flag = flags["oneIMM"]
        else:
            raise ValueError("Invalid operand")
    elif len(temp) == 3: # 두 개의 operand를 받는 명령어
        opcode = temp[0]
        dest = temp[1]
        src = temp[2]
        if src in registers and dest in registers: 
            flag = flags["bothReg"]
        elif src not in registers and dest in registers: 
            flag = flags["bothIMM"]
        else:
            raise ValueError("Invalid operands")
    else:
        raise ValueError("Invalid instruction format")
    
    return opcode, src, dest, flag

def main():
    if sys.argv.__len__() != 3:
        print("Usage: python3 encode.py <inputfile> <outputfile>")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    out = open(output_file, "wb")
    with open(input_file, "r") as f:
        while True:
            line = f.readline()
            if not line:  # EOF
                break
            opcode, src, dest, flag = decode_instruction(line)
            out.write(bytes([instructions[opcode] << 2 | flag])) # 6 + 2bit
            out.write(bytes([0b00000000])) # 8bit, reserved
            if flag == flags["bothReg"]:
                out.write(bytes([registers[src]])) # 8bit, src
                out.write(bytes([registers[dest]])) # 8bit, dest
            elif flag == flags["bothIMM"]:
                out.write(bytes([int(src)])) # 8bit, src
                out.write(bytes([registers[dest]])) # 8bit, dest
            elif flag == flags["oneReg"]:
                out.write(bytes([0b00000000])) # 8bit, src
                out.write(bytes([registers[dest]])) # 8bit, dest
            elif flag == flags["oneIMM"]:
                out.write(bytes([0b00000000])) # 8bit, src
                out.write(bytes([int(dest)])) # 8bit, dest

if __name__ == "__main__":
    main()