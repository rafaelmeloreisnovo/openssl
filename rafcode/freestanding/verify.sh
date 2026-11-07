#!/bin/sh
# SPDX-FileCopyrightText: 2026 Rafael Melo Reis
# SPDX-License-Identifier: MIT
# Host build factory only; none of these tools is a runtime dependency.
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
CC_BIN=${CC:-cc}
command -v "$CC_BIN" >/dev/null 2>&1 || { echo 'BLOCKED: missing C compiler'; exit 2; }
command -v nm >/dev/null 2>&1 || { echo 'BLOCKED: missing nm'; exit 2; }
TMP=$(mktemp -d "${TMPDIR:-/tmp}/rafcode-fs.XXXXXXXX")
trap 'rm -rf "$TMP"' EXIT HUP INT TERM

for OPT in -Os -O2 -O3; do
    "$CC_BIN" -std=c11 "$OPT" -Wall -Wextra -Werror -Wpedantic \
        -ffreestanding -fno-builtin -fno-stack-protector \
        -fno-asynchronous-unwind-tables -fno-unwind-tables \
        -c "$ROOT/src/raf_fs_core.c" -o "$TMP/core.o"
    "$CC_BIN" -std=c11 "$OPT" -Wall -Wextra -Werror -Wpedantic \
        -ffreestanding -fno-builtin -fno-stack-protector \
        -fno-asynchronous-unwind-tables -fno-unwind-tables \
        -c "$ROOT/src/raf_fs_codec.c" -o "$TMP/codec.o"

    # No unresolved objects allowed in either standalone unit.
    test -z "$(nm -u "$TMP/core.o")" || { echo 'FAIL: unresolved core symbol'; exit 1; }
    test -z "$(nm -u "$TMP/codec.o")" || { echo 'FAIL: unresolved codec symbol'; exit 1; }

    # Five intended C API names only; no libc helpers.
    EXPECTED='raf_fs_decode raf_fs_encode raf_fs_init raf_fs_rollback raf_fs_transition'
    FOUND=$(nm -g --defined-only "$TMP/core.o" "$TMP/codec.o" | awk '$2 ~ /^[TW]$/ {print $3}' | sort | tr '\n' ' ' | sed 's/ $//')
    test "$FOUND" = "$EXPECTED" || {
        echo "FAIL: wrong exported functions: $FOUND"
        exit 1
    }

    # Hosted test executable is ONLY a test fixture, not the freestanding artifact.
    "$CC_BIN" -std=c11 "$OPT" -Wall -Wextra -Werror -Wpedantic \
        "$ROOT/src/raf_fs_core.c" "$ROOT/src/raf_fs_codec.c" \
        "$ROOT/tests/host_test.c" -o "$TMP/host-test"
    "$TMP/host-test" || { echo "FAIL: negative test ($OPT)"; exit 1; }
    echo "PASS scoped host+object $CC_BIN $OPT"
done
echo 'RAFCODE_FS_HOST_SCOPED_PASS; CRYPTO/ANDROID=TOKEN_VAZIO'
