# RAFCODE provenance boundary

Status: THIRD_PARTY_UPSTREAM_WITH_LOCAL_AUTHORIAL_OVERLAY

This repository contains inherited OpenSSL source, history, documentation, tests and build logic. That inherited material remains attributable to its original authors and governed by the repository's Apache-2.0 license and applicable notices.

RAFCODE authorship is limited to independently written files under `rafcode/` and explicitly named RAFCODE workflow deltas. Fork ownership does not transfer authorship of inherited OpenSSL material.

The L0 leaf in `rafcode/l0/` is a non-cryptographic capability gate. It is not a cipher, digest, MAC, KDF, RNG, signature implementation, provider replacement or security claim.

Required evidence chain:

`upstream ref -> local path -> local diff -> compile gate -> symbol gate -> receipt -> claim`

Missing execution or security evidence remains `TOKEN_VAZIO`.

`SOURCE != ARTIFACT != EXECUTION != EVIDENCE != CLAIM`
