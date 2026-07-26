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

A lógica corrente do branch foi reconstruída e executada novamente após o
endurecimento do fold do manifesto e a separação `audit-host`/`audit-cross`.

## Result

```text
status=PASS_LIMITED
host_kat_c=PASS
host_kat_asm=PASS
freestanding_object=PASS
cross_arch_linked_objects=PASS
undefined_symbols=ZERO
linker_sections=PASS
immutable_manifest_fold=PASS
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

The immutable zero-capability template now contains fold `f11a0efa`, derived by
the same function used for materialized manifests; the KAT rejects any mismatch.

## Object sizes

```text
AArch64 linked: 440 bytes text/data/bss aggregate reported by size
ARMv7 linked:   544 bytes
x86_64 linked:  532 bytes
host pure C:    439 bytes
```

## SHA-256 receipts after manifest hardening

```text
fa0af44ea085b0bdbe71b7a33b2e8f2c5ab230cda1252ba701329634a8489117  rmr_core.aarch64.o
738fe00d59e60feb8e25f75222573b0c2f260f9638b36fdc23441cad50fa0069  rmr_core.armv7.o
7acaae6f968a2fcd386b4c067a9fa8fdd61138e2176dfe8ef031b7e00bbca6ac  rmr_core.x86_64.o
b3bf6578186dbd8f8d87e9bd02fc74cbb62ace6911b3ddda7a377dca6b987984  rmr_mix32.aarch64.o
c578e422d4702b60c4910edefdcd5a7749eb7b33d683ea0e2699998594e9fe63  rmr_mix32.armv7.o
ed0156a518e436fbf4017a04505e9c23dc3f0771da737fde6fe848c5acbc53f0  rmr_mix32.x86_64.o
c8c8c7020df825a891a16c6ae04192147a9dd08e736de2218f56e4e03d0bcae7  rmr_silicon.aarch64.linked.o
7d034511567ca2f0fa3c2d8a912e8668eebd1808419f29fc2b4e866ce0c65be5  rmr_silicon.armv7.linked.o
339192200f31c5e48736cfe785101d80594878d8d11add0aa4b5bc38ccfa0438  rmr_silicon.freestanding.o
bfac0fec218f18d7792a27b39daabfb780d0587ba2e0afd24709fa6952f4b9fb  rmr_silicon.x86_64.linked.o
63184fd9ebe8bc8a8d2a4869ed0ff5b20e9a348811b0d7a5d7edfd7e10b18cab  rmr_kat_c
9574077599bc15253dc2b3777146657c2055d4def4a2508275393cf68178ab0f  rmr_kat_x86_64
```

These hashes are local receipts, not reproducible-build claims: compiler path,
version, environment and linker can change object bytes. The GitHub Actions
artifact remains the independent CI receipt when its current run completes.

## Native Android gate

`tools/run_termux_kat.sh` now performs the host-only subset on the actual
Android/Termux ABI and emits:

```text
build/termux_device_receipt.txt
build/TERMUX_SHA256SUMS
```

The gate is implemented, but no device receipt is claimed in this validation:

```text
native_android_execution=TOKEN_VAZIO
```

## Public OpenSSL probe

The hosted probe read:

```text
OpenSSL version: 3.5.5
OPENSSL_INFO_CPU_SETTINGS: available
```

This confirms the adapter path against the system OpenSSL only. It does not
prove a full build or runtime of this repository fork.
