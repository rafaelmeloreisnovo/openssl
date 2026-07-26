# RMR OpenSSL Silicon Contract

Camada externa, isolada e code-first para rastrear:

```text
flags → bits → ISA → ELF → linker → KAT → recibo
```

sem modificar o OpenSSL upstream.

## Execução

```sh
cd rmr
make kat-c       # referência C hospedada
make kat         # referência C versus ASM da arquitetura hospedeira
make freestanding
make cross       # objetos ligados x86_64, AArch64 e ARMv7
make audit
make probe       # opcional: API pública OpenSSL do sistema
```

Ou:

```sh
cd rmr
sh tools/audit_contract.sh
```

## Saída local

```text
build/rmr_kat_c
build/rmr_kat_<host>
build/rmr_silicon.freestanding.o
build/rmr_silicon.{x86_64,aarch64,armv7}.linked.o
build/SHA256SUMS
build/receipt.txt
```

## Limite

`PASS_LIMITED` não significa:

- execução Android/Termux em ARM;
- benchmark no silício-alvo;
- ganho sobre OpenSSL;
- validação FIPS;
- síntese ASIC/FPGA;
- conformidade ISO.

Esses estados permanecem `TOKEN_VAZIO` até o respectivo gate produzir evidência.
