/*
 * Hosted OpenSSL EVP adapter/KAT. It is deliberately outside libcrypto.
 * Copyright 2026 Rafael Melo Reis. See repository LICENSE.txt.
 */
#include <stdio.h>
#include <string.h>
#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <openssl/provider.h>
#include "../include/rmr_silicon.h"

static int from_hex(unsigned char *out, size_t out_len, const char *hex)
{
    size_t i;

    for (i = 0; i < out_len; ++i) {
        unsigned int value;

        if (sscanf(hex + (i * 2u), "%2x", &value) != 1)
            return 0;
        out[i] = (unsigned char)value;
    }
    return hex[out_len * 2u] == '\0';
}

static const char *md_provider_name(const EVP_MD *md)
{
    const OSSL_PROVIDER *provider = EVP_MD_get0_provider(md);
    const char *name =
        provider != NULL ? OSSL_PROVIDER_get0_name(provider) : NULL;

    return name != NULL ? name : "TOKEN_VAZIO";
}

static const char *cipher_provider_name(const EVP_CIPHER *cipher)
{
    const OSSL_PROVIDER *provider = EVP_CIPHER_get0_provider(cipher);
    const char *name =
        provider != NULL ? OSSL_PROVIDER_get0_name(provider) : NULL;

    return name != NULL ? name : "TOKEN_VAZIO";
}

static int digest_kat(const char *algorithm, const char *expected_hex)
{
    static const unsigned char message[] = {'a', 'b', 'c'};
    unsigned char expected[EVP_MAX_MD_SIZE];
    unsigned char actual[EVP_MAX_MD_SIZE];
    unsigned int actual_len = 0;
    size_t expected_len = strlen(expected_hex) / 2u;
    EVP_MD *md = NULL;
    EVP_MD_CTX *ctx = NULL;
    int ok = 0;

    if (expected_len == 0 || expected_len > sizeof(expected) ||
        !from_hex(expected, expected_len, expected_hex))
        goto done;

    md = EVP_MD_fetch(NULL, algorithm, NULL);
    if (md == NULL) {
        printf("digest=%s status=TOKEN_VAZIO provider=TOKEN_VAZIO\n",
               algorithm);
        goto done;
    }

    ctx = EVP_MD_CTX_new();
    if (ctx == NULL ||
        EVP_DigestInit_ex(ctx, md, NULL) != 1 ||
        EVP_DigestUpdate(ctx, message, sizeof(message)) != 1 ||
        EVP_DigestFinal_ex(ctx, actual, &actual_len) != 1)
        goto done;

    ok = actual_len == expected_len &&
         memcmp(actual, expected, expected_len) == 0;
    printf("digest=%s status=%s provider=%s bytes=%u\n",
           algorithm, ok ? "PASS" : "FAIL", md_provider_name(md), actual_len);

done:
    EVP_MD_CTX_free(ctx);
    EVP_MD_free(md);
    return ok;
}

static void probe_cipher(const char *algorithm)
{
    EVP_CIPHER *cipher = EVP_CIPHER_fetch(NULL, algorithm, NULL);

    if (cipher == NULL) {
        printf("cipher=%s status=TOKEN_VAZIO provider=TOKEN_VAZIO\n",
               algorithm);
        return;
    }

    printf("cipher=%s status=AVAILABLE provider=%s "
           "key_bits=%d block_bytes=%d\n",
           algorithm,
           cipher_provider_name(cipher),
           EVP_CIPHER_get_key_length(cipher) * 8,
           EVP_CIPHER_get_block_size(cipher));
    EVP_CIPHER_free(cipher);
}

int main(void)
{
    int sha256_ok;
    int sha3_ok;

    printf("openssl=%s\n", OpenSSL_version(OPENSSL_FULL_VERSION_STRING));
    printf("rmr_compile_caps=%016llx\n",
           (unsigned long long)rmr_silicon_compile_caps());
    printf("rmr_build_policy=%016llx\n",
           (unsigned long long)rmr_silicon_build_policy());

    sha256_ok = digest_kat(
        "SHA2-256",
        "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
    sha3_ok = digest_kat(
        "SHA3-256",
        "3a985da74fe225b2045c172d6bd390bd855f086e3e9d525b46bfe24511431532");

    probe_cipher("AES-128-GCM");
    probe_cipher("AES-256-GCM");
    probe_cipher("CHACHA20-POLY1305");
    probe_cipher("AES-128-CTR");

    if (!sha256_ok || !sha3_ok)
        return 1;

    puts("RMR_OPENSSL_EVP_KAT PASS_LIMITED");
    return 0;
}
