/* SPDX-FileCopyrightText: 2026 Rafael Melo Reis
 * SPDX-License-Identifier: MIT
 * Independent deterministic control plane. Noncryptographic.
 */
#include "../include/raf_fs.h"

void raf_fs_init(raf_fs_state *state, raf_fs_u32 capabilities)
{
    if (state == (void *)0)
        return;
    state->allowed = capabilities;
    state->active = 0u;
    state->previous = 0u;
    state->generation = 0u;
    state->undo_valid = 0u;
}

raf_fs_u32 raf_fs_transition(raf_fs_state *state, raf_fs_u32 requested,
                             raf_fs_u32 expected_generation)
{
    if (state == (void *)0)
        return 0u;
    if (state->generation != expected_generation
        || state->generation == ~0u
        || (requested & ~state->allowed) != 0u)
        return 0u;
    state->previous = state->active;
    state->active = requested;
    state->generation += 1u;
    state->undo_valid = 1u;
    return 1u;
}

raf_fs_u32 raf_fs_rollback(raf_fs_state *state,
                           raf_fs_u32 expected_generation)
{
    raf_fs_u32 restored;
    if (state == (void *)0)
        return 0u;
    if (state->generation != expected_generation
        || state->generation == ~0u || state->undo_valid != 1u)
        return 0u;
    restored = state->previous;
    state->previous = state->active;
    state->active = restored;
    state->generation += 1u;
    state->undo_valid = 0u;
    return 1u;
}
