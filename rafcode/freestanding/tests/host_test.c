/* SPDX-FileCopyrightText: 2026 Rafael Melo Reis
 * SPDX-License-Identifier: MIT
 * Host-test only: checks structural behavior; not a crypto validation.
 */
#include "../include/raf_fs.h"
static raf_fs_u32 same(const raf_fs_state *a, const raf_fs_state *b)
{
    return a->allowed == b->allowed && a->active == b->active
        && a->previous == b->previous && a->generation == b->generation
        && a->undo_valid == b->undo_valid;
}
int main(void)
{
    raf_fs_state st, restored, snapshot, genesis;
    raf_fs_u8 raw[RAF_FS_WIRE_BYTES];
    raf_fs_u32 i;
    raf_fs_init((void *)0, 7u);
    if (raf_fs_transition((void *)0, 1u, 0u)) return 1;
    if (raf_fs_rollback((void *)0, 0u)) return 2;
    raf_fs_init(&st, 3u);
    if (st.generation != 0u || st.active != 0u || st.undo_valid) return 3;
    if (raf_fs_transition(&st, 4u, 0u)) return 4; /* unauthorized */
    if (st.generation != 0u || st.active != 0u) return 5;
    if (raf_fs_transition(&st, 1u, 1u)) return 6; /* stale */
    if (!raf_fs_transition(&st, 1u, 0u)) return 7;
    if (st.generation != 1u || st.active != 1u) return 8;
    if (!raf_fs_transition(&st, 3u, 1u)) return 9;
    if (st.generation != 2u || st.previous != 1u) return 10;
    if (raf_fs_rollback(&st, 1u)) return 11; /* stale rollback */
    if (!raf_fs_rollback(&st, 2u)) return 12;
    if (st.active != 1u || st.generation != 3u || st.undo_valid) return 13;
    if (raf_fs_rollback(&st, 3u)) return 14; /* replay */
    if (raf_fs_encode(&st, raw, 23u)) return 15;
    if (raf_fs_encode((const void *)0, raw, 24u)) return 16;
    if (raf_fs_encode(&st, (void *)0, 24u)) return 17;
    if (raf_fs_encode(&st, raw, 24u) != 24u) return 18;
    if (raw[0] != 0x31u || raw[1] != 0x46u
        || raw[2] != 0x41u || raw[3] != 0x52u) return 19;
    raf_fs_init(&restored, 3u);
    if (!raf_fs_decode(&restored, raw, 24u)) return 20;
    if (!same(&restored, &st)) return 21;
    snapshot = restored;
    raw[0] ^= 1u; /* corrupted magic */
    if (raf_fs_decode(&restored, raw, 24u)) return 22;
    raw[0] ^= 1u;
    if (!same(&restored, &snapshot)) return 23;
    raw[8] = 0x80u; /* active outside capabilities */
    if (raf_fs_decode(&restored, raw, 24u)) return 24;
    raw[8] = 1u;
    if (!same(&restored, &snapshot)) return 25;
    if (raf_fs_decode(&restored, raw, 23u)) return 26;
    /* Wire may not elevate receiver capability mask or rewind generation. */
    raw[4] = 7u;
    if (raf_fs_decode(&restored, raw, 24u)) return 34;
    raw[4] = 3u;
    if (!same(&restored, &snapshot)) return 35;
    raw[16] = 0u;
    if (raf_fs_decode(&restored, raw, 24u)) return 36;
    raw[16] = 3u;
    if (!same(&restored, &snapshot)) return 37;
    /* Same generation must be either identical (idempotent) or rejected.
     * Old code allowed an unchanged version to rewrite active/previous/undo. */
    raw[8] = 2u;
    if (raf_fs_decode(&restored, raw, 24u)) return 38;
    raw[8] = 1u;
    if (!same(&restored, &snapshot)) return 39;
    raw[12] = 2u;
    if (raf_fs_decode(&restored, raw, 24u)) return 40;
    raw[12] = 3u;
    if (!same(&restored, &snapshot)) return 41;
    raw[20] = 1u;
    if (raf_fs_decode(&restored, raw, 24u)) return 42;
    raw[20] = 0u;
    if (!same(&restored, &snapshot)) return 43;
    if (!raf_fs_decode(&restored, raw, 24u)) return 44;
    if (!same(&restored, &snapshot)) return 45;
    /* Genesis generation zero cannot be used to inject a state change. */
    raf_fs_init(&genesis, 3u);
    raw[16] = 0u;
    raw[8] = 1u;
    raw[12] = 0u;
    if (raf_fs_decode(&genesis, raw, 24u)) return 46;
    if (genesis.generation || genesis.active || genesis.previous) return 47;
    raw[8] = 0u;
    if (!raf_fs_decode(&genesis, raw, 24u)) return 48;
    if (genesis.generation || genesis.active || genesis.previous) return 49;
    for (i = 0u; i < 24u; ++i) raw[i] = 0u;
    if (raf_fs_decode(&restored, raw, 24u)) return 27;
    if (!same(&restored, &snapshot)) return 28;
    if (raf_fs_decode((void *)0, raw, 24u)) return 29;
    if (raf_fs_decode(&restored, (const void *)0, 24u)) return 30;
    st.generation = ~0u; /* prohibit generation wraparound */
    if (raf_fs_transition(&st, 1u, ~0u)) return 31;
    st.undo_valid = 1u;
    if (raf_fs_rollback(&st, ~0u)) return 32;
    if (st.generation != ~0u) return 33;
    return 0;
}
