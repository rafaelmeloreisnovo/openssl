# RMR OpenSSL Silicon Contract V1

## Invariante

```text
upstream OpenSSL preservado
+ camada autoral isolada
+ capability word reproduzível
+ política de build separada
+ KAT C ↔ ASM
+ seções de linker auditáveis
= caminho verificável até a instrução
```

A camada `rmr/` não substitui `OPENSSL_cpuid_setup`, `OPENSSL_armcap_P`,
`OPENSSL_ia32cap_P`, providers, EVP ou assembly upstream. Ela cria um contrato
externo para observar e testar a passagem:

```text
flag de compilação → macro ISA → objeto → seção ELF → símbolo → KAT → recibo
```

## Camadas

1. `include/rmr_silicon.h`: ABI fixa, bits de capacidade e política.
2. `core/rmr_silicon.c`: referência C freestanding, seleção/equivalência em tempo constante, endian e zeroização.
3. `arch/*/rmr_mix32.S`: mesma transformação por instruções reais x86_64/AArch64/ARMv7.
4. `linker/rmr_sections.ld`: seções `.rmr.text`, `.rmr.manifest`, `.rmr.rodata`, `.rmr.kat`.
5. `tests/kat.c`: equivalência C/ASM e invariantes de ABI.
6. `tools/openssl_probe.c`: adaptador hospedado que lê somente APIs públicas do OpenSSL.
7. `tools/audit_contract.sh`: hashes e recibo do estado comprovado.

## Máscara de silício

O campo `compile_caps` diferencia:

- arquitetura e largura de palavra;
- endianidade;
- SSE2, SSSE3, AES-NI, PCLMUL, SHA, AVX2 e AVX-512;
- NEON, Crypto, AES, SHA2, SHA3, CRC32, SVE e SVE2;
- extensões RISC-V Zbb, Zbc e Vector;
- presença do backend ASM no link.

Esses bits representam o que foi habilitado para o objeto pelo compilador.
Não representam automaticamente aquilo que existe no aparelho.

## Política de build

O campo `build_policy` é separado da capacidade física e registra:

- freestanding;
- sem libc;
- sem heap;
- sem builtins;
- sem stack protector;
- function/data sections;
- visibilidade oculta;
- `--gc-sections`;
- `--build-id=none`;
- LTO quando explicitamente habilitado;
- ABI fixa.

Esta separação impede confundir:

```text
recurso do silício ≠ decisão do compilador ≠ decisão do linker
```

## Linkers

A compilação cross gera, para x86_64, AArch64 e ARMv7:

```text
core freestanding + backend ASM → link relocável → objeto fechado
```

O gate exige zero símbolos externos indefinidos. O script aditivo de linker
preserva o manifesto e o texto RMR sem substituir a topologia oficial do
OpenSSL.

## Fronteiras

- Compilar objeto para uma ISA não prova execução naquele silício.
- Uma macro do compilador prova capacidade habilitada no objeto, não presença no aparelho.
- `OPENSSL_INFO_CPU_SETTINGS` é observação hospedada, não substitui KAT nem benchmark.
- Nenhum claim de aceleração, segurança superior, FIPS ou produção é permitido sem evidência específica.
- ASIC/FPGA exige RTL, síntese, timing, área, potência e equivalência formal; permanece hipótese.
