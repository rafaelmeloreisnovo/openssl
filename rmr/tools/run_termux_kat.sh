#!/data/data/com.termux/files/usr/bin/sh
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
cd "$ROOT"

need()
{
    command -v "$1" >/dev/null 2>&1 || {
        echo "missing_tool=$1" >&2
        exit 2
    }
}

for tool in cmake ctest ninja cc nm readelf sha256sum uname sed date dirname grep; do
    need "$tool"
done

ARCH=$(uname -m)
ABI=TOKEN_VAZIO
SDK=TOKEN_VAZIO
if command -v getprop >/dev/null 2>&1; then
    ABI=$(getprop ro.product.cpu.abi 2>/dev/null || true)
    SDK=$(getprop ro.build.version.sdk 2>/dev/null || true)
    [ -n "$ABI" ] || ABI=TOKEN_VAZIO
    [ -n "$SDK" ] || SDK=TOKEN_VAZIO
fi

BUILD_DIR=out/build/termux-release
rm -rf "$BUILD_DIR"
cmake --preset termux-release
cmake --build --preset termux-release
ctest --preset termux-release
sh tools/audit_sha256_uop_contract.sh "$BUILD_DIR"

C_KAT=$($BUILD_DIR/rmr_kat_c)
ASM_KAT=$($BUILD_DIR/rmr_kat)
$BUILD_DIR/rmr_sha256_uop_kat

if [ -n "$(nm -u "$BUILD_DIR/librmr_silicon_c.a")" ]; then
    echo "freestanding_archive_has_undefined_symbols" >&2
    nm -u "$BUILD_DIR/librmr_silicon_c.a" >&2
    exit 1
fi
readelf -SW "$BUILD_DIR/librmr_silicon_c.a" | grep -q '\.rmr.text'
readelf -SW "$BUILD_DIR/librmr_silicon_c.a" | grep -q '\.rmr.manifest'

grep -q -- '-O3' "$BUILD_DIR/compile_commands.json"
case "$ARCH" in
    aarch64|arm64)
        grep -q -- '-march=armv8-a+crypto+crc+simd' "$BUILD_DIR/compile_commands.json"
        ;;
    armv7l|armv8l)
        grep -q -- '-march=armv7-a' "$BUILD_DIR/compile_commands.json"
        grep -q -- '-mfpu=neon-vfpv4' "$BUILD_DIR/compile_commands.json"
        ;;
    x86_64|amd64)
        grep -q -- '-march=x86-64-v3' "$BUILD_DIR/compile_commands.json"
        ;;
esac

EVP_KAT_STATUS=TOKEN_VAZIO
if [ "${RMR_RUN_EVP_KAT:-0}" = "1" ] &&
   [ -x "$BUILD_DIR/rmr_openssl_evp_kat" ]; then
    "$BUILD_DIR/rmr_openssl_evp_kat" > "$BUILD_DIR/evp_kat.txt"
    EVP_KAT_STATUS=PASS_LIMITED
fi

BENCH_STATUS=TOKEN_VAZIO
if [ "${RMR_RUN_BENCH:-0}" = "1" ] &&
   [ -x "$BUILD_DIR/rmr_openssl_evp_bench" ]; then
    "$BUILD_DIR/rmr_openssl_evp_bench" > "$BUILD_DIR/evp_bench.csv"
    BENCH_STATUS=MEASURED_LOCAL_NO_COMPARISON
fi

OPENSSL_VERSION=TOKEN_VAZIO
if command -v openssl >/dev/null 2>&1; then
    OPENSSL_VERSION=$(openssl version 2>/dev/null || true)
    [ -n "$OPENSSL_VERSION" ] || OPENSSL_VERSION=TOKEN_VAZIO
fi

CC_ID=$(cc --version 2>/dev/null | sed -n '1p')
STAMP=$(date -u +%Y-%m-%dT%H:%M:%SZ)
RECEIPT=$BUILD_DIR/termux_device_receipt.txt

{
    echo "status=PASS_LIMITED"
    echo "observed_at_utc=$STAMP"
    echo "canonical_build_system=CMAKE"
    echo "cmake_preset=termux-release"
    echo "device_runtime=TERMUX_ANDROID"
    echo "device_arch=$ARCH"
    echo "android_abi=$ABI"
    echo "android_sdk=$SDK"
    echo "compiler=$CC_ID"
    echo "openssl_cli=$OPENSSL_VERSION"
    echo "native_c_kat=PASS"
    echo "native_asm_kat=PASS"
    echo "native_sha256_uop_kat=PASS"
    echo "hosted_evp_kat=$EVP_KAT_STATUS"
    echo "evp_benchmark=$BENCH_STATUS"
    echo "flag_O3=PASS"
    echo "architecture_flags=PASS"
    echo "strict_sha256_uop_flags=PASS"
    echo "ipo_requested=ON"
    echo "freestanding_archive=PASS"
    echo "undefined_symbols=ZERO"
    echo "linker_sections=PASS"
    echo "c_kat_output=$C_KAT"
    echo "asm_kat_output=$ASM_KAT"
    echo "full_openssl_fork_build=TOKEN_VAZIO"
    echo "openssl_tls_integration=TOKEN_VAZIO"
    echo "performance_comparison=TOKEN_VAZIO"
    echo "physical_parallel_issue=TOKEN_VAZIO_HARDWARE"
    echo "asic_fpga_synthesis=TOKEN_VAZIO"
} > "$RECEIPT"

{
    sha256sum "$BUILD_DIR/rmr_kat_c"
    sha256sum "$BUILD_DIR/rmr_kat"
    sha256sum "$BUILD_DIR/rmr_sha256_uop_kat"
    sha256sum "$BUILD_DIR/librmr_silicon_c.a"
    sha256sum "$BUILD_DIR/librmr_silicon.a"
    sha256sum "$BUILD_DIR/librmr_freestanding_sha256.a"
    sha256sum "$BUILD_DIR/sha256_uop_receipt.txt"
    if [ -x "$BUILD_DIR/rmr_openssl_evp_kat" ]; then
        sha256sum "$BUILD_DIR/rmr_openssl_evp_kat"
    fi
    if [ -x "$BUILD_DIR/rmr_openssl_evp_bench" ]; then
        sha256sum "$BUILD_DIR/rmr_openssl_evp_bench"
    fi
    if [ -f "$BUILD_DIR/evp_kat.txt" ]; then
        sha256sum "$BUILD_DIR/evp_kat.txt"
    fi
    if [ -f "$BUILD_DIR/evp_bench.csv" ]; then
        sha256sum "$BUILD_DIR/evp_bench.csv"
    fi
    sha256sum "$BUILD_DIR/rmr-cmake-flags-Release.txt"
    sha256sum "$RECEIPT"
} > "$BUILD_DIR/TERMUX_SHA256SUMS"

cat "$RECEIPT"
echo "flags=$ROOT/$BUILD_DIR/rmr-cmake-flags-Release.txt"
echo "receipt=$ROOT/$RECEIPT"
echo "hashes=$ROOT/$BUILD_DIR/TERMUX_SHA256SUMS"
