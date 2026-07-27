#!/usr/bin/env sh
set -eu

cd "$(dirname "$0")/.."

BUILD_DIR=${1:-out/build/portable-release}
KAT="$BUILD_DIR/rmr_sha256_uop_kat"
LIB="$BUILD_DIR/librmr_freestanding_sha256.a"
COMPILE_DB="$BUILD_DIR/compile_commands.json"
RECEIPT="$BUILD_DIR/sha256_uop_receipt.txt"
HASHES="$BUILD_DIR/SHA256_UOP_SHA256SUMS"

need()
{
    command -v "$1" >/dev/null 2>&1 || {
        echo "missing_tool=$1" >&2
        exit 2
    }
}

for tool in grep nm readelf sha256sum; do
    need "$tool"
done

[ -x "$KAT" ] || {
    echo "missing_kat=$KAT" >&2
    exit 3
}
[ -f "$LIB" ] || {
    echo "missing_library=$LIB" >&2
    exit 4
}
[ -f "$COMPILE_DB" ] || {
    echo "missing_compile_database=$COMPILE_DB" >&2
    exit 5
}

"$KAT"

grep -q -- '-ffreestanding' "$COMPILE_DB"
grep -q -- '-fno-builtin' "$COMPILE_DB"
grep -q -- '-fno-stack-protector' "$COMPILE_DB"
grep -q -- '-fno-optimize-sibling-calls' "$COMPILE_DB"
grep -q -- '-fno-common' "$COMPILE_DB"
grep -q -- '-Wshadow' "$COMPILE_DB"
grep -q -- '-Wconversion' "$COMPILE_DB"
grep -q -- '-Wsign-conversion' "$COMPILE_DB"
grep -q -- '-Wstrict-prototypes' "$COMPILE_DB"
grep -q -- '-Wmissing-prototypes' "$COMPILE_DB"
grep -q -- '-Wundef' "$COMPILE_DB"

if nm -u "$LIB" | grep -E '\b(malloc|calloc|realloc|free|memcpy|memset|pthread_|dlopen|dlsym|__cxa_|__stack_chk_)\b'; then
    echo 'forbidden_runtime_symbol=FOUND' >&2
    exit 6
fi

readelf -SW "$KAT" | grep -q '\.rmr\.modules'
readelf -SW "$KAT" | grep -q '\.rmr\.notes'
readelf -SW "$KAT" | grep -q '\.rmr\.rodata\.sha256'

sha256sum "$KAT" "$LIB" > "$HASHES"

{
    echo 'status=PASS_LOCAL_CONTRACT'
    echo 'sha256_empty_kat=PASS'
    echo 'sha256_abc_kat=PASS'
    echo 'uop16_mov_xor_push_pull_patch_jump=PASS'
    echo 'caller_owned_memory=PASS'
    echo 'heap_gc_runtime_symbols=ZERO'
    echo 'strict_warning_flags=PASS'
    echo 'tailcall_optimization=DISABLED'
    echo 'module_sections=PASS'
    echo 'comments_as_live_notes=EXPLICIT_RMR_NOTES'
    echo 'physical_parallel_issue=TOKEN_VAZIO_HARDWARE'
    echo 'openssl_tls_integration=TOKEN_VAZIO'
    echo "hashes=$HASHES"
} > "$RECEIPT"

cat "$RECEIPT"
