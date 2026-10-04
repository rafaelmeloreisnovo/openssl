# START HERE — OpenSSL fork provenance and reconstruction

Status: `FORK_LOCAL_NAVIGATION`  
Observed: `2026-10-04`  
Repository: `rafaelmeloreisnovo/openssl`  
Local branch authority: `master`

This file is a **fork-local navigation overlay**. It does not replace OpenSSL documentation, licensing, copyright notices, security policy, or upstream release guidance.

## 1. Authority map

| Question | Authority |
|---|---|
| What is OpenSSL? | upstream `openssl/openssl` documentation |
| What license governs inherited OpenSSL content? | `LICENSE.txt` plus applicable file-level notices |
| Who authored inherited OpenSSL code/docs? | upstream project and original copyright holders |
| What is local to this fork? | only deltas supported by commit/path/blob provenance |
| What records the fork authorship boundary? | `THIRD_PARTY_NOTICES_RAFAELIA.md` |
| How should local build/install work be interpreted? | upstream docs + local delta evidence; neither implies upstream endorsement |

`security-maintainer` in the local README is an **operational governance role for this fork**. It is not a statement of ownership over inherited OpenSSL material.

## 2. Current divergence snapshot

Provider readback on 2026-10-04 observed:

```text
fork                    = true
upstream/source         = openssl/openssl
upstream branch         = master
local master            = 9d493a9006d5428d2d44bcb30afc3cff38d1af7e
upstream compared head  = 4d25710dbfeacbb36d055592a3bc6811172248b1
merge base              = 971b8d060e52499d6ffd2f9ca697fe23f72a629a
compare status          = diverged
local ahead_by          = 74 commits
local behind_by         = 939 commits
```

This is a **measurement**, not a recommendation to merge, rebase, or discard either side. The counts are time-bound and must be re-read before any synchronization decision.

## 3. Provenance invariant

```text
FORK_CONTROL != AUTHORSHIP_OF_UPSTREAM
LOCAL_COMMIT != WHOLE_REPOSITORY_AUTHORSHIP
UPSTREAM_LICENSE != REVOKED_BY_LOCAL_NOTICE
SOURCE != ARTIFACT != EXECUTION != EVIDENCE != CLAIM
TOKEN_VAZIO != 0
```

For inherited or modified upstream files, the upstream Apache-2.0 license and file-level notices remain controlling unless a specific file supplies a compatible additional notice. A separate RAFAELIA license is **not inferred** merely because a local commit exists.

## 4. Lowest-risk reconstruction route

1. Freeze the exact local and upstream refs being compared.
2. Inventory the local delta by path and commit; do not infer ownership from repository control.
3. Classify each affected path as `UPSTREAM_INHERITED`, `UPSTREAM_MODIFIED`, `LOCAL_ADDED`, `GENERATED`, or `TOKEN_VAZIO_LINEAGE`.
4. Resolve the applicable license/copyright notice at file level before redistribution or relicensing claims.
5. If synchronization is intended, perform it on a candidate branch only.
6. Run the relevant OpenSSL build/test gates for the changed surface.
7. Record the exact refs, commands, results, failures, and rollback point in a receipt before promotion.

Do **not** use a blind upstream merge as a provenance mechanism. A green build after synchronization would prove only the executed build/test scope; it would not prove authorship, legal clearance, FIPS validation, or production suitability.

## 5. Fast routes

- Official project overview/build/docs: `README.md`, `INSTALL.md`, `doc/`, and upstream `openssl/openssl`.
- License authority: `LICENSE.txt`.
- Local third-party/authorship boundary: `THIRD_PARTY_NOTICES_RAFAELIA.md`.
- Upstream contribution rules: `CONTRIBUTING.md`.
- Security-sensitive work: preserve upstream security guidance and never treat this fork as the official OpenSSL distribution.

## 6. Current gaps

```text
FILE_LEVEL_LOCAL_DELTA_RIGHTS_MATRIX = TOKEN_VAZIO
SYNC_CANDIDATE_VALIDATION            = NOT_RUN
CURRENT_UPSTREAM_RECONCILIATION      = NOT_RUN
WHOLE_REPOSITORY_AUTHORSHIP_CLAIM    = false
```

The next high-value step is **not more code**: it is a bounded file-level inventory of the 74 local commits against the frozen merge base, with license/provenance classification before any upstream synchronization.

R3=<F_ok: upstream/fork/license/divergence route is explicit, F_gap: file-level delta rights and sync validation remain TOKEN_VAZIO/NOT_RUN, F_next: inventory local delta by path+commit before touching upstream synchronization>
