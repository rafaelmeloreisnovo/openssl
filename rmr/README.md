# RMR OpenSSL Silicon Contract

Camada externa, isolada e code-first para rastrear:

```text
flags → bits → ISA → ELF → linker → KAT → recibo
```

sem modificar o OpenSSL upstream.

## Execução no host/CI

```sh
cd rmr
make kat-c       # referência C hospedada
make kat         # referência C versus ASM da arquitetura hospedeira
make evp-kat     # SHA-256/SHA3 e disponibilidade de ciphers via EVP público
make bench       # medição opcional p50/p95/p99; não cria claim de ganho
make freestanding
make audit-host  # objeto local, símbolos e seções
make cross       # objetos ligados x86_64, AArch64 e ARMv7
make audit-cross
make audit
make probe       # opcional: API pública OpenSSL do sistema
```

Auditoria completa do host de desenvolvimento:

```sh
cd rmr
sh tools/audit_contract.sh
```

O `evp-kat` e o benchmark são hospedados e podem usar alocação interna do
OpenSSL. Eles validam a ponte pública de biblioteca; não alteram o contrato
freestanding do núcleo RMR.

## Execução nativa no Android/Termux

Este é o gate que transforma cross-compilação em evidência de execução na ISA do aparelho:

```sh
git clone https://github.com/rafaelmeloreisnovo/openssl.git
cd openssl
git checkout agent/rmr-openssl-silicon-v1-20260726
cd rmr
sh tools/run_termux_kat.sh
```

No Termux, instale apenas as ferramentas de construção quando faltarem:

```sh
pkg install clang make binutils coreutils
```

Para incluir a ponte EVP e a medição local, instale o OpenSSL do Termux e ative
explicitamente os gates mais pesados:

```sh
pkg install openssl
RMR_RUN_EVP_KAT=1 RMR_RUN_BENCH=1 sh tools/run_termux_kat.sh
```

O script executa por padrão:

```text
C nativo
↔ ASM nativo da arquitetura
↔ objeto freestanding
↔ zero símbolos externos
↔ seções ELF
↔ recibo + SHA-256
```

Com as variáveis opcionais, também produz o KAT EVP e o CSV p50/p95/p99.
Ele registra arquitetura, ABI Android, SDK, compilador e resultados, mas não
coleta serial, Android ID, IMEI ou outro identificador pessoal.

## Saída local

```text
build/rmr_kat_c
build/rmr_kat_<host>
build/rmr_openssl_evp_kat
build/rmr_openssl_evp_bench
build/evp_bench.csv
build/rmr_silicon.freestanding.o
build/rmr_silicon.{x86_64,aarch64,armv7}.linked.o
build/SHA256SUMS
build/receipt.txt
```

Saída adicional no aparelho:

```text
build/termux_device_receipt.txt
build/TERMUX_SHA256SUMS
```

## Limite

`PASS_LIMITED` não significa:

- build integral deste fork OpenSSL;
- comparação válida com outro algoritmo ou build;
- ganho sobre OpenSSL;
- validação FIPS;
- síntese ASIC/FPGA;
- conformidade ISO.

O gate Termux promove somente:

```text
objeto cross-compiled
→ KAT nativo executado na ABI observada
```

Uma execução de `make bench` promove somente `MEASURED_LOCAL_NO_COMPARISON`.
Os demais estados permanecem `TOKEN_VAZIO` até o respectivo gate produzir evidência.
