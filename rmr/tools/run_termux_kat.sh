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

for tool in make cc nm readelf sha256sum uname sed date dirname; do
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

make clean
make kat-c
make kat
make freestanding
make audit-host

EVP_KAT_STATUS=TOKEN_VAZIO
if [ "${RMR_RUN_EVP_KAT:-0}" = "1" ]; then
    make evp-kat
    EVP_KAT_STATUS=PASS_LIMITED
fi

BENCH_STATUS=TOKEN_VAZIO
if [ "${RMR_RUN_BENCH:-0}" = "1" ]; then
    make bench
    BENCH_STATUS=MEASURED_LOCAL_NO_COMPARISON
fi

C_KAT=$(build/rmr_kat_c)
ASM_KAT=TOKEN_VAZIO
ASM_BIN="build/rmr_kat_${ARCH}"
if [ -x "$ASM_BIN" ]; then
    ASM_KAT=$($ASM_BIN)
fi

OPENSSL_VERSION=TOKEN_VAZIO
if command -v openssl >/dev/null 2>&1; then
    OPENSSL_VERSION=$(openssl version 2>/dev/null || true)
    [ -n "$OPENSSL_VERSION" ] || OPENSSL_VERSION=TOKEN_VAZIO
fi

CC_ID=$(cc --version 2>/dev/null | sed -n '1p')
STAMP=$(date -u +%Y-%m-%dT%H:%M:%SZ)
RECEIPT=build/termux_device_receipt.txt

{
    echo "status=PASS_LIMITED"
    echo "observed_at_utc=$STAMP"
    echo "device_runtime=TERMUX_ANDROID"
    echo "device_arch=$ARCH"
    echo "android_abi=$ABI"
    echo "android_sdk=$SDK"
    echo "compiler=$CC_ID"
    echo "openssl_cli=$OPENSSL_VERSION"
    echo "native_c_kat=PASS"
    if [ "$ASM_KAT" = TOKEN_VAZIO ]; then
        echo "native_asm_kat=TOKEN_VAZIO"
    else
        echo "native_asm_kat=PASS"
    fi
    echo "hosted_evp_kat=$EVP_KAT_STATUS"
    echo "evp_benchmark=$BENCH_STATUS"
    echo "freestanding_object=PASS"
    echo "undefined_symbols=ZERO"
    echo "linker_sections=PASS"
    echo "c_kat_output=$C_KAT"
    echo "asm_kat_output=$ASM_KAT"
    echo "full_openssl_fork_build=TOKEN_VAZIO"
    echo "performance_comparison=TOKEN_VAZIO"
    echo "asic_fpga_synthesis=TOKEN_VAZIO"
} > "$RECEIPT"

{
    sha256sum build/rmr_kat_c
    if [ -x "$ASM_BIN" ]; then
        sha256sum "$ASM_BIN"
    fi
    sha256sum build/rmr_silicon.freestanding.o
    if [ -x build/rmr_openssl_evp_kat ]; then
        sha256sum build/rmr_openssl_evp_kat
    fi
    if [ -x build/rmr_openssl_evp_bench ]; then
        sha256sum build/rmr_openssl_evp_bench
    fi
    if [ -f build/evp_bench.csv ]; then
        sha256sum build/evp_bench.csv
    fi
    sha256sum "$RECEIPT"
} > build/TERMUX_SHA256SUMS

cat "$RECEIPT"
echo "receipt=$ROOT/$RECEIPT"
echo "hashes=$ROOT/build/TERMUX_SHA256SUMS"
