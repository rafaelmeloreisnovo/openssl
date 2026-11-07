#!/bin/sh
# SPDX-FileCopyrightText: 2026 Rafael Melo Reis
# SPDX-License-Identifier: MIT
# Clang cross-object evidence, not hardware execution.
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
command -v clang >/dev/null 2>&1 || { echo 'BLOCKED: missing clang'; exit 2; }
command -v readelf >/dev/null 2>&1 || { echo 'BLOCKED: missing readelf'; exit 2; }
TMP=$(mktemp -d "${TMPDIR:-/tmp}/rafcode-fs-cross.XXXXXXXX")
trap 'rm -rf "$TMP"' EXIT HUP INT TERM
for TARGET in armv7a-linux-gnueabi aarch64-linux-gnu; do
    for SRC in raf_fs_core raf_fs_codec; do
        OUT="$TMP/$SRC-$TARGET.o"
        clang --target="$TARGET" -std=c11 -Os -Wall -Wextra -Werror \
            -ffreestanding -fno-builtin -fno-stack-protector \
            -c "$ROOT/src/$SRC.c" -o "$OUT"
        # Verify the actual ELF machine, not just successful compiler exit.
        case "$TARGET" in
          armv7a-linux-gnueabi) EXPECT='Machine:.*ARM' ;;
          aarch64-linux-gnu)  EXPECT='Machine:.*AArch64' ;;
        esac
        readelf -h "$OUT" | grep -Eq "$EXPECT" || { echo "FAIL: wrong ELF machine"; exit 1; }
        if readelf -Ws "$OUT" | awk '$7 == "UND" && $8 != "" {bad=1} END {exit !bad}'; then
            echo "FAIL: external undefined symbol in $OUT"
            exit 1
        fi
    done
    echo "PASS cross-ELF structural $TARGET"
done
echo 'RAFCODE_FS_CROSS_OBJECT_PASS; DEVICE_RUNTIME=TOKEN_VAZIO'
