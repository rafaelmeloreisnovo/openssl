/* SPDX-FileCopyrightText: 2026 Rafael Melo Reis
 * SPDX-License-Identifier: MIT
 * Fixed-size versioned transport, no integrity/authentication claim.
 */
#include "../include/raf_fs.h"

/* A fixed 24-byte record: magic + five 32-bit state words. */
static void raf_fs_store32(raf_fs_u8 *p, raf_fs_u32 v)
{
    p[0] = (raf_fs_u8)v;
    p[1] = (raf_fs_u8)(v >> 8u);
    p[2] = (raf_fs_u8)(v >> 16u);
    p[3] = (raf_fs_u8)(v >> 24u);
}

static raf_fs_u32 raf_fs_load32(const raf_fs_u8 *p)
{
    return ((raf_fs_u32)p[0])
        | ((raf_fs_u32)p[1] << 8u)
        | ((raf_fs_u32)p[2] << 16u)
        | ((raf_fs_u32)p[3] << 24u);
}

static raf_fs_u32 raf_fs_valid(const raf_fs_state *state)
{
    return (state->undo_valid <= 1u
            && (state->active & ~state->allowed) == 0u
            && (state->previous & ~state->allowed) == 0u
            && (state->undo_valid == 0u || state->generation != 0u));
}

raf_fs_u32 raf_fs_encode(const raf_fs_state *state, void *dst,
                        raf_fs_u32 capacity)
{
    raf_fs_u8 *p = (raf_fs_u8 *)dst;
    if (state == (const void *)0 || p == (void *)0
        || capacity < RAF_FS_WIRE_BYTES || !raf_fs_valid(state))
        return 0u;
    raf_fs_store32(p, RAF_FS_WIRE_MAGIC);
    raf_fs_store32(p + 4u, state->allowed);
    raf_fs_store32(p + 8u, state->active);
    raf_fs_store32(p + 12u, state->previous);
    raf_fs_store32(p + 16u, state->generation);
    raf_fs_store32(p + 20u, state->undo_valid);
    return RAF_FS_WIRE_BYTES;
}

raf_fs_u32 raf_fs_decode(raf_fs_state *state, const void *src,
                        raf_fs_u32 length)
{
    const raf_fs_u8 *p = (const raf_fs_u8 *)src;
    raf_fs_state next;
    if (state == (void *)0 || p == (const void *)0
        || length != RAF_FS_WIRE_BYTES
        || raf_fs_load32(p) != RAF_FS_WIRE_MAGIC)
        return 0u;
    next.allowed = raf_fs_load32(p + 4u);
    next.active = raf_fs_load32(p + 8u);
    next.previous = raf_fs_load32(p + 12u);
    next.generation = raf_fs_load32(p + 16u);
    next.undo_valid = raf_fs_load32(p + 20u);
    if (!raf_fs_valid(&next))
        return 0u;
    /* Commit after all field checks: invalid packet leaves state intact. */
    state->allowed = next.allowed;
    state->active = next.active;
    state->previous = next.previous;
    state->generation = next.generation;
    state->undo_valid = next.undo_valid;
    return 1u;
}
