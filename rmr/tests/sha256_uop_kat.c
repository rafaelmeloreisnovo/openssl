/*
 * Copyright 2026 Rafael Melo Reis.
 * See the repository LICENSE.txt.
 */
#include "../include/rmr_sha256.h"
#include "../include/rmr_microop.h"

static int bytes_equal(const rmr_u8 *a, const rmr_u8 *b, rmr_u32 len)
{
    rmr_u32 index;
    rmr_u32 diff = 0u;

    for (index = 0u; index < len; ++index) {
        diff |= (rmr_u32)(a[index] ^ b[index]);
    }
    return diff == 0u;
}

int main(void)
{
    static const rmr_u8 expected_empty[32] = {
        0xe3,0xb0,0xc4,0x42,0x98,0xfc,0x1c,0x14,
        0x9a,0xfb,0xf4,0xc8,0x99,0x6f,0xb9,0x24,
        0x27,0xae,0x41,0xe4,0x64,0x9b,0x93,0x4c,
        0xa4,0x95,0x99,0x1b,0x78,0x52,0xb8,0x55
    };
    static const rmr_u8 expected_abc[32] = {
        0xba,0x78,0x16,0xbf,0x8f,0x01,0xcf,0xea,
        0x41,0x41,0x40,0xde,0x5d,0xae,0x22,0x23,
        0xb0,0x03,0x61,0xa3,0x96,0x17,0x7a,0x9c,
        0xb4,0x10,0xff,0x61,0xf2,0x00,0x15,0xad
    };
    static const char long_message[] =
        "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    static const rmr_u8 expected_long[32] = {
        0x24,0x8d,0x6a,0x61,0xd2,0x06,0x38,0xb8,
        0xe5,0xc0,0x26,0x93,0x0c,0x3e,0x60,0x39,
        0xa3,0x3c,0xe4,0x59,0x64,0xff,0x21,0x67,
        0xf6,0xec,0xed,0xd4,0x19,0xdb,0x06,0xc1
    };
    struct rmr_sha256_ctx sha_ctx;
    struct rmr_sha256_workspace workspace;
    rmr_u8 digest[32];
    struct rmr_uop block[16] = {{0}};
    struct rmr_uop_state state;
    struct rmr_uop_receipt receipt;
    const struct rmr_module_descriptor *sha_module;
    const struct rmr_module_descriptor *uop_module;

    rmr_sha256_digest((const void *)0, 0u, &workspace, digest);
    if (!bytes_equal(digest, expected_empty, 32u)) {
        return 1;
    }

    rmr_sha256_digest("abc", 3u, &workspace, digest);
    if (!bytes_equal(digest, expected_abc, 32u)) {
        return 2;
    }

    rmr_sha256_init(&sha_ctx);
    rmr_sha256_update(&sha_ctx, &workspace, "a", 1u);
    rmr_sha256_update(&sha_ctx, &workspace, "b", 1u);
    rmr_sha256_update(&sha_ctx, &workspace, "c", 1u);
    rmr_sha256_final(&sha_ctx, &workspace, digest);
    if (!bytes_equal(digest, expected_abc, 32u)) {
        return 3;
    }

    rmr_sha256_digest(
        long_message,
        (rmr_size)(sizeof(long_message) - 1u),
        &workspace,
        digest);
    if (!bytes_equal(digest, expected_long, 32u)) {
        return 4;
    }

    rmr_uop_state_init(&state);

    block[0].op = RMR_UOP_MOV;
    block[0].dst = 0u;
    block[0].flags = RMR_UOP_F_IMM;
    block[0].imm = 0x12345678u;

    block[1].op = RMR_UOP_XOR;
    block[1].dst = 0u;
    block[1].flags = RMR_UOP_F_IMM;
    block[1].imm = 0x00ff00ffu;

    block[2].op = RMR_UOP_PUSH;
    block[2].src = 0u;

    block[3].op = RMR_UOP_MOV;
    block[3].dst = 0u;
    block[3].flags = RMR_UOP_F_IMM;
    block[3].imm = 0u;

    block[4].op = RMR_UOP_PULL;
    block[4].dst = 1u;

    block[5].op = RMR_UOP_PATCH;
    block[5].dst = 1u;
    block[5].src = 0u;
    block[5].imm = 0x0000ff00u;

    block[6].op = RMR_UOP_JUMP;
    block[6].imm = 8u;

    block[7].op = RMR_UOP_MOV;
    block[7].dst = 2u;
    block[7].flags = RMR_UOP_F_IMM;
    block[7].imm = 0xdeadu;

    block[8].op = RMR_UOP_BSIG0;
    block[8].dst = 3u;
    block[8].src = 1u;

    block[9].op = RMR_UOP_JUMP;
    block[9].imm = 9u;

    rmr_uop_run16(&state, block, 0x03ffu, &receipt);

    if (state.reg[0] != 0u) {
        return 5;
    }
    if (state.reg[1] != 0x12cb0087u) {
        return 6;
    }
    if (state.reg[2] != 0u) {
        return 7;
    }
    if (receipt.jump_count == 0u || receipt.fault_mask != 0u) {
        return 8;
    }

    sha_module = rmr_sha256_module_descriptor();
    uop_module = rmr_uop_module_descriptor();

    if (sha_module->module_id != RMR_MODULE_ID_SHA256 ||
        rmr_module_descriptor_fold(sha_module) == 0u) {
        return 9;
    }
    if (uop_module->module_id != RMR_MODULE_ID_UOP16 ||
        rmr_module_descriptor_fold(uop_module) == 0u) {
        return 10;
    }

    return 0;
}
