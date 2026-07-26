/*
 * Copyright 2026 Rafael Melo Reis.
 * See the repository LICENSE.txt.
 */
#include "../include/rmr_silicon.h"

#if defined(__GNUC__) || defined(__clang__)
#define RMR_TEXT __attribute__((section(".rmr.text"), noinline))
#define RMR_MANIFEST __attribute__((section(".rmr.manifest"), used, aligned(64)))
#else
#define RMR_TEXT
#define RMR_MANIFEST
#endif

#define RMR_BIT64(n) (((rmr_u64)1u) << (n))

static RMR_TEXT rmr_u64 rmr_detect_compile_caps(void)
{
    rmr_u64 caps = 0;

#if defined(__x86_64__) || defined(_M_X64)
    caps |= RMR_BIT64(RMR_CAP_ARCH_X86_64) | RMR_BIT64(RMR_CAP_WORD64);
#elif defined(__aarch64__) || defined(_M_ARM64)
    caps |= RMR_BIT64(RMR_CAP_ARCH_AARCH64) | RMR_BIT64(RMR_CAP_WORD64);
#elif defined(__arm__) || defined(_M_ARM)
    caps |= RMR_BIT64(RMR_CAP_ARCH_ARMV7) | RMR_BIT64(RMR_CAP_WORD32);
#elif defined(__riscv) && (__riscv_xlen == 64)
    caps |= RMR_BIT64(RMR_CAP_ARCH_RISCV64) | RMR_BIT64(RMR_CAP_WORD64);
#endif

#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
    caps |= RMR_BIT64(RMR_CAP_BIG_ENDIAN);
#else
    caps |= RMR_BIT64(RMR_CAP_LITTLE_ENDIAN);
#endif

#if defined(__SSE2__) || defined(_M_X64)
    caps |= RMR_BIT64(RMR_CAP_X86_SSE2);
#endif
#if defined(__SSSE3__)
    caps |= RMR_BIT64(RMR_CAP_X86_SSSE3);
#endif
#if defined(__AES__)
    caps |= RMR_BIT64(RMR_CAP_X86_AES);
#endif
#if defined(__PCLMUL__)
    caps |= RMR_BIT64(RMR_CAP_X86_PCLMUL);
#endif
#if defined(__SHA__)
    caps |= RMR_BIT64(RMR_CAP_X86_SHA);
#endif
#if defined(__AVX2__)
    caps |= RMR_BIT64(RMR_CAP_X86_AVX2);
#endif
#if defined(__AVX512F__)
    caps |= RMR_BIT64(RMR_CAP_X86_AVX512F);
#endif

#if defined(__ARM_NEON) || defined(__ARM_NEON__)
    caps |= RMR_BIT64(RMR_CAP_ARM_NEON);
#endif
#if defined(__ARM_FEATURE_CRYPTO)
    caps |= RMR_BIT64(RMR_CAP_ARM_CRYPTO);
#endif
#if defined(__ARM_FEATURE_AES)
    caps |= RMR_BIT64(RMR_CAP_ARM_AES);
#endif
#if defined(__ARM_FEATURE_SHA2)
    caps |= RMR_BIT64(RMR_CAP_ARM_SHA2);
#endif
#if defined(__ARM_FEATURE_SHA3)
    caps |= RMR_BIT64(RMR_CAP_ARM_SHA3);
#endif
#if defined(__ARM_FEATURE_CRC32)
    caps |= RMR_BIT64(RMR_CAP_ARM_CRC32);
#endif
#if defined(__ARM_FEATURE_SVE)
    caps |= RMR_BIT64(RMR_CAP_ARM_SVE);
#endif
#if defined(__ARM_FEATURE_SVE2)
    caps |= RMR_BIT64(RMR_CAP_ARM_SVE2);
#endif

#if defined(__riscv_zbb)
    caps |= RMR_BIT64(RMR_CAP_RISCV_ZBB);
#endif
#if defined(__riscv_zbc)
    caps |= RMR_BIT64(RMR_CAP_RISCV_ZBC);
#endif
#if defined(__riscv_vector)
    caps |= RMR_BIT64(RMR_CAP_RISCV_VECTOR);
#endif
#if defined(RMR_USE_ARCH_ASM)
    caps |= RMR_BIT64(RMR_CAP_ARCH_ASM_LINKED);
#endif
    return caps;
}

RMR_TEXT rmr_u64 rmr_silicon_compile_caps(void)
{
    return rmr_detect_compile_caps();
}

RMR_TEXT rmr_u64 rmr_silicon_build_policy(void)
{
    rmr_u64 policy = RMR_BIT64(RMR_POLICY_FIXED_ABI);
#if defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 0)
    policy |= RMR_BIT64(RMR_POLICY_FREESTANDING);
#endif
#if defined(RMR_NO_LIBC)
    policy |= RMR_BIT64(RMR_POLICY_NO_LIBC);
#endif
#if defined(RMR_NO_HEAP)
    policy |= RMR_BIT64(RMR_POLICY_NO_HEAP);
#endif
#if defined(RMR_NO_BUILTIN)
    policy |= RMR_BIT64(RMR_POLICY_NO_BUILTIN);
#endif
#if defined(RMR_NO_STACK_PROTECTOR)
    policy |= RMR_BIT64(RMR_POLICY_NO_STACK_PROTECTOR);
#endif
#if defined(RMR_FUNCTION_SECTIONS)
    policy |= RMR_BIT64(RMR_POLICY_FUNCTION_SECTIONS);
#endif
#if defined(RMR_DATA_SECTIONS)
    policy |= RMR_BIT64(RMR_POLICY_DATA_SECTIONS);
#endif
#if defined(RMR_HIDDEN_VISIBILITY)
    policy |= RMR_BIT64(RMR_POLICY_HIDDEN_VISIBILITY);
#endif
#if defined(RMR_GC_SECTIONS)
    policy |= RMR_BIT64(RMR_POLICY_GC_SECTIONS);
#endif
#if defined(RMR_BUILD_ID_NONE)
    policy |= RMR_BIT64(RMR_POLICY_BUILD_ID_NONE);
#endif
#if defined(RMR_LTO)
    policy |= RMR_BIT64(RMR_POLICY_LTO);
#endif
    return policy;
}

RMR_TEXT rmr_u32 rmr_rotl32(rmr_u32 x, rmr_u32 n)
{
    n &= 31u;
    return (x << n) | (x >> ((32u - n) & 31u));
}

RMR_TEXT rmr_u32 rmr_rotr32(rmr_u32 x, rmr_u32 n)
{
    n &= 31u;
    return (x >> n) | (x << ((32u - n) & 31u));
}

RMR_TEXT rmr_u32 rmr_mix32_ref(rmr_u32 x)
{
    return x ^ rmr_rotl32(x, 13u) ^ rmr_rotr32(x, 7u) ^ 0x9e3779b9u;
}

#if !defined(RMR_USE_ARCH_ASM)
RMR_TEXT rmr_u32 rmr_mix32_arch(rmr_u32 x)
{
    return rmr_mix32_ref(x);
}
#endif

RMR_TEXT rmr_u32 rmr_ct_mask_u32(rmr_u32 bit)
{
    return (rmr_u32)0u - (bit & 1u);
}

RMR_TEXT rmr_u32 rmr_ct_select_u32(rmr_u32 mask, rmr_u32 a, rmr_u32 b)
{
    return (a & mask) | (b & ~mask);
}

RMR_TEXT rmr_u32 rmr_ct_eq_u32(rmr_u32 a, rmr_u32 b)
{
    rmr_u32 x = a ^ b;
    x |= (rmr_u32)0u - x;
    return (x >> 31u) ^ 1u;
}

RMR_TEXT rmr_u32 rmr_load32_le(const rmr_u8 src[4])
{
    return ((rmr_u32)src[0]) | ((rmr_u32)src[1] << 8u) |
           ((rmr_u32)src[2] << 16u) | ((rmr_u32)src[3] << 24u);
}

RMR_TEXT void rmr_store32_le(rmr_u8 dst[4], rmr_u32 x)
{
    dst[0] = (rmr_u8)x;
    dst[1] = (rmr_u8)(x >> 8u);
    dst[2] = (rmr_u8)(x >> 16u);
    dst[3] = (rmr_u8)(x >> 24u);
}

RMR_TEXT void rmr_secure_zero(void *ptr, rmr_size len)
{
    volatile rmr_u8 *p = (volatile rmr_u8 *)ptr;
    while (len != 0u) {
        *p++ = 0u;
        --len;
    }
}

/* Immutable zero-capability template retained by the linker. */
static const struct rmr_silicon_manifest g_rmr_manifest RMR_MANIFEST = {
    RMR_SILICON_MAGIC,
    RMR_SILICON_ABI_VERSION,
    0u,
    0u,
    0xf11a0efau,
    0u
};

RMR_TEXT const struct rmr_silicon_manifest *rmr_silicon_manifest_get(void)
{
    return &g_rmr_manifest;
}

RMR_TEXT rmr_u32 rmr_silicon_manifest_fold(const struct rmr_silicon_manifest *m)
{
    rmr_u32 x = m->magic ^ m->abi_version;
    x = rmr_mix32_ref(x ^ (rmr_u32)m->compile_caps);
    x = rmr_mix32_ref(x ^ (rmr_u32)(m->compile_caps >> 32u));
    x = rmr_mix32_ref(x ^ (rmr_u32)m->build_policy);
    x = rmr_mix32_ref(x ^ (rmr_u32)(m->build_policy >> 32u));
    return x;
}

RMR_TEXT void rmr_silicon_manifest_materialize(struct rmr_silicon_manifest *out)
{
    if (out == (void *)0)
        return;
    out->magic = RMR_SILICON_MAGIC;
    out->abi_version = RMR_SILICON_ABI_VERSION;
    out->compile_caps = rmr_silicon_compile_caps();
    out->build_policy = rmr_silicon_build_policy();
    out->reserved = 0u;
    out->manifest_fold = rmr_silicon_manifest_fold(out);
}
