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

O script executa:

```text
C nativo
↔ ASM nativo da arquitetura
↔ objeto freestanding
↔ zero símbolos externos
↔ seções ELF
↔ recibo + SHA-256
```

Ele registra arquitetura, ABI Android, SDK, compilador e resultado dos KATs,
mas não coleta serial, Android ID, IMEI ou outro identificador pessoal.

## Saída local

```text
build/rmr_kat_c
build/rmr_kat_<host>
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
- benchmark no silício-alvo;
- ganho sobre OpenSSL;
- validação FIPS;
- síntese ASIC/FPGA;
- conformidade ISO.

O gate Termux promove somente:

```text
objeto cross-compiled
→ KAT nativo executado na ABI observada
```

Os demais estados permanecem `TOKEN_VAZIO` até o respectivo gate produzir evidência.
