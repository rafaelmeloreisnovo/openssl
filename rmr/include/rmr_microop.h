/*
 * Copyright 2026 Rafael Melo Reis.
 * See the repository LICENSE.txt.
 *
 * Sixteen-slot, caller-owned micro-op block. It is a deterministic semantic
 * lowering layer; it does not claim sixteen physical instructions per cycle.
 */
#ifndef RMR_MICROOP_H
#define RMR_MICROOP_H

#include "rmr_module.h"

#define RMR_UOP_LANES 16u
#define RMR_UOP_REGS 16u
#define RMR_UOP_STACK 16u

#define RMR_UOP_F_IMM 0x01u
#define RMR_UOP_F_JNZ 0x02u

enum rmr_uop_code {
    RMR_UOP_NOP = 0,
    RMR_UOP_MOV = 1,
    RMR_UOP_XOR = 2,
    RMR_UOP_OR = 3,
    RMR_UOP_AND = 4,
    RMR_UOP_ADD = 5,
    RMR_UOP_ROTR = 6,
    RMR_UOP_SHR = 7,
    RMR_UOP_PATCH = 8,
    RMR_UOP_PUSH = 9,
    RMR_UOP_PULL = 10,
    RMR_UOP_CH = 11,
    RMR_UOP_MAJ = 12,
    RMR_UOP_BSIG0 = 13,
    RMR_UOP_BSIG1 = 14,
    RMR_UOP_JUMP = 15
};

struct rmr_uop {
    rmr_u8 op;
    rmr_u8 dst;
    rmr_u8 src;
    rmr_u8 aux;
    rmr_u8 flags;
    rmr_u8 reserved0;
    rmr_u8 reserved1;
    rmr_u8 reserved2;
    rmr_u32 imm;
};

struct rmr_uop_state {
    rmr_u32 reg[RMR_UOP_REGS];
    rmr_u32 stack[RMR_UOP_STACK];
    rmr_u32 pc;
    rmr_u32 sp;
    rmr_u32 fault_mask;
    rmr_u32 fold;
};

struct rmr_uop_receipt {
    rmr_u32 executed_slots;
    rmr_u32 changed_regs;
    rmr_u32 changed_stack;
    rmr_u32 jump_count;
    rmr_u32 fault_mask;
    rmr_u32 final_fold;
};

rmr_u32 rmr_patch32(rmr_u32 current, rmr_u32 value, rmr_u32 mask);
rmr_u32 rmr_sha256_ch32(rmr_u32 x, rmr_u32 y, rmr_u32 z);
rmr_u32 rmr_sha256_maj32(rmr_u32 x, rmr_u32 y, rmr_u32 z);
rmr_u32 rmr_sha256_bsig0(rmr_u32 x);
rmr_u32 rmr_sha256_bsig1(rmr_u32 x);
void rmr_uop_state_init(struct rmr_uop_state *state);
void rmr_uop_run16(
    struct rmr_uop_state *state,
    const struct rmr_uop block[RMR_UOP_LANES],
    rmr_u32 active_mask,
    struct rmr_uop_receipt *receipt);
const struct rmr_module_descriptor *rmr_uop_module_descriptor(void);

#endif
