# EVP benchmark observation — 2026-07-26

## Protocol

```text
algorithms: SHA2-256, SHA3-256
input sizes: 64 B, 1 KiB, 16 KiB, 1 MiB
warmups per case: 5
samples per case: 21
clock: CLOCK_MONOTONIC_RAW
statistics: p50, p95, p99
input: deterministic 1 MiB static buffer
```

Each sample processes a fixed batch. The number of operations is reduced for
larger inputs so that most cases process approximately 4 MiB per sample.

## One local observation

```csv
algorithm,bytes_per_op,ops_per_sample,samples,p50_ns,p95_ns,p99_ns,p50_mib_s
SHA2-256,64,4096,21,395158,417001,421647,632.658
SHA2-256,1024,4096,21,2576928,2745899,3110291,1552.236
SHA2-256,16384,256,21,2335418,2474625,2509927,1712.755
SHA2-256,1048576,4,21,2324953,2345342,2401046,1720.465
SHA3-256,64,4096,21,1309420,1351302,1403159,190.924
SHA3-256,1024,4096,21,8921466,9386118,12118716,448.357
SHA3-256,16384,256,21,8233551,8454761,8662269,485.817
SHA3-256,1048576,4,21,8271067,8448942,8523423,483.614
```

## Interpretation boundary

This is a local observation against the installed OpenSSL 3.5.5 provider. It is
not a comparison with this fork, BLAKE3, another compiler, another provider or
another device.

```text
measurement_harness=IMPLEMENTED
local_measurement=OBSERVED
reproduction_on_same_host=PARTIAL
termux_arm_measurement=TOKEN_VAZIO
upstream_vs_rmr_comparison=TOKEN_VAZIO
performance_gain_claim=BLOCKED
```

A performance claim requires at least:

- identical host, thermal state and governor;
- pinned OpenSSL/provider build and compiler flags;
- repeated independent runs;
- p50/p95/p99 and variance;
- output equality/KAT before timing;
- raw CSV and artifact hashes;
- explicit baseline and falsifier.
