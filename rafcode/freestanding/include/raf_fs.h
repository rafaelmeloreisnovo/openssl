/* SPDX-FileCopyrightText: 2026 Rafael Melo Reis
 * SPDX-License-Identifier: MIT
 * RAFCODE isolated freestanding control-plane core; NOT a cryptographic API.
 */
#ifndef RAFCODE_FS_H
#define RAFCODE_FS_H

/* No hosted headers: explicit, checked C11 ABI assumptions. */
typedef unsigned int raf_fs_u32;
typedef unsigned char raf_fs_u8;
_Static_assert(sizeof(raf_fs_u32) == 4, "RAFCODE needs a 32-bit unsigned int");
_Static_assert(((raf_fs_u8)~0u) == 255u, "RAFCODE needs 8-bit bytes");

#define RAF_FS_WIRE_BYTES 24u
#define RAF_FS_WIRE_MAGIC 0x52414631u /* "RAF1", little-endian bytes */

/* Non-atomic, single-owner state. Neither an OpenSSL provider nor a syscall. */
typedef struct raf_fs_state {
    raf_fs_u32 allowed;
    raf_fs_u32 active;
    raf_fs_u32 previous;
    raf_fs_u32 generation;
    raf_fs_u32 undo_valid;
} raf_fs_state;

/* Null pointer is accepted; a null call does not mutate memory. */
void raf_fs_init(raf_fs_state *state, raf_fs_u32 capabilities);

/* 1 = committed, 0 = denied; denied operations are side-effect free.
 * expected_generation prevents stale writes. No wrap-around allowed. */
raf_fs_u32 raf_fs_transition(raf_fs_state *state, raf_fs_u32 requested,
                             raf_fs_u32 expected_generation);

/* One-level compensating rollback; increments generation, forbids replay. */
raf_fs_u32 raf_fs_rollback(raf_fs_state *state,
                           raf_fs_u32 expected_generation);

/* Wire format is explicit little-endian, not sizeof(struct).
 * No allocation, no filesystem or OS, no zero-length success. */
raf_fs_u32 raf_fs_encode(const raf_fs_state *state, void *dst,
                        raf_fs_u32 capacity);
raf_fs_u32 raf_fs_decode(raf_fs_state *state, const void *src,
                        raf_fs_u32 length);

#endif
