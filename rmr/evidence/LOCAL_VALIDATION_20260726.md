# Local validation — 2026-07-26

## Environment

```text
host: Linux x86_64 6.12.13
cc: GCC 14.2.0
clang: 17.0.0
lld: 17.0.0
```

## Commands

```sh
cd rmr
sh tools/audit_contract.sh
make probe
```

## Result

```text
status=PASS_LIMITED
host_kat_c=PASS
host_kat_asm=PASS
freestanding_object=PASS
cross_arch_linked_objects=PASS
undefined_symbols=ZERO
linker_sections=PASS
device_execution=TOKEN_VAZIO
openssl_full_build=TOKEN_VAZIO
silicon_benchmark=TOKEN_VAZIO
asic_fpga_synthesis=TOKEN_VAZIO
```

KATs:

```text
C:       caps=0000000000000151 policy=0000000000000be4 fold=5b9a1170
x86_64:  caps=0000000080000151 policy=0000000000000be4 fold=db921178
```

The `0x80000000` delta records that the architecture-specific ASM symbol was
linked. The baseline host mask records x86_64, little-endian, 64-bit word and
SSE2 as compiler-visible capabilities.

## Object sizes

```text
AArch64 linked: 440 bytes text/data/bss aggregate reported by size
ARMv7 linked:   544 bytes
x86_64 linked:  532 bytes
host pure C:    439 bytes
```

## SHA-256 receipts

```text
0d3d2709c509c87c80c9d1267985998674af6aa3ec660fbf698e7ea32a2b25b8  rmr_core.aarch64.o
7bd8d3523457b310c51da746b6a381b860f11a16e4d112aeca1f01ab78b191f1  rmr_core.armv7.o
6ed86f0effa1ecff654f77dde0edab57034a255ef80a9e466855c38f7642acca  rmr_core.x86_64.o
b3bf6578186dbd8f8d87e9bd02fc74cbb62ace6911b3ddda7a377dca6b987984  rmr_mix32.aarch64.o
c578e422d4702b60c4910edefdcd5a7749eb7b33d683ea0e2699998594e9fe63  rmr_mix32.armv7.o
ed0156a518e436fbf4017a04505e9c23dc3f0771da737fde6fe848c5acbc53f0  rmr_mix32.x86_64.o
25894a7292ad028333d35b750346233b9238c7783114d12f452fe24e15b96dc1  rmr_silicon.aarch64.linked.o
e168a81d85470e518184654fd97d5b448e41c6f9db2e9324ed3fab50d4a5c9c4  rmr_silicon.armv7.linked.o
c2f518a957ed3882f188d984528d1a06f05fc5c1e6b61545e99b36d398c7b83e  rmr_silicon.freestanding.o
7320624ddb0490b5e1c57133c2dd4b23f60ab762758586bb67fa6e1f5dce27a9  rmr_silicon.x86_64.linked.o
20bf1bd8a5ee29a07ec112fdb00ee515c01e401d00ecdd7c317d882969962625  rmr_kat_c
73d63b1273f5b07183eda29afd91707da1688471e469f822b668946fbd9b7f8e  rmr_kat_x86_64
```

These hashes are local receipts, not reproducible-build claims: compiler path,
version, environment and linker can change object bytes.

## Public OpenSSL probe

The hosted probe read:

```text
OpenSSL version: 3.5.5
OPENSSL_INFO_CPU_SETTINGS: available
```

This confirms the adapter path against the system OpenSSL only. It does not
prove a full build or runtime of this repository fork.
