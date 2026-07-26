/* Hosted KAT only; the core under test remains freestanding. */
#include <stdio.h>
#include "../include/rmr_silicon.h"

struct kat32 {
    rmr_u32 in;
    rmr_u32 out;
};

static const struct kat32 vectors[] = {
    {0x00000000u, 0x9e3779b9u},
    {0x00000001u, 0x9c3759b8u},
    {0x12345678u, 0xf6e8452bu},
    {0xffffffffu, 0x61c88646u},
    {0xa5a5a5a5u, 0xc46d23e3u}
};

static int test_mix(void)
{
    unsigned i;

    for (i = 0; i < sizeof(vectors) / sizeof(vectors[0]); ++i) {
        rmr_u32 ref = rmr_mix32_ref(vectors[i].in);
        rmr_u32 arch = rmr_mix32_arch(vectors[i].in);

        if (ref != vectors[i].out || arch != ref) {
            fprintf(stderr,
                    "mix KAT failed i=%u ref=%08x arch=%08x expected=%08x\n",
                    i, ref, arch, vectors[i].out);
            return 1;
        }
    }
    return 0;
}

static int test_ct(void)
{
    rmr_u32 m0 = rmr_ct_mask_u32(0);
    rmr_u32 m1 = rmr_ct_mask_u32(1);

    if (m0 != 0 || m1 != 0xffffffffu)
        return 1;
    if (rmr_ct_select_u32(m1, 0x11u, 0x22u) != 0x11u)
        return 1;
    if (rmr_ct_select_u32(m0, 0x11u, 0x22u) != 0x22u)
        return 1;
    if (rmr_ct_eq_u32(7u, 7u) != 1u || rmr_ct_eq_u32(7u, 8u) != 0u)
        return 1;
    return 0;
}

static int test_endian_and_zero(void)
{
    rmr_u8 b[8] = {0};
    unsigned i;

    rmr_store32_le(b, 0x78563412u);
    if (b[0] != 0x12 || b[1] != 0x34 || b[2] != 0x56 || b[3] != 0x78)
        return 1;
    if (rmr_load32_le(b) != 0x78563412u)
        return 1;

    for (i = 0; i < sizeof(b); ++i)
        b[i] = 0xa5u;
    rmr_secure_zero(b, sizeof(b));
    for (i = 0; i < sizeof(b); ++i) {
        if (b[i] != 0)
            return 1;
    }
    return 0;
}

int main(void)
{
    const struct rmr_silicon_manifest *template_m = rmr_silicon_manifest_get();
    struct rmr_silicon_manifest m;

    rmr_silicon_manifest_materialize(&m);
    if (template_m->magic != RMR_SILICON_MAGIC ||
        template_m->abi_version != RMR_SILICON_ABI_VERSION ||
        template_m->compile_caps != 0u ||
        template_m->build_policy != 0u ||
        template_m->reserved != 0u ||
        template_m->manifest_fold != rmr_silicon_manifest_fold(template_m))
        return 1;
    if (m.magic != RMR_SILICON_MAGIC ||
        m.abi_version != RMR_SILICON_ABI_VERSION ||
        m.reserved != 0u)
        return 1;
    if (m.compile_caps != rmr_silicon_compile_caps() ||
        m.build_policy != rmr_silicon_build_policy() ||
        m.manifest_fold != rmr_silicon_manifest_fold(&m))
        return 1;
    if (test_mix() || test_ct() || test_endian_and_zero())
        return 1;

    printf("RMR_OPENSSL_SILICON_KAT PASS caps=%016llx policy=%016llx fold=%08x\n",
           (unsigned long long)m.compile_caps,
           (unsigned long long)m.build_policy,
           m.manifest_fold);
    return 0;
}
