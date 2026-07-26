# Claims ledger — RMR OpenSSL Silicon Contract V1.1

| Claim | Estado | Evidência ou falsificador |
|---|---|---|
| Upstream OpenSSL não foi alterado | `VERIFIED_BY_DIFF_SCOPE` | Mudança somente em `rmr/` e workflow escopado |
| CMake é o sistema canônico da camada RMR | `IMPLEMENTED` | `rmr/CMakeLists.txt`, `CMakePresets.json` e `cmake/RMROptimization.cmake` |
| O CMake seleciona ISA, flags e backend ASM por perfil | `IMPLEMENTED` | perfis `portable`, `host-native`, `x86_64-v3`, `aarch64-crypto` e `armv7-neon` |
| `-O3`, arquitetura, tune, IPO e linker policy são rastreáveis | `IMPLEMENTED_AWAITING_REMOTE_RECEIPT` | manifesto gerado `rmr-cmake-flags-Release.txt`; CI deve preservar o artefato |
| Preset host-native aplica `-march=native -mtune=native` | `IMPLEMENTED_AWAITING_REMOTE_RECEIPT` | `compile_commands.json` é verificado pelo gate `audit_cmake_contract.sh` |
| ARM64 usa perfil crypto/CRC/SIMD | `IMPLEMENTED_NOT_DEVICE_EXECUTED` | `-march=armv8-a+crypto+crc+simd`; build/execution Android ainda pendentes |
| ARMv7 usa NEON/VFPv4/softfp | `IMPLEMENTED_NOT_DEVICE_EXECUTED` | `-march=armv7-a -mfpu=neon-vfpv4 -mfloat-abi=softfp`; execução ainda pendente |
| Core RMR não usa heap/libc | `TESTED_STATIC_LIMITED` | objeto `-ffreestanding`; `nm -u` vazio; auditoria de objeto |
| C e ASM x86_64 são equivalentes nos KATs | `TESTED_LOCAL` | `rmr_kat_x86_64` e CTest |
| ASM AArch64 e ARMv7 compilam e ligam | `COMPILED_CROSS` | objetos relocáveis fechados por LLD |
| Existe gate CMake nativo para Android/Termux | `IMPLEMENTED` | `tools/run_termux_kat.sh` configura, compila, testa e emite recibo na ABI local |
| AArch64/ARMv7 executam corretamente no aparelho | `TOKEN_VAZIO` | executar o gate Termux no aparelho correspondente e preservar recibo/hash |
| Linker preserva manifesto e texto RMR | `TESTED_LOCAL` | `readelf -SW` |
| Manifesto imutável e materializado usam fold derivado | `TESTED_BY_KAT` | ambos são recalculados por `rmr_silicon_manifest_fold` |
| SHA2-256 e SHA3-256 funcionam pela API EVP pública | `TESTED_HOSTED_LIMITED` | vetores conhecidos de `abc` em `openssl_evp_kat.c` |
| AES-GCM, ChaCha20-Poly1305 e AES-CTR são consultáveis por provider | `OBSERVED_HOSTED_LOCAL` | `EVP_CIPHER_fetch` no OpenSSL local; não generalizar para outro build |
| A API pública expõe configurações de CPU do OpenSSL | `SUPPORTED_BY_SOURCE_AND_LOCAL_PROBE` | `OPENSSL_info(OPENSSL_INFO_CPU_SETTINGS)` |
| O fork BLAKE3 de Rafael é mais rápido que o oficial | `DECLARED_BY_AUTHOR_REPRODUCTION_PENDING` | comparar ambos na mesma máquina, commit, compilador, flags, ISA, entradas e protocolo p50/p95/p99 |
| O CMake do fork BLAKE3 participa do desempenho observado | `STRUCTURALLY_SUPPORTED_NOT_CAUSALLY_ISOLATED` | CMake seleciona SIMD/ASM e flags por fonte; medir ablação CMake/flags/código para isolar causalidade |
| A camada OpenSSL RMR é mais rápida que OpenSSL upstream | `TOKEN_VAZIO` | build completo comparável e benchmark controlado ainda ausentes |
| Há ganho de OpenSSL RMR sobre o BLAKE3 autoral | `NOT_A_VALID_CURRENT_COMPARISON` | algoritmos e objetivos distintos; comparação só com workload e finalidade explicitados |
| A camada é apta a ASIC/FPGA | `HYPOTHESIS` | RTL, síntese, timing, área, potência e equivalência formal ausentes |
| Há conformidade FIPS/ISO | `NOT_CLAIMED` | fora do escopo e sem certificação |
| Compilação cross prova execução física | `FALSE_BOUNDARY` | compilação não executa no silício-alvo |

## Gate mestre

```text
source_present
≠ cmake_profile_defined
≠ flags_applied
≠ optimized_object_built
≠ object_linked
≠ KAT_executed
≠ provider_observed
≠ device_executed
≠ benchmark_reproduced
≠ faster_than_baseline
≠ silicon_proven
```

## Regra de comparação BLAKE3

A declaração autoral de superioridade não é descartada. Ela permanece registrada,
mas somente pode ser promovida quando o ensaio fixar:

```text
mesmo aparelho e estado térmico
mesmo compilador e versão
mesma ISA e conjunto de flags
mesma quantidade e distribuição de dados
mesma API e custo de inicialização
mesmos warmups e número de amostras
p50, p95, p99, IQR e hashes dos binários
commits exatos do fork e do oficial
```

Uma comparação entre binários produzidos por CMake diferente sem registrar as
flags mede um pacote indistinguível de efeitos. O protocolo precisa separar por
ablação:

```text
código oficial + build oficial
código oficial + flags RMR compatíveis
código RMR + build conservadora
código RMR + CMake/flags RMR
```

Assim se distingue ganho de código, dispatch, compilador, linker e ISA.

Qualquer promoção deve apontar o artefato, arquitetura, compilador, CMake preset,
flags, linker, ABI, aparelho, ambiente, provider, hash e falsificador.
