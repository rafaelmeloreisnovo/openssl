/*
 * Copyright 2026 Rafael Melo Reis.
 * See the repository LICENSE.txt.
 *
 * Freestanding SHA-256 with caller-owned state and workspace.
 */
#ifndef RMR_SHA256_H
#define RMR_SHA256_H

#include "rmr_module.h"

#define RMR_SHA256_BLOCK_BYTES 64u
#define RMR_SHA256_DIGEST_BYTES 32u
#define RMR_SHA256_SCHEDULE_WORDS 64u

struct rmr_sha256_ctx {
    rmr_u32 h[8];
    rmr_u64 total_bytes;
    rmr_u32 buffer_len;
    rmr_u8 buffer[RMR_SHA256_BLOCK_BYTES];
};

struct rmr_sha256_workspace {
    rmr_u32 schedule[RMR_SHA256_SCHEDULE_WORDS];
};

void rmr_sha256_init(struct rmr_sha256_ctx *ctx);
void rmr_sha256_update(
    struct rmr_sha256_ctx *ctx,
    struct rmr_sha256_workspace *workspace,
    const void *data,
    rmr_size len);
void rmr_sha256_final(
    struct rmr_sha256_ctx *ctx,
    struct rmr_sha256_workspace *workspace,
    rmr_u8 digest[RMR_SHA256_DIGEST_BYTES]);
void rmr_sha256_digest(
    const void *data,
    rmr_size len,
    struct rmr_sha256_workspace *workspace,
    rmr_u8 digest[RMR_SHA256_DIGEST_BYTES]);
const struct rmr_module_descriptor *rmr_sha256_module_descriptor(void);

#endif
