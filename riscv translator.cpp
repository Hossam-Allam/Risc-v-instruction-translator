#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdint>
#include <fstream>
#include <stdint.h>
#include <cstdlib>



struct RISCVInstruction {
    uint32_t opcode;
    uint32_t funct3;
    uint32_t funct7;
    uint32_t rs1;
    uint32_t rs2;
    uint32_t rd;
    std::string mnemonic;
};

RISCVInstruction instruction_table[] = {
    {0x13, 0x0, 0x0, 1, 0, 2, "addi"},
    {0x13, 0x0, 0x0, 1, 2, 1, "addi"},
    {0x33, 0x0, 0x0, 1, 2, 3, "add"},
    {0x37, 0x0, 0x0, 0, 0, 0, "lui"},
    {0x17, 0x0, 0x0, 0, 0, 0, "auipc"},
    {0x6f, 0x0, 0x0, 0, 0, 0, "jal"},
    {0x67, 0x0, 0x0, 0, 0, 0, "jalr"},
    {0x63, 0x0, 0x0, 0, 0, 0, "beq"},
    {0x63, 0x1, 0x0, 0, 0, 0, "bne"},
    {0x63, 0x4, 0x0, 0, 0, 0, "blt"},
    {0x63, 0x5, 0x0, 0, 0, 0, "bge"},
    {0x63, 0x6, 0x0, 0, 0, 0, "bltu"},
    {0x63, 0x7, 0x0, 0, 0, 0, "bgeu"},
    {0x03, 0x0, 0x0, 0, 0, 0, "lb"},
    {0x03, 0x1, 0x0, 0, 0, 0, "lh"},
    {0x03, 0x2, 0x0, 0, 0, 0, "lw"},
    {0x03, 0x4, 0x0, 0, 0, 0, "lbu"},
    {0x03, 0x5, 0x0, 0, 0, 0, "lhu"},
    {0x23, 0x0, 0x0, 0, 0, 0, "sb"},
    {0x23, 0x1, 0x0, 0, 0, 0, "sh"},
    {0x23, 0x2, 0x0, 0, 0, 0, "sw"},
    {0x13, 0x2, 0x0, 0, 0, 0, "slti"},
    {0x13, 0x3, 0x0, 0, 0, 0, "sltiu"},
    {0x13, 0x4, 0x0, 0, 0, 0, "xori"},
    {0x13, 0x6, 0x0, 0, 0, 0, "ori"},
    {0x13, 0x7, 0x0, 0, 0, 0, "andi"},
    {0x13, 0x1, 0x0, 0, 0, 0, "slli"},
    {0x13, 0x5, 0x0, 0, 0, 0, "srli"},
    {0x13, 0x5, 0x20, 0, 0, 0, "srai"},
    {0x33, 0x0, 0x0, 0, 0, 0, "add"},
    {0x33, 0x0, 0x20, 0, 0, 0, "sub"},
    {0x33, 0x1, 0x0, 0, 0, 0, "sll"},
    {0x33, 0x2, 0x0, 0, 0, 0, "slt"},
    {0x33, 0x3, 0x0, 0, 0, 0, "sltu"},
    {0x33, 0x4, 0x0, 0, 0, 0, "xor"},
    {0x33, 0x5, 0x0, 0, 0, 0, "srl"},
    {0x33, 0x5, 0x20, 0, 0, 0, "sra"},
    {0x33, 0x6, 0x0, 0, 0, 0, "or"},
    {0x33, 0x7, 0x0, 0, 0, 0, "and"},

};

void disassemble_riscv_instruction(uint32_t binary_instruction, int instruction_number) {
    // extract fields from binary_instruction
     uint32_t opcode = binary_instruction & 0x7F;
    uint32_t funct3 = 0;
    uint32_t funct7 = 0;
    if (opcode == 0x33 || opcode == 0x3B || opcode==0x13 ) {
        
        funct7 = (binary_instruction >> 25) & 0x7F;
    }

   

    
  
    if(opcode ==0x33 || opcode == 0x3B || opcode == 0x03 || opcode == 0x23 || opcode == 0x63 || opcode == 0x13)
{
  funct3 = (binary_instruction >> 12) & 0x7; 
}
uint32_t rs1 = (binary_instruction >> 15) & 0x1F;
uint32_t rs2 = (binary_instruction >> 20) & 0x1F;
uint32_t rd = (binary_instruction >> 7) & 0x1F;
int32_t imm;
if (opcode == 0x03 || opcode == 0x13 || opcode == 0x1B || opcode == 0x67 || opcode == 0x73) {
    // I-type or U-type instruction
    imm = ((int32_t)binary_instruction >> 20) & 0xFFF;
    if (imm & 0x800) { // check if the immediate value is negative
        imm |= 0xFFFFF000; // sign-extend the immediate value
    }
}
    else if (opcode == 0x37 || opcode == 0x17 ) {
    // U-type instruction (lui and auipc)
    imm = ((int32_t)binary_instruction & 0xFFFFF000);
}
    

    else if (opcode == 0x6F ) {
    // J-type instruction (jal)
    uint32_t imm_20 = (binary_instruction >> 31) & 0x1;
    uint32_t imm_10_1 = (binary_instruction >> 21) & 0x3FF;
    uint32_t imm_11 = (binary_instruction >> 20) & 0x1;
    uint32_t imm_19_12 = (binary_instruction >> 12) & 0xFF;
    imm = (imm_20 << 20) | (imm_19_12 << 12) | (imm_11 << 11) | (imm_10_1 << 1);
    if (imm_20 == 1) { // If imm[20] is 1, then the immediate value is negative (two's complement notation)
        imm = (0xFFF00000 | imm); // Sign-extend the 21-bit immediate value to 32 bits
    }
}

    else if (opcode == 0x63 || opcode == 0x23) {
    // B-type instruction (beq, bne, blt, bge, bltu, bgeu), S-type instruction (sb, sh, sw)
    uint32_t imm_12 = (binary_instruction >> 31) & 0x1;
    uint32_t imm_10_5 = (binary_instruction >> 25) & 0x3F;
    uint32_t imm_4_1 = (binary_instruction >> 8) & 0xF;
    uint32_t imm_11 = (binary_instruction >> 7) & 0x1;
    imm = (imm_12 << 12) | (imm_11 << 11) | (imm_10_5 << 5) | (imm_4_1 << 1);
    if (imm_12 == 1) { // If imm[12] is 1, then the immediate value is negative (two's complement notation)
        imm = (0xFFFFF000 | imm); // Sign-extend the 13-bit immediate value to 32 bits
    }
}

    
  
    for (int i = 0; i < sizeof(instruction_table)/sizeof(RISCVInstruction); i++) {
        RISCVInstruction& inst = instruction_table[i];
        if (inst.opcode == opcode && inst.funct3 == funct3 ) {
            // check if instruction is R-type
            if (opcode == 0x33 || opcode == 0x3B ||( inst.opcode ==0x13 && funct3== 0x5) ) {
                // check if instruction has correct funct7 field
                if (inst.funct7 == funct7) {
                  if(inst.mnemonic == "srli" || inst.mnemonic == "srai")
                  {
                    printf("inst %d: %08x %s x%d, x%d, %d\n", instruction_number-1, binary_instruction, inst.mnemonic.c_str(), rd, rs1, rs2);
                  }
                  else
                  {
                    printf("inst %d: %08x %s x%d, x%d, x%d\n", instruction_number-1, binary_instruction, inst.mnemonic.c_str(), rd, rs1, rs2);
                  }
                    return;
                }
              
            } 

            
            
            
            else {
                if (inst.mnemonic == "addi" || inst.mnemonic == "slti" || inst.mnemonic == "sltiu" || inst.mnemonic == "xori" || inst.mnemonic == "ori" || inst.mnemonic == "andi" || inst.mnemonic == "slli" || inst.mnemonic == "srli" || inst.mnemonic == "srai" ) {
                printf("inst %d: %08x %s x%d, x%d, %d\n", instruction_number-1, binary_instruction, inst.mnemonic.c_str(), rd, rs1, imm);
            }
            else if(inst.mnemonic == "beq" || inst.mnemonic == "bne" || inst.mnemonic == "blt" || inst.mnemonic == "bge" || inst.mnemonic == "bgeu" || inst.mnemonic == "bltu") 
            {
              printf("inst %d: %08x %s x%d, x%d, %d\n", instruction_number-1, binary_instruction, inst.mnemonic.c_str(), rs1, rs2, imm);
            }
            else if(inst.mnemonic == "jalr" || inst.mnemonic == "lh" || inst.mnemonic == "lb" || inst.mnemonic == "lw" || inst.mnemonic == "lbu" || inst.mnemonic == "lhu" || inst.mnemonic == "sb" || inst.mnemonic == "sh" || inst.mnemonic == "sw")
            {
              printf("inst %d: %08x %s x%d, %d(x%d)\n", instruction_number-1, binary_instruction, inst.mnemonic.c_str(), rd, imm, rs1);
            }
            else if(inst.mnemonic == "jal" || inst.mnemonic == "lui" || inst.mnemonic == "auipc")
            {
              printf("inst %d: %08x %s x%d, %d\n", instruction_number-1, binary_instruction, inst.mnemonic.c_str(), rd, imm);
            }
                return;
            }
        }
    }
    // instruction not found in table
    printf("unknown instruction\n");
}


int main(int argc, char** argv) {
    std::vector<uint32_t> instructions;

    // check for command line argument
    if (argc < 2) {
        std::cerr << "Error: Please specify a binary file as a command line argument." << std::endl;
        return 1;
    }

    // read input binary code from file
    std::ifstream binary_file(argv[1], std::ios::binary);
    if (!binary_file) {
        std::cerr << "Error: Unable to open binary file." << std::endl;
        return 1;
    }

    uint32_t instruction;
    while (binary_file.read(reinterpret_cast<char*>(&instruction), sizeof(instruction))) {
        instructions.push_back(instruction);
    }

    // disassemble each instruction in turn
    for (int i = 0; i < instructions.size(); i++) {
        disassemble_riscv_instruction(instructions[i], i+1);
    }

    return 0;
}