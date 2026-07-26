# RMR OpenSSL Silicon Contract

Camada externa, isolada e code-first para rastrear:

```text
CMake preset
→ flags reais
→ bits de capacidade
→ fonte C/ASM selecionada
→ ISA
→ ELF/linker
→ KAT
→ benchmark
→ recibo
```

sem modificar o OpenSSL upstream.

## Build canônico: CMake

O `Makefile` permanece como auditoria legada e matriz cross de objetos. O caminho
canônico de compilação otimizada é agora o CMake, seguindo a disciplina já usada
no fork BLAKE3: flags por arquitetura, fontes SIMD/ASM selecionadas por target,
configuração substituível por cache/toolchain e benchmark separado do KAT.

### Portável

```sh
cd rmr
cmake --preset portable-release
cmake --build --preset portable-release
ctest --preset portable-release
```

### Máquina atual — máxima especialização local

```sh
cmake --preset host-native-release
cmake --build --preset host-native-release
ctest --preset host-native-release
```

Esse preset usa:

```text
-O3
-march=native
-mtune=native
IPO/LTO quando suportado
hidden visibility
function/data sections
--gc-sections
--build-id=none
```

O binário `host-native` deve permanecer no mesmo processador ou em CPU
compatível. Ele não é o artefato de distribuição universal.

### Perfis explícitos

| Perfil | ISA/flags principais | Uso |
|---|---|---|
| `portable-release` | `-O3`, sem ISA especial e sem ASM | baseline e CI portável |
| `host-native-release` | `-march=native -mtune=native` | desempenho no host atual |
| `x86_64-v3-release` | `-march=x86-64-v3` | x86-64 com AVX2/BMI2/FMA |
| `aarch64-crypto` | `-march=armv8-a+crypto+crc+simd` | ARM64 crypto/CRC/NEON |
| `armv7-neon` | `-march=armv7-a -mfpu=neon-vfpv4 -mfloat-abi=softfp` | ARM32/armeabi-v7a |

O ajuste de microarquitetura é opcional e separado da ISA:

```sh
cmake --preset termux-release -DRMR_CPU_TUNE=<cpu-confirmada>
```

Não existe `-ffast-math`: ele não melhora primitivas inteiras e enfraqueceria o
contrato sem benefício legítimo.

### Auditoria completa do CMake

```sh
sh tools/audit_cmake_contract.sh
```

Ela executa:

```text
portable configure/build/CTest
+ native configure/build/CTest
+ verificação de -O3
+ verificação de -march=native
+ KAT C↔ASM
+ EVP KAT
+ benchmark p50/p95/p99
+ manifesto de flags
+ SHA-256 dos artefatos
```

O recibo de flags fica em:

```text
out/build/<preset>/rmr-cmake-flags-Release.txt
```

## Android NDK

Com `ANDROID_NDK_HOME` definido:

```sh
cmake --preset android-arm64-release
cmake --build --preset android-arm64-release

cmake --preset android-armv7-release
cmake --build --preset android-armv7-release
```

Esses presets compilam as bibliotecas para as ABIs Android. Não executam os KATs
no aparelho; execução física pertence ao gate Termux.

## Execução nativa no Android/Termux

```sh
git clone https://github.com/rafaelmeloreisnovo/openssl.git
cd openssl
git checkout agent/rmr-openssl-silicon-v1-20260726
cd rmr

pkg install clang cmake ninja binutils coreutils
sh tools/run_termux_kat.sh
```

O script usa obrigatoriamente:

```text
CMake preset termux-release
→ -O3
→ perfil automático da ABI
→ ASM nativo
→ IPO quando suportado
→ CTest
→ auditoria ELF
→ recibo + hashes
```

Para incluir EVP e benchmark no aparelho:

```sh
pkg install openssl
RMR_RUN_EVP_KAT=1 RMR_RUN_BENCH=1 sh tools/run_termux_kat.sh
```

Ele registra arquitetura, ABI Android, SDK, compilador, flags e resultados, mas
não coleta serial, Android ID, IMEI ou outro identificador pessoal.

## Targets gerados

```text
librmr_silicon_c.a           referência C freestanding
librmr_silicon.a             C + backend ASM selecionado
rmr_kat_c                     KAT da referência
rmr_kat                       KAT do backend selecionado
rmr_openssl_probe             diagnóstico por API pública
rmr_openssl_evp_kat           SHA2/SHA3 + providers
rmr_openssl_evp_bench         p50/p95/p99
```

## Limite

`PASS_LIMITED` não significa:

- build integral deste fork OpenSSL;
- comparação válida com o BLAKE3 autoral;
- ganho sobre OpenSSL ou BLAKE3 upstream;
- validação FIPS;
- síntese ASIC/FPGA;
- conformidade ISO.

A cadeia correta é:

```text
flags_present
≠ flags_applied
≠ optimized_object_built
≠ KAT_passed
≠ device_executed
≠ benchmark_reproduced
≠ faster_than_baseline
```

Uma execução do benchmark promove somente `MEASURED_LOCAL_NO_COMPARISON`. O
claim de superioridade exige os dois binários, mesma máquina, mesma entrada,
mesmo protocolo, hashes, p50/p95/p99 e repetição independente.
