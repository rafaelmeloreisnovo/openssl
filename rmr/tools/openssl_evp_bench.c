/*
 * Hosted EVP benchmark. No performance claim is made by this tool alone.
 * Copyright 2026 Rafael Melo Reis. See repository LICENSE.txt.
 */
#define _POSIX_C_SOURCE 200809L
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <openssl/crypto.h>
#include <openssl/evp.h>
#include "../include/rmr_silicon.h"

#define RMR_BENCH_SAMPLES 21u
#define RMR_BENCH_WARMUPS 5u
#define RMR_BENCH_MAX_INPUT (1024u * 1024u)

static unsigned char input_buffer[RMR_BENCH_MAX_INPUT];
static unsigned char digest_buffer[EVP_MAX_MD_SIZE];

struct bench_case {
    size_t bytes_per_op;
    unsigned int ops_per_sample;
};

static const struct bench_case cases[] = {
    {64u, 4096u},
    {1024u, 4096u},
    {16384u, 256u},
    {1048576u, 4u}
};

static uint64_t now_ns(void)
{
    struct timespec ts;

    if (clock_gettime(CLOCK_MONOTONIC_RAW, &ts) != 0)
        return 0;
    return ((uint64_t)ts.tv_sec * 1000000000ull) + (uint64_t)ts.tv_nsec;
}

static void sort_u64(uint64_t *values, unsigned int count)
{
    unsigned int i;

    for (i = 1; i < count; ++i) {
        uint64_t value = values[i];
        unsigned int j = i;

        while (j > 0 && values[j - 1] > value) {
            values[j] = values[j - 1];
            --j;
        }
        values[j] = value;
    }
}

static unsigned int percentile_index(unsigned int count, unsigned int pct)
{
    unsigned int rank = (count * pct + 99u) / 100u;

    if (rank == 0)
        return 0;
    if (rank > count)
        rank = count;
    return rank - 1u;
}

static int digest_batch(EVP_MD_CTX *ctx, const EVP_MD *md,
                        size_t bytes_per_op, unsigned int operations)
{
    unsigned int i;

    for (i = 0; i < operations; ++i) {
        unsigned int digest_len = 0;

        if (EVP_DigestInit_ex(ctx, md, NULL) != 1 ||
            EVP_DigestUpdate(ctx, input_buffer, bytes_per_op) != 1 ||
            EVP_DigestFinal_ex(ctx, digest_buffer, &digest_len) != 1)
            return 0;
    }
    return 1;
}

static int benchmark_algorithm(const char *algorithm)
{
    EVP_MD *md = EVP_MD_fetch(NULL, algorithm, NULL);
    EVP_MD_CTX *ctx = NULL;
    unsigned int case_index;
    int ok = 0;

    if (md == NULL) {
        fprintf(stderr, "algorithm=%s status=TOKEN_VAZIO\n", algorithm);
        return 0;
    }

    ctx = EVP_MD_CTX_new();
    if (ctx == NULL)
        goto done;

    for (case_index = 0;
         case_index < sizeof(cases) / sizeof(cases[0]);
         ++case_index) {
        uint64_t samples[RMR_BENCH_SAMPLES];
        uint64_t p50;
        uint64_t p95;
        uint64_t p99;
        double total_mib;
        double p50_mib_s;
        unsigned int i;

        for (i = 0; i < RMR_BENCH_WARMUPS; ++i) {
            if (!digest_batch(ctx, md,
                              cases[case_index].bytes_per_op,
                              cases[case_index].ops_per_sample))
                goto done;
        }

        for (i = 0; i < RMR_BENCH_SAMPLES; ++i) {
            uint64_t start = now_ns();
            uint64_t end;

            if (start == 0 ||
                !digest_batch(ctx, md,
                              cases[case_index].bytes_per_op,
                              cases[case_index].ops_per_sample))
                goto done;
            end = now_ns();
            if (end <= start)
                goto done;
            samples[i] = end - start;
        }

        sort_u64(samples, RMR_BENCH_SAMPLES);
        p50 = samples[percentile_index(RMR_BENCH_SAMPLES, 50u)];
        p95 = samples[percentile_index(RMR_BENCH_SAMPLES, 95u)];
        p99 = samples[percentile_index(RMR_BENCH_SAMPLES, 99u)];
        total_mib = ((double)cases[case_index].bytes_per_op *
                     (double)cases[case_index].ops_per_sample) /
                    (1024.0 * 1024.0);
        p50_mib_s = total_mib / ((double)p50 / 1000000000.0);

        printf("%s,%zu,%u,%u,%llu,%llu,%llu,%.3f\n",
               algorithm,
               cases[case_index].bytes_per_op,
               cases[case_index].ops_per_sample,
               RMR_BENCH_SAMPLES,
               (unsigned long long)p50,
               (unsigned long long)p95,
               (unsigned long long)p99,
               p50_mib_s);
    }

    ok = 1;

done:
    EVP_MD_CTX_free(ctx);
    EVP_MD_free(md);
    return ok;
}

int main(void)
{
    size_t i;
    int sha256_ok;
    int sha3_ok;

    for (i = 0; i < sizeof(input_buffer); ++i)
        input_buffer[i] = (unsigned char)((i * 131u + 17u) & 0xffu);

    printf("# openssl=%s\n", OpenSSL_version(OPENSSL_FULL_VERSION_STRING));
    printf("# rmr_compile_caps=%016llx\n",
           (unsigned long long)rmr_silicon_compile_caps());
    printf("# rmr_build_policy=%016llx\n",
           (unsigned long long)rmr_silicon_build_policy());
    puts("algorithm,bytes_per_op,ops_per_sample,samples,"
         "p50_ns,p95_ns,p99_ns,p50_mib_s");

    sha256_ok = benchmark_algorithm("SHA2-256");
    sha3_ok = benchmark_algorithm("SHA3-256");
    OPENSSL_cleanse(input_buffer, sizeof(input_buffer));
    OPENSSL_cleanse(digest_buffer, sizeof(digest_buffer));

    if (!sha256_ok || !sha3_ok)
        return 1;
    return 0;
}
