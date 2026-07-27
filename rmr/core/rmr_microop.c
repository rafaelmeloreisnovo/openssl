/*
 * Copyright 2026 Rafael Melo Reis.
 * See the repository LICENSE.txt.
 */
#include "../include/rmr_microop.h"

#if defined(__GNUC__) || defined(__clang__)
#define RMR_UOP_META __attribute__((section(".rmr.modules"), used, aligned(64)))
#define RMR_UOP_NOTE __attribute__((section(".rmr.notes"), used, aligned(16)))
#else
#define RMR_UOP_META
#define RMR_UOP_NOTE
#endif

static const char g_rmr_uop_note[] RMR_UOP_NOTE =
    "uop16:caller-storage;fixed-16-steps;masked-alu;bounded-jump;selective-patch";

static const struct rmr_module_descriptor g_rmr_uop_module RMR_UOP_META = {
    RMR_MODULE_MAGIC,
    RMR_MODULE_ABI_VERSION,
    RMR_MODULE_ID_UOP16,
    RMR_MODULE_F_FREESTANDING | RMR_MODULE_F_NO_HEAP |
        RMR_MODULE_F_CALLER_STORAGE | RMR_MODULE_F_FIXED_STEPS |
        RMR_MODULE_F_SELECTIVE_PATCH | RMR_MODULE_F_BRANCHLESS_ALU |
        RMR_MODULE_F_ZERO_EXTERNAL | RMR_MODULE_F_BOUNDED_CONTROL,
    (rmr_u32)sizeof(struct rmr_uop_state),
    0u,
    RMR_UOP_LANES,
    0x3a91d5e7u
};

static rmr_u32 rmr_nonzero_u32(rmr_u32 value)
{
    value |= 0u - value;
    return value >> 31u;
}

rmr_u32 rmr_patch32(rmr_u32 current, rmr_u32 value, rmr_u32 mask)
{
    return current ^ ((current ^ value) & mask);
}

rmr_u32 rmr_sha256_ch32(rmr_u32 x, rmr_u32 y, rmr_u32 z)
{
    return (x & y) ^ (~x & z);
}

rmr_u32 rmr_sha256_maj32(rmr_u32 x, rmr_u32 y, rmr_u32 z)
{
    return (x & y) ^ (x & z) ^ (y & z);
}

rmr_u32 rmr_sha256_bsig0(rmr_u32 x)
{
    return rmr_rotr32(x, 2u) ^ rmr_rotr32(x, 13u) ^
        rmr_rotr32(x, 22u);
}

rmr_u32 rmr_sha256_bsig1(rmr_u32 x)
{
    return rmr_rotr32(x, 6u) ^ rmr_rotr32(x, 11u) ^
        rmr_rotr32(x, 25u);
}

void rmr_uop_state_init(struct rmr_uop_state *state)
{
    rmr_u32 index;

    if (state == (struct rmr_uop_state *)0) {
        return;
    }

    for (index = 0u; index < RMR_UOP_REGS; ++index) {
        state->reg[index] = 0u;
        state->stack[index] = 0u;
    }
    state->pc = 0u;
    state->sp = 0u;
    state->fault_mask = 0u;
    state->fold = 0x811c9dc5u;
}

void
rmr_uop_run16(struct rmr_uop_state *state,
              const struct rmr_uop block[RMR_UOP_LANES],
              rmr_u32 active_mask,
              struct rmr_uop_receipt *receipt)
{
    rmr_u32 step;
    rmr_u32 executed = 0u;
    rmr_u32 changed_regs = 0u;
    rmr_u32 changed_stack = 0u;
    rmr_u32 jumps = 0u;

    if (state == (struct rmr_uop_state *)0 ||
        block == (const struct rmr_uop *)0) {
        return;
    }

    for (step = 0u; step < RMR_UOP_LANES; ++step) {
        const rmr_u32 slot = state->pc & 15u;
        const struct rmr_uop *instruction = &block[slot];
        const rmr_u32 op = (rmr_u32)instruction->op;
        const rmr_u32 dst_index = (rmr_u32)instruction->dst & 15u;
        const rmr_u32 src_index = (rmr_u32)instruction->src & 15u;
        const rmr_u32 aux_index = (rmr_u32)instruction->aux & 15u;
        const rmr_u32 slot_enabled = (active_mask >> slot) & 1u;
        const rmr_u32 valid_op = rmr_ct_eq_u32(op >> 4u, 0u);
        const rmr_u32 execute_bit = slot_enabled & valid_op;
        const rmr_u32 execute_mask = rmr_ct_mask_u32(execute_bit);
        const rmr_u32 use_imm_mask = rmr_ct_mask_u32(
            ((rmr_u32)instruction->flags & RMR_UOP_F_IMM) != 0u);
        const rmr_u32 dst_old = state->reg[dst_index];
        const rmr_u32 src_reg = state->reg[src_index];
        const rmr_u32 src_value = rmr_ct_select_u32(
            use_imm_mask, instruction->imm, src_reg);
        const rmr_u32 aux_value = state->reg[aux_index];
        const rmr_u32 m_mov = rmr_ct_mask_u32(
            rmr_ct_eq_u32(op, RMR_UOP_MOV)) & execute_mask;
        const rmr_u32 m_xor = rmr_ct_mask_u32(
            rmr_ct_eq_u32(op, RMR_UOP_XOR)) & execute_mask;
        const rmr_u32 m_or = rmr_ct_mask_u32(
            rmr_ct_eq_u32(op, RMR_UOP_OR)) & execute_mask;
        const rmr_u32 m_and = rmr_ct_mask_u32(
            rmr_ct_eq_u32(op, RMR_UOP_AND)) & execute_mask;
        const rmr_u32 m_add = rmr_ct_mask_u32(
            rmr_ct_eq_u32(op, RMR_UOP_ADD)) & execute_mask;
        const rmr_u32 m_rotr = rmr_ct_mask_u32(
            rmr_ct_eq_u32(op, RMR_UOP_ROTR)) & execute_mask;
        const rmr_u32 m_shr = rmr_ct_mask_u32(
            rmr_ct_eq_u32(op, RMR_UOP_SHR)) & execute_mask;
        const rmr_u32 m_patch = rmr_ct_mask_u32(
            rmr_ct_eq_u32(op, RMR_UOP_PATCH)) & execute_mask;
        const rmr_u32 m_pull = rmr_ct_mask_u32(
            rmr_ct_eq_u32(op, RMR_UOP_PULL)) & execute_mask;
        const rmr_u32 m_ch = rmr_ct_mask_u32(
            rmr_ct_eq_u32(op, RMR_UOP_CH)) & execute_mask;
        const rmr_u32 m_maj = rmr_ct_mask_u32(
            rmr_ct_eq_u32(op, RMR_UOP_MAJ)) & execute_mask;
        const rmr_u32 m_bsig0 = rmr_ct_mask_u32(
            rmr_ct_eq_u32(op, RMR_UOP_BSIG0)) & execute_mask;
        const rmr_u32 m_bsig1 = rmr_ct_mask_u32(
            rmr_ct_eq_u32(op, RMR_UOP_BSIG1)) & execute_mask;
        const rmr_u32 m_push = rmr_ct_mask_u32(
            rmr_ct_eq_u32(op, RMR_UOP_PUSH)) & execute_mask;
        const rmr_u32 m_jump = rmr_ct_mask_u32(
            rmr_ct_eq_u32(op, RMR_UOP_JUMP)) & execute_mask;
        const rmr_u32 pull_valid = rmr_nonzero_u32(state->sp);
        const rmr_u32 push_valid = rmr_ct_eq_u32(state->sp >> 4u, 0u);
        const rmr_u32 pull_mask = m_pull & rmr_ct_mask_u32(pull_valid);
        const rmr_u32 push_mask = m_push & rmr_ct_mask_u32(push_valid);
        const rmr_u32 pull_index = (state->sp - pull_valid) & 15u;
        const rmr_u32 push_index = state->sp & 15u;
        const rmr_u32 pulled = state->stack[pull_index];
        rmr_u32 result = dst_old;
        rmr_u32 write_mask;
        rmr_u32 next_pc;
        rmr_u32 jump_condition;
        rmr_u32 jump_mask;
        rmr_u32 old_stack;
        rmr_u32 new_stack;

        result = rmr_ct_select_u32(m_mov, src_value, result);
        result = rmr_ct_select_u32(m_xor, dst_old ^ src_value, result);
        result = rmr_ct_select_u32(m_or, dst_old | src_value, result);
        result = rmr_ct_select_u32(m_and, dst_old & src_value, result);
        result = rmr_ct_select_u32(m_add, dst_old + src_value, result);
        result = rmr_ct_select_u32(
            m_rotr, rmr_rotr32(dst_old, src_value), result);
        result = rmr_ct_select_u32(
            m_shr, dst_old >> (src_value & 31u), result);
        result = rmr_ct_select_u32(
            m_patch,
            rmr_patch32(dst_old, src_value, instruction->imm),
            result);
        result = rmr_ct_select_u32(
            m_ch, rmr_sha256_ch32(dst_old, src_value, aux_value), result);
        result = rmr_ct_select_u32(
            m_maj, rmr_sha256_maj32(dst_old, src_value, aux_value), result);
        result = rmr_ct_select_u32(
            m_bsig0, rmr_sha256_bsig0(src_value), result);
        result = rmr_ct_select_u32(
            m_bsig1, rmr_sha256_bsig1(src_value), result);
        result = rmr_ct_select_u32(pull_mask, pulled, result);

        write_mask = m_mov | m_xor | m_or | m_and | m_add | m_rotr |
            m_shr | m_patch | m_pull | m_ch | m_maj | m_bsig0 | m_bsig1;
        state->reg[dst_index] = rmr_ct_select_u32(
            write_mask, result, dst_old);
        changed_regs |= (rmr_u32)(
            rmr_nonzero_u32(dst_old ^ state->reg[dst_index]) << dst_index);

        old_stack = state->stack[push_index];
        new_stack = rmr_ct_select_u32(push_mask, src_value, old_stack);
        state->stack[push_index] = new_stack;
        changed_stack |= (rmr_u32)(
            rmr_nonzero_u32(old_stack ^ new_stack) << push_index);

        old_stack = state->stack[pull_index];
        new_stack = rmr_ct_select_u32(pull_mask, 0u, old_stack);
        state->stack[pull_index] = new_stack;
        changed_stack |= (rmr_u32)(
            rmr_nonzero_u32(old_stack ^ new_stack) << pull_index);

        state->sp += push_valid & rmr_nonzero_u32(push_mask);
        state->sp -= pull_valid & rmr_nonzero_u32(pull_mask);

        jump_condition = rmr_nonzero_u32(src_value);
        jump_condition = rmr_ct_select_u32(
            rmr_ct_mask_u32(
                ((rmr_u32)instruction->flags & RMR_UOP_F_JNZ) != 0u),
            jump_condition,
            1u);
        jump_mask = m_jump & rmr_ct_mask_u32(jump_condition);
        next_pc = (state->pc + 1u) & 15u;
        state->pc = rmr_ct_select_u32(
            jump_mask, instruction->imm & 15u, next_pc);
        jumps += rmr_nonzero_u32(jump_mask);

        executed |= execute_bit << slot;
        state->fault_mask |= (rmr_u32)(
            (slot_enabled & (valid_op ^ 1u)) << slot);
        state->fault_mask |= (rmr_u32)(
            (rmr_nonzero_u32(m_push) & (push_valid ^ 1u)) << slot);
        state->fault_mask |= (rmr_u32)(
            (rmr_nonzero_u32(m_pull) & (pull_valid ^ 1u)) << slot);
        state->fold = rmr_mix32_ref(
            state->fold ^ op ^ slot ^ result ^ state->sp);
    }

    if (receipt != (struct rmr_uop_receipt *)0) {
        receipt->executed_slots = executed;
        receipt->changed_regs = changed_regs;
        receipt->changed_stack = changed_stack;
        receipt->jump_count = jumps;
        receipt->fault_mask = state->fault_mask;
        receipt->final_fold = state->fold;
    }
}

const struct rmr_module_descriptor *rmr_uop_module_descriptor(void)
{
    return &g_rmr_uop_module;
}
