/*
 * Copyright 2026 Rafael Melo Reis.
 * See the repository LICENSE.txt.
 *
 * RMR OpenSSL silicon contract: freestanding, fixed-width, no heap.
 */
#ifndef RMR_SILICON_H
#define RMR_SILICON_H

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char rmr_u8;
typedef unsigned int rmr_u32;
typedef unsigned long long rmr_u64;
typedef unsigned long rmr_size;

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
_Static_assert(sizeof(rmr_u8) == 1, "rmr_u8 must be 8-bit");
_Static_assert(sizeof(rmr_u32) == 4, "rmr_u32 must be 32-bit");
_Static_assert(sizeof(rmr_u64) == 8, "rmr_u64 must be 64-bit");
#endif

#define RMR_SILICON_ABI_VERSION 0x00010000u
#define RMR_SILICON_MAGIC 0x524d5253u /* RMRS */

/* Architecture and compiler-visible ISA capabilities. */
enum rmr_silicon_cap_bit {
    RMR_CAP_ARCH_X86_64 = 0,
    RMR_CAP_ARCH_AARCH64 = 1,
    RMR_CAP_ARCH_ARMV7 = 2,
    RMR_CAP_ARCH_RISCV64 = 3,
    RMR_CAP_LITTLE_ENDIAN = 4,
    RMR_CAP_BIG_ENDIAN = 5,
    RMR_CAP_WORD64 = 6,
    RMR_CAP_WORD32 = 7,
    RMR_CAP_X86_SSE2 = 8,
    RMR_CAP_X86_SSSE3 = 9,
    RMR_CAP_X86_AES = 10,
    RMR_CAP_X86_PCLMUL = 11,
    RMR_CAP_X86_SHA = 12,
    RMR_CAP_X86_AVX2 = 13,
    RMR_CAP_X86_AVX512F = 14,
    RMR_CAP_ARM_NEON = 16,
    RMR_CAP_ARM_CRYPTO = 17,
    RMR_CAP_ARM_AES = 18,
    RMR_CAP_ARM_SHA2 = 19,
    RMR_CAP_ARM_SHA3 = 20,
    RMR_CAP_ARM_CRC32 = 21,
    RMR_CAP_ARM_SVE = 22,
    RMR_CAP_ARM_SVE2 = 23,
    RMR_CAP_RISCV_ZBB = 24,
    RMR_CAP_RISCV_ZBC = 25,
    RMR_CAP_RISCV_VECTOR = 26,
    RMR_CAP_ARCH_ASM_LINKED = 31
};

/* Build/link policy is separate from silicon capability. */
enum rmr_build_policy_bit {
    RMR_POLICY_FREESTANDING = 0,
    RMR_POLICY_NO_LIBC = 1,
    RMR_POLICY_NO_HEAP = 2,
    RMR_POLICY_NO_BUILTIN = 3,
    RMR_POLICY_NO_STACK_PROTECTOR = 4,
    RMR_POLICY_FUNCTION_SECTIONS = 5,
    RMR_POLICY_DATA_SECTIONS = 6,
    RMR_POLICY_HIDDEN_VISIBILITY = 7,
    RMR_POLICY_GC_SECTIONS = 8,
    RMR_POLICY_BUILD_ID_NONE = 9,
    RMR_POLICY_LTO = 10,
    RMR_POLICY_FIXED_ABI = 11
};

struct rmr_silicon_manifest {
    rmr_u32 magic;
    rmr_u32 abi_version;
    rmr_u64 compile_caps;
    rmr_u64 build_policy;
    rmr_u32 manifest_fold;
    rmr_u32 reserved;
};

rmr_u64 rmr_silicon_compile_caps(void);
rmr_u64 rmr_silicon_build_policy(void);
const struct rmr_silicon_manifest *rmr_silicon_manifest_get(void);
void rmr_silicon_manifest_materialize(struct rmr_silicon_manifest *out);
rmr_u32 rmr_silicon_manifest_fold(const struct rmr_silicon_manifest *m);

rmr_u32 rmr_rotl32(rmr_u32 x, rmr_u32 n);
rmr_u32 rmr_rotr32(rmr_u32 x, rmr_u32 n);
rmr_u32 rmr_mix32_ref(rmr_u32 x);
rmr_u32 rmr_mix32_arch(rmr_u32 x);
rmr_u32 rmr_ct_mask_u32(rmr_u32 bit);
rmr_u32 rmr_ct_select_u32(rmr_u32 mask, rmr_u32 a, rmr_u32 b);
rmr_u32 rmr_ct_eq_u32(rmr_u32 a, rmr_u32 b);
rmr_u32 rmr_load32_le(const rmr_u8 src[4]);
void rmr_store32_le(rmr_u8 dst[4], rmr_u32 x);
void rmr_secure_zero(void *ptr, rmr_size len);

#ifdef __cplusplus
}
#endif

#endif
