# Claims ledger — RMR OpenSSL Silicon Contract V1

| Claim | Estado | Evidência ou falsificador |
|---|---|---|
| Upstream OpenSSL não foi alterado | `VERIFIED_BY_DIFF_SCOPE` | Mudança somente em `rmr/` e workflow escopado |
| Core RMR não usa heap/libc | `TESTED_STATIC_LIMITED` | objeto `-ffreestanding`; `nm -u` vazio; auditoria de objeto |
| C e ASM x86_64 são equivalentes nos KATs | `TESTED_LOCAL` | `rmr_kat_x86_64` |
| ASM AArch64 e ARMv7 compilam e ligam | `COMPILED_CROSS` | objetos relocáveis fechados por LLD |
| Existe gate nativo para Android/Termux | `IMPLEMENTED` | `tools/run_termux_kat.sh` executa C, ASM, objeto e recibo na ABI local |
| AArch64/ARMv7 executam corretamente no aparelho | `TOKEN_VAZIO` | executar o gate Termux no aparelho correspondente e preservar recibo/hash |
| Linker preserva manifesto e texto RMR | `TESTED_LOCAL` | `readelf -SW` |
| Manifesto imutável e materializado usam fold derivado | `TESTED_BY_KAT` | ambos são recalculados por `rmr_silicon_manifest_fold` |
| A API pública expõe configurações de CPU do OpenSSL | `SUPPORTED_BY_SOURCE_AND_LOCAL_PROBE` | `OPENSSL_info(OPENSSL_INFO_CPU_SETTINGS)` |
| Há ganho de desempenho sobre OpenSSL upstream | `TOKEN_VAZIO` | benchmark isolado, p50/p95/p99, mesma ISA e flags |
| A camada é apta a ASIC/FPGA | `HYPOTHESIS` | RTL, síntese, timing, área, potência e equivalência formal ausentes |
| Há conformidade FIPS/ISO | `NOT_CLAIMED` | fora do escopo e sem certificação |
| Compilação cross prova execução física | `FALSE_BOUNDARY` | compilação não executa no silício-alvo |

## Gate mestre

```text
source_present
≠ object_compiled
≠ object_linked
≠ KAT_executed
≠ device_executed
≠ benchmark_reproduced
≠ silicon_proven
```

Qualquer promoção deve apontar o artefato, arquitetura, compilador, flags,
linker, ABI, aparelho, ambiente, hash e falsificador correspondentes.
