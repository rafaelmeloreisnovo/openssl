/* Hosted diagnostic only. It is not linked into libcrypto or the hot path. */
#include <stdio.h>
#include <openssl/crypto.h>
#include "../include/rmr_silicon.h"

int main(void)
{
    const char *cpu = OPENSSL_info(OPENSSL_INFO_CPU_SETTINGS);

    printf("openssl_version_num=%lx\n", OpenSSL_version_num());
    printf("openssl_version=%s\n", OpenSSL_version(OPENSSL_FULL_VERSION_STRING));
    printf("openssl_cpu_settings=%s\n", cpu != NULL ? cpu : "TOKEN_VAZIO");
    printf("rmr_compile_caps=%016llx\n",
           (unsigned long long)rmr_silicon_compile_caps());
    printf("rmr_build_policy=%016llx\n",
           (unsigned long long)rmr_silicon_build_policy());
    return 0;
}
