/*
 * Copyright 2026 Rafael Melo Reis.
 * See the repository LICENSE.txt.
 *
 * Scalar SHA-256 reference path with caller-owned state and workspace.
 */
#include "../include/rmr_sha256.h"
#include "../include/rmr_microop.h"

#if defined(__GNUC__) || defined(__clang__)
#define RMR_SHA_META __attribute__((section(".rmr.modules"), used, aligned(64)))
#define RMR_SHA_NOTE __attribute__((section(".rmr.notes"), used, aligned(16)))
#define RMR_SHA_CONST __attribute__((section(".rmr.rodata.sha256"), aligned(64)))
#else
#define RMR_SHA_META
#define RMR_SHA_NOTE
#define RMR_SHA_CONST
#endif

static const char g_rmr_sha256_note[] RMR_SHA_NOTE =
    "sha256:fixed-rounds;caller-workspace;zero-heap;fixed-width;kat-required";

static const struct rmr_module_descriptor g_rmr_sha256_module RMR_SHA_META = {
    RMR_MODULE_MAGIC,
    RMR_MODULE_ABI_VERSION,
    RMR_MODULE_ID_SHA256,
    RMR_MODULE_F_FREESTANDING | RMR_MODULE_F_NO_HEAP |
        RMR_MODULE_F_CALLER_STORAGE | RMR_MODULE_F_BRANCHLESS_ALU |
        RMR_MODULE_F_ZERO_EXTERNAL | RMR_MODULE_F_BOUNDED_CONTROL,
    (rmr_u32)sizeof(struct rmr_sha256_ctx),
    (rmr_u32)sizeof(struct rmr_sha256_workspace),
    64u,
    0x8c4f7a21u
};

static const rmr_u32 g_rmr_sha256_k[64] RMR_SHA_CONST = {
    0x428a2f98u, 0x71374491u, 0xb5c0fbcfu, 0xe9b5dba5u,
    0x3956c25bu, 0x59f111f1u, 0x923f82a4u, 0xab1c5ed5u,
    0xd807aa98u, 0x12835b01u, 0x243185beu, 0x550c7dc3u,
    0x72be5d74u, 0x80deb1feu, 0x9bdc06a7u, 0xc19bf174u,
    0xe49b69c1u, 0xefbe4786u, 0x0fc19dc6u, 0x240ca1ccu,
    0x2de92c6fu, 0x4a7484aau, 0x5cb0a9dcu, 0x76f988dau,
    0x983e5152u, 0xa831c66du, 0xb00327c8u, 0xbf597fc7u,
    0xc6e00bf3u, 0xd5a79147u, 0x06ca6351u, 0x14292967u,
    0x27b70a85u, 0x2e1b2138u, 0x4d2c6dfcu, 0x53380d13u,
    0x650a7354u, 0x766a0abbu, 0x81c2c92eu, 0x92722c85u,
    0xa2bfe8a1u, 0xa81a664bu, 0xc24b8b70u, 0xc76c51a3u,
    0xd192e819u, 0xd6990624u, 0xf40e3585u, 0x106aa070u,
    0x19a4c116u, 0x1e376c08u, 0x2748774cu, 0x34b0bcb5u,
    0x391c0cb3u, 0x4ed8aa4au, 0x5b9cca4fu, 0x682e6ff3u,
    0x748f82eeu, 0x78a5636fu, 0x84c87814u, 0x8cc70208u,
    0x90befffau, 0xa4506cebu, 0xbef9a3f7u, 0xc67178f2u
};

static rmr_u32 rmr_load32_be(const rmr_u8 source[4])
{
    return ((rmr_u32)source[0] << 24u) |
        ((rmr_u32)source[1] << 16u) |
        ((rmr_u32)source[2] << 8u) |
        (rmr_u32)source[3];
}

static void rmr_store32_be(rmr_u8 destination[4], rmr_u32 value)
{
    destination[0] = (rmr_u8)(value >> 24u);
    destination[1] = (rmr_u8)(value >> 16u);
    destination[2] = (rmr_u8)(value >> 8u);
    destination[3] = (rmr_u8)value;
}

static rmr_u32 rmr_sha256_ssig0(rmr_u32 value)
{
    return rmr_rotr32(value, 7u) ^ rmr_rotr32(value, 18u) ^
        (value >> 3u);
}

static rmr_u32 rmr_sha256_ssig1(rmr_u32 value)
{
    return rmr_rotr32(value, 17u) ^ rmr_rotr32(value, 19u) ^
        (value >> 10u);
}

static void
rmr_sha256_compress(struct rmr_sha256_ctx *ctx,
                    struct rmr_sha256_workspace *workspace,
                    const rmr_u8 block[RMR_SHA256_BLOCK_BYTES])
{
    rmr_u32 a;
    rmr_u32 b;
    rmr_u32 c;
    rmr_u32 d;
    rmr_u32 e;
    rmr_u32 f;
    rmr_u32 g;
    rmr_u32 h;
    rmr_u32 index;

    for (index = 0u; index < 16u; ++index) {
        workspace->schedule[index] = rmr_load32_be(&block[index * 4u]);
    }
    for (index = 16u; index < 64u; ++index) {
        workspace->schedule[index] =
            rmr_sha256_ssig1(workspace->schedule[index - 2u]) +
            workspace->schedule[index - 7u] +
            rmr_sha256_ssig0(workspace->schedule[index - 15u]) +
            workspace->schedule[index - 16u];
    }

    a = ctx->h[0];
    b = ctx->h[1];
    c = ctx->h[2];
    d = ctx->h[3];
    e = ctx->h[4];
    f = ctx->h[5];
    g = ctx->h[6];
    h = ctx->h[7];

    for (index = 0u; index < 64u; ++index) {
        const rmr_u32 t1 = h + rmr_sha256_bsig1(e) +
            rmr_sha256_ch32(e, f, g) + g_rmr_sha256_k[index] +
            workspace->schedule[index];
        const rmr_u32 t2 = rmr_sha256_bsig0(a) +
            rmr_sha256_maj32(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    ctx->h[0] += a;
    ctx->h[1] += b;
    ctx->h[2] += c;
    ctx->h[3] += d;
    ctx->h[4] += e;
    ctx->h[5] += f;
    ctx->h[6] += g;
    ctx->h[7] += h;
}

void rmr_sha256_init(struct rmr_sha256_ctx *ctx)
{
    rmr_u32 index;

    if (ctx == (struct rmr_sha256_ctx *)0) {
        return;
    }

    ctx->h[0] = 0x6a09e667u;
    ctx->h[1] = 0xbb67ae85u;
    ctx->h[2] = 0x3c6ef372u;
    ctx->h[3] = 0xa54ff53au;
    ctx->h[4] = 0x510e527fu;
    ctx->h[5] = 0x9b05688cu;
    ctx->h[6] = 0x1f83d9abu;
    ctx->h[7] = 0x5be0cd19u;
    ctx->total_bytes = 0u;
    ctx->buffer_len = 0u;

    for (index = 0u; index < RMR_SHA256_BLOCK_BYTES; ++index) {
        ctx->buffer[index] = 0u;
    }
}

void
rmr_sha256_update(struct rmr_sha256_ctx *ctx,
                  struct rmr_sha256_workspace *workspace,
                  const void *data,
                  rmr_size len)
{
    const rmr_u8 *source = (const rmr_u8 *)data;

    if (ctx == (struct rmr_sha256_ctx *)0 ||
        workspace == (struct rmr_sha256_workspace *)0 ||
        (source == (const rmr_u8 *)0 && len != 0u)) {
        return;
    }

    ctx->total_bytes += (rmr_u64)len;
    while (len != 0u) {
        const rmr_u32 free_bytes =
            RMR_SHA256_BLOCK_BYTES - ctx->buffer_len;
        const rmr_u32 take = ((rmr_size)free_bytes < len) ?
            free_bytes : (rmr_u32)len;
        rmr_u32 index;

        for (index = 0u; index < take; ++index) {
            ctx->buffer[ctx->buffer_len + index] = source[index];
        }
        ctx->buffer_len += take;
        source += take;
        len -= take;

        if (ctx->buffer_len == RMR_SHA256_BLOCK_BYTES) {
            rmr_sha256_compress(ctx, workspace, ctx->buffer);
            ctx->buffer_len = 0u;
        }
    }
}

void
rmr_sha256_final(struct rmr_sha256_ctx *ctx,
                 struct rmr_sha256_workspace *workspace,
                 rmr_u8 digest[RMR_SHA256_DIGEST_BYTES])
{
    rmr_u32 index;
    rmr_u64 bit_length;

    if (ctx == (struct rmr_sha256_ctx *)0 ||
        workspace == (struct rmr_sha256_workspace *)0 ||
        digest == (rmr_u8 *)0) {
        return;
    }

    bit_length = ctx->total_bytes << 3u;
    ctx->buffer[ctx->buffer_len++] = 0x80u;

    if (ctx->buffer_len > 56u) {
        while (ctx->buffer_len < RMR_SHA256_BLOCK_BYTES) {
            ctx->buffer[ctx->buffer_len++] = 0u;
        }
        rmr_sha256_compress(ctx, workspace, ctx->buffer);
        ctx->buffer_len = 0u;
    }

    while (ctx->buffer_len < 56u) {
        ctx->buffer[ctx->buffer_len++] = 0u;
    }
    for (index = 0u; index < 8u; ++index) {
        ctx->buffer[56u + index] =
            (rmr_u8)(bit_length >> (56u - index * 8u));
    }

    rmr_sha256_compress(ctx, workspace, ctx->buffer);
    for (index = 0u; index < 8u; ++index) {
        rmr_store32_be(&digest[index * 4u], ctx->h[index]);
    }

    rmr_secure_zero(workspace, (rmr_size)sizeof(*workspace));
    rmr_secure_zero(ctx->buffer, (rmr_size)sizeof(ctx->buffer));
    ctx->buffer_len = 0u;
}

void
rmr_sha256_digest(const void *data,
                  rmr_size len,
                  struct rmr_sha256_workspace *workspace,
                  rmr_u8 digest[RMR_SHA256_DIGEST_BYTES])
{
    struct rmr_sha256_ctx ctx;

    rmr_sha256_init(&ctx);
    rmr_sha256_update(&ctx, workspace, data, len);
    rmr_sha256_final(&ctx, workspace, digest);
    rmr_secure_zero(&ctx, (rmr_size)sizeof(ctx));
}

const struct rmr_module_descriptor *rmr_sha256_module_descriptor(void)
{
    return &g_rmr_sha256_module;
}
