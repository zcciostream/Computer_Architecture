#include <stdio.h>
#include "shell.h"

#define funct_and 0b100100
#define funct_or 0b100101
#define funct_xor 0b100110
#define funct_nor 0b100111 
#define funct_sll 0b000000
#define funct_srl 0b000010
#define funct_sra 0b000011
#define funct_sllv 0b000100
#define funct_srlv 0b000110
#define funct_srav 0b000111
#define funct_mfhi 0b010000
#define funct_mflo 0b010010
#define funct_mthi 0b010001
#define funct_mtlo 0b010011
#define funct_add 0b100000
#define funct_addu 0b100001
#define funct_sub 0b100010
#define funct_subu 0b100011
#define funct_slt 0b101010
#define funct_sltu 0b101011
#define funct_div 0b011010
#define funct_divu 0b011011
#define funct_mult 0b011000
#define funct_multu 0b011001
#define funct_jr 0b001000
#define funct_jalr 0b001001
#define funct_syscall 0b001100

#define op_andi 0b001100
#define op_ori 0b001101
#define op_xori 0b001110
#define op_lui 0b001111
#define op_addi 0b001000
#define op_addiu 0b001001
#define op_slti 0b001010
#define op_sltiu 0b001011
#define op_beq 0b000100
#define op_bne 0b000101
#define op_bgtz 0b000111
#define op_blez 0b000110

#define op_bgez 0b000001  //
#define op_bltz 0b000001   // 
#define op_bltzal 0b000001 //
#define op_bgezal 0b000001 //
#define rt_bgez 0b00001
#define rt_bltz 0b00000
#define rt_bltzal 0b10000
#define rt_bgezal 0b10001

#define op_j 0b000010
#define op_jal 0b000011
#define op_lb 0b100000
#define op_lbu 0b100100
#define op_lh 0b100001
#define op_lhu  0b100101
#define op_lw 0b100011
#define op_sb 0b101000
#define op_sh 0b101001
#define op_sw 0b101011

void process_instruction()
{
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. */

    uint32_t pc = CURRENT_STATE.PC;
    NEXT_STATE = CURRENT_STATE;
    NEXT_STATE.PC = pc + 4;

    uint32_t inst = mem_read_32(pc);

    uint32_t op,rs,rt,rd,shamt,funct,imm,target;
    op = (inst & 0xfc000000) >> 26;
    rs = (inst & 0x3e00000) >> 21;
    rt = (inst & 0x1f0000) >> 16;
    rd = (inst & 0xf800) >> 11;
    shamt = (inst & 0x7c0) >> 6;
    funct = (inst & 0x3f);
    imm = inst & 0xffff;
    target = inst & 0x3ffffff;

    int32_t simm = ((imm & 0x8000) == 0x8000) ? (0xffff0000 | imm) : imm;
    
    switch(op){
        case 0b000000:
                uint32_t temp1 = CURRENT_STATE.REGS[rs];
                uint32_t temp2 = CURRENT_STATE.REGS[rt];
            switch(funct){
                case funct_and:
                    NEXT_STATE.REGS[rd] = temp1 & temp2;
                    break;
                case funct_or:
                    NEXT_STATE.REGS[rd] = temp1 | temp2;
                    break;
                case funct_xor:
                    NEXT_STATE.REGS[rd] = temp1 ^ temp2;
                    break;
                case funct_nor:
                    NEXT_STATE.REGS[rd] = ~(temp1 | temp2);
                    break;
                case funct_sll:
                    NEXT_STATE.REGS[rd] = temp2 << shamt;
                    break;
                case funct_srl:
                    NEXT_STATE.REGS[rd] = temp2 >> shamt;
                    break;
                case funct_sra:
                    NEXT_STATE.REGS[rd] = ((int32_t)temp2) >> shamt;
                    break;
                case funct_sllv:
                    NEXT_STATE.REGS[rd] = temp2 << temp1;
                    break;
                case funct_srlv:
                    NEXT_STATE.REGS[rd] = temp2 >> temp1;
                    break;
                case funct_srav:
                    NEXT_STATE.REGS[rd] = ((int32_t)temp2) >> temp1;
                    break;
                case funct_mfhi:
                    NEXT_STATE.REGS[rd] = CURRENT_STATE.HI;
                    break;
                case funct_mflo:
                    NEXT_STATE.REGS[rd] = CURRENT_STATE.LO;
                    break;
                case funct_mthi:
                    NEXT_STATE.HI = temp1;
                    break;
                case funct_mtlo:
                    NEXT_STATE.LO = temp1;
                    break;
                case funct_add:
                    NEXT_STATE.REGS[rd] = ((int32_t)temp1) + ((int32_t)temp2);
                    break;
                case funct_addu:
                    NEXT_STATE.REGS[rd] = (temp1 + temp2) & 0xFFFFFFFF;
                    break;
                case funct_sub:
                    NEXT_STATE.REGS[rd] = ((int32_t)temp1) - ((int32_t)temp2);
                    break;
                case funct_subu:
                    NEXT_STATE.REGS[rd] = temp1 - temp2;
                    break;
                case funct_slt:
                    NEXT_STATE.REGS[rd] = (((int32_t)temp1) < ((int32_t)temp2)) ? 1 : 0;
                    break;
                case funct_sltu:
                    NEXT_STATE.REGS[rd] = (temp1 < temp2) ? 1 : 0;
                    break;
                case funct_div:
                    NEXT_STATE.HI = ((int32_t)temp1) % ((int32_t)temp2);
                    NEXT_STATE.LO = ((int32_t)temp1) / ((int32_t)temp2);
                    break;
                case funct_divu:
                    NEXT_STATE.HI = temp1 % temp2;
                    NEXT_STATE.LO = temp1 / temp2;
                    break;
                case funct_mult:
                    int64_t result = ((int32_t)temp1) * ((int32_t)temp2);
                    NEXT_STATE.LO = result & 0x00000000FFFFFFFF;
                    NEXT_STATE.HI = result >> 32;
                    break;
                case funct_multu:
                    uint64_t result1 = temp1 * temp2;
                    NEXT_STATE.LO = result1 & 0x00000000FFFFFFFF;
                    NEXT_STATE.HI = result1 >> 32;
                    break;
                case funct_jr:
                    NEXT_STATE.PC = temp1;
                    break;
                case funct_jalr:
                    NEXT_STATE.REGS[rd] = CURRENT_STATE.PC + 8;
                    NEXT_STATE.PC = temp1;
                    break;
                case funct_syscall:
                    if(CURRENT_STATE.REGS[2]==10){
                        RUN_BIT = 0;
                    }
                    break;
            }
            break;
        case op_andi:
            NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] & imm;
            break;
        case op_ori:
            NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] | imm;
            break;
        case op_xori:
            NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] ^ imm;
            break;
        case op_lui:
            NEXT_STATE.REGS[rt] = 0x00000000 | (imm << 16);
            break;
        case op_addi:
            NEXT_STATE.REGS[rt] = (int32_t)CURRENT_STATE.REGS[rs] + simm;
            break;
        case op_addiu:
            NEXT_STATE.REGS[rt] = (int32_t)CURRENT_STATE.REGS[rs] + simm;
            break;
        case op_slti:
            NEXT_STATE.REGS[rt] = ((int32_t)CURRENT_STATE.REGS[rs] < simm) ? 1 : 0;
            break;
        case op_sltiu:
            NEXT_STATE.REGS[rt] = (CURRENT_STATE.REGS[rs] < (uint32_t)simm) ? 1 : 0;
            break;
        case op_beq:
            NEXT_STATE.PC = (CURRENT_STATE.REGS[rs]==CURRENT_STATE.REGS[rt]) ? (CURRENT_STATE.PC + (simm << 2)) : (CURRENT_STATE.PC + 4);
            break;
        case op_bne:
            NEXT_STATE.PC = (CURRENT_STATE.REGS[rs]!=CURRENT_STATE.REGS[rt]) ? (CURRENT_STATE.PC + (simm << 2)) : (CURRENT_STATE.PC + 4);
            break;
        case op_bgtz:
            NEXT_STATE.PC = ((int32_t)CURRENT_STATE.REGS[rs] > 0) ? (CURRENT_STATE.PC + (simm << 2)) : (CURRENT_STATE.PC + 4);
            break;
        case op_blez:
            NEXT_STATE.PC = ((int32_t)CURRENT_STATE.REGS[rs] <= 0) ? (CURRENT_STATE.PC + (simm << 2)) : (CURRENT_STATE.PC + 4);
            break;
        case 0b000001:
            switch (rt){
                case rt_bgez:
                    NEXT_STATE.PC = ((int32_t)CURRENT_STATE.REGS[rs] >= 0) ? (CURRENT_STATE.PC + (simm << 2)) : (CURRENT_STATE.PC + 4);
                    break;
                case rt_bltz:
                    NEXT_STATE.PC = ((int32_t)CURRENT_STATE.REGS[rs] < 0) ? (CURRENT_STATE.PC + (simm << 2)) : (CURRENT_STATE.PC + 4);
                    break;
                case rt_bltzal:
                    if((int32_t)CURRENT_STATE.REGS[rs] < 0){
                        NEXT_STATE.PC = CURRENT_STATE.PC + (simm << 2);
                        NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 8;
                    }
                    break;
                case rt_bgezal:
                    if((int32_t)CURRENT_STATE.REGS[rs] >= 0){
                        NEXT_STATE.PC = CURRENT_STATE.PC + (simm << 2);
                        NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 8;
                    }
                    break;
            }
            break;
        case op_j:
            NEXT_STATE.PC = (target << 2) | (CURRENT_STATE.PC & 0xF0000000);
            break;
        case op_jal:
            NEXT_STATE.PC = (target << 2) | (CURRENT_STATE.PC & 0xF0000000);
            NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 8;
            break;
        case op_lb:
            uint32_t temp3 = (mem_read_32(CURRENT_STATE.REGS[rs] + (simm)) & 0xFF);
            NEXT_STATE.REGS[rt] = ((temp3 & 0x80) == 0x80) ? (0xFFFFFF00 | temp3) : (temp3);
            break;
        case op_lbu:
            NEXT_STATE.REGS[rt] = (uint32_t)(mem_read_32(CURRENT_STATE.REGS[rs] + (simm)) & 0xFF);
            break;
        case op_lh:
            uint32_t temp4 = (mem_read_32(CURRENT_STATE.REGS[rs] + (simm)) & 0xFFFF);
            NEXT_STATE.REGS[rt] = ((temp4 & 0x8000) == 1) ? (0xFFFF0000 | temp4) : (temp4);
            break;
        case op_lhu:
            NEXT_STATE.REGS[rt] = (uint32_t)(mem_read_32(CURRENT_STATE.REGS[rs] + (simm)) & 0xFFFF);
            break;
        case op_lw:
            NEXT_STATE.REGS[rt] = mem_read_32(CURRENT_STATE.REGS[rs] + simm);
            break;
        case op_sb:
            mem_write_32((uint32_t)(CURRENT_STATE.REGS[rs] + simm), (CURRENT_STATE.REGS[rt] & 0XFF));
            break;
        case op_sh:
            mem_write_32((uint32_t)(CURRENT_STATE.REGS[rs] + simm), (CURRENT_STATE.REGS[rt] & 0xFFFF));
            break;
        case op_sw:
            mem_write_32((uint32_t)(CURRENT_STATE.REGS[rs] + simm), CURRENT_STATE.REGS[rt]);
            break;
    }
}
