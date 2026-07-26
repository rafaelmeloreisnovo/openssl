# Hosted EVP capability validation — 2026-07-26

## Scope

This is a hosted adapter validation against the locally installed OpenSSL
library. It validates public EVP/provider APIs and known-answer vectors. It does
not prove that the full repository fork was built, nor that any algorithm used
hardware acceleration.

## Environment observation

```text
OpenSSL: 3.5.5
RMR compile caps: 0000000000000151
RMR build policy: 0000000000000be4
```

## Output

```text
digest=SHA2-256 status=PASS provider=default bytes=32
digest=SHA3-256 status=PASS provider=default bytes=32
cipher=AES-128-GCM status=AVAILABLE provider=default key_bits=128 block_bytes=1
cipher=AES-256-GCM status=AVAILABLE provider=default key_bits=256 block_bytes=1
cipher=CHACHA20-POLY1305 status=AVAILABLE provider=default key_bits=256 block_bytes=1
cipher=AES-128-CTR status=AVAILABLE provider=default key_bits=128 block_bytes=1
RMR_OPENSSL_EVP_KAT PASS_LIMITED
```

The digest vectors are the standard `abc` outputs for SHA-256 and SHA3-256.
Cipher availability was observed through `EVP_CIPHER_fetch`; no encryption key,
plaintext or sensitive material was processed.

## Binary receipt

```text
e38607cd0b4479e0ba2bc137a72a9953bf6974269e8c70006251cbd21f5ff534  rmr_openssl_evp_kat
```

This SHA-256 is a local custody receipt, not a reproducible-build claim.

## Boundaries

```text
public_evp_api=TESTED_HOSTED_LIMITED
sha2_256_kat=PASS
sha3_256_kat=PASS
cipher_fetch=OBSERVED_LOCAL
provider_default=OBSERVED_LOCAL
fork_full_build=TOKEN_VAZIO
hardware_acceleration=TOKEN_VAZIO
performance_gain=TOKEN_VAZIO
fips_validation=NOT_CLAIMED
```
