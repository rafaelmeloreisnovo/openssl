#!/usr/bin/env sh
set -eu

cd "$(dirname "$0")/.."

need()
{
    command -v "$1" >/dev/null 2>&1 || {
        echo "missing_tool=$1" >&2
        exit 2
    }
}

for tool in cmake ctest ninja sha256sum grep; do
    need "$tool"
done

rm -rf out/build/portable-release out/build/host-native-release

cmake --preset portable-release
cmake --build --preset portable-release
ctest --preset portable-release

cmake --preset host-native-release
cmake --build --preset host-native-release
ctest --preset host-native-release

PORTABLE_DB=out/build/portable-release/compile_commands.json
NATIVE_DB=out/build/host-native-release/compile_commands.json

grep -q -- '-O3' "$PORTABLE_DB"
grep -q -- '-O3' "$NATIVE_DB"
grep -q -- '-march=native' "$NATIVE_DB"

NATIVE_DIR=out/build/host-native-release
BENCH_STATUS=TOKEN_VAZIO
if [ -x "$NATIVE_DIR/rmr_openssl_evp_bench" ]; then
    "$NATIVE_DIR/rmr_openssl_evp_bench" > "$NATIVE_DIR/evp_bench.csv"
    BENCH_STATUS=MEASURED_LOCAL_NO_COMPARISON
fi

{
    find out/build/portable-release out/build/host-native-release \
        -maxdepth 1 -type f \
        \( -name 'rmr_*' -o -name 'librmr_*.a' -o -name 'evp_bench.csv' \) \
        -print | sort | while IFS= read -r artifact; do
            sha256sum "$artifact"
        done
} > "$NATIVE_DIR/CMAKE_SHA256SUMS"

{
    echo "status=PASS_LIMITED"
    echo "canonical_build_system=CMAKE"
    echo "portable_configure=PASS"
    echo "portable_build=PASS"
    echo "portable_ctest=PASS"
    echo "native_configure=PASS"
    echo "native_build=PASS"
    echo "native_ctest=PASS"
    echo "flag_O3=PASS"
    echo "flag_march_native=PASS"
    echo "ipo_requested=ON"
    echo "benchmark=$BENCH_STATUS"
    echo "performance_gain_claim=BLOCKED"
    echo "android_arm64_build=TOKEN_VAZIO"
    echo "android_armv7_build=TOKEN_VAZIO"
    echo "native_android_execution=TOKEN_VAZIO"
} > "$NATIVE_DIR/cmake_receipt.txt"

cat "$NATIVE_DIR/cmake_receipt.txt"
echo "flags=$NATIVE_DIR/rmr-cmake-flags-Release.txt"
echo "hashes=$NATIVE_DIR/CMAKE_SHA256SUMS"
