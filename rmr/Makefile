CC ?= cc
CLANG ?= clang
READELF ?= readelf
NM ?= nm
BUILD ?= build
HOST_ARCH := $(shell uname -m)

COMMON_WARN := -Wall -Wextra -Wpedantic -Werror
COMMON_OPT := -O3 -fvisibility=hidden -ffunction-sections -fdata-sections
FREESTANDING := -ffreestanding -fno-builtin -fno-stack-protector \
                -fno-asynchronous-unwind-tables -fno-unwind-tables
POLICY_DEFS := -DRMR_NO_HEAP=1 -DRMR_FUNCTION_SECTIONS=1 -DRMR_DATA_SECTIONS=1 \
               -DRMR_HIDDEN_VISIBILITY=1 -DRMR_GC_SECTIONS=1 -DRMR_BUILD_ID_NONE=1
FREESTANDING_DEFS := $(POLICY_DEFS) -DRMR_NO_LIBC=1 -DRMR_NO_BUILTIN=1 \
                     -DRMR_NO_STACK_PROTECTOR=1
CORE := core/rmr_silicon.c
INC := -Iinclude

ifeq ($(HOST_ARCH),x86_64)
HOST_ASM := arch/x86_64/rmr_mix32.S
else ifeq ($(HOST_ARCH),aarch64)
HOST_ASM := arch/aarch64/rmr_mix32.S
else ifneq (,$(filter armv7l armv8l,$(HOST_ARCH)))
HOST_ASM := arch/armv7/rmr_mix32.S
else
HOST_ASM :=
endif

.PHONY: all kat kat-c evp-kat bench freestanding audit audit-host audit-cross cross probe clean
all: kat-c kat freestanding cross audit

$(BUILD):
	mkdir -p $(BUILD)

kat-c: | $(BUILD)
	$(CC) $(COMMON_WARN) $(COMMON_OPT) $(POLICY_DEFS) $(INC) \
		$(CORE) tests/kat.c -Wl,--gc-sections -Wl,--build-id=none \
		-Wl,-T,linker/rmr_sections.ld -o $(BUILD)/rmr_kat_c
	$(BUILD)/rmr_kat_c

kat: | $(BUILD)
ifeq ($(HOST_ASM),)
	@echo "host ASM backend unsupported for $(HOST_ARCH): TOKEN_VAZIO"
else
	$(CC) $(COMMON_WARN) $(COMMON_OPT) $(POLICY_DEFS) -DRMR_USE_ARCH_ASM=1 $(INC) \
		$(CORE) $(HOST_ASM) tests/kat.c \
		-Wl,--gc-sections -Wl,--build-id=none -Wl,-T,linker/rmr_sections.ld \
		-o $(BUILD)/rmr_kat_$(HOST_ARCH)
	$(BUILD)/rmr_kat_$(HOST_ARCH)
endif

evp-kat: | $(BUILD)
	$(CC) $(COMMON_WARN) $(COMMON_OPT) $(POLICY_DEFS) $(INC) \
		$(CORE) tools/openssl_evp_kat.c -lcrypto \
		-o $(BUILD)/rmr_openssl_evp_kat
	$(BUILD)/rmr_openssl_evp_kat

bench: | $(BUILD)
	$(CC) $(COMMON_WARN) $(COMMON_OPT) $(POLICY_DEFS) $(INC) \
		$(CORE) tools/openssl_evp_bench.c -lcrypto \
		-o $(BUILD)/rmr_openssl_evp_bench
	$(BUILD)/rmr_openssl_evp_bench > $(BUILD)/evp_bench.csv
	cat $(BUILD)/evp_bench.csv
	sha256sum $(BUILD)/rmr_openssl_evp_bench $(BUILD)/evp_bench.csv \
		> $(BUILD)/EVP_BENCH_SHA256SUMS

freestanding: | $(BUILD)
	$(CC) $(COMMON_WARN) $(COMMON_OPT) $(FREESTANDING) $(FREESTANDING_DEFS) \
		$(INC) -c $(CORE) -o $(BUILD)/rmr_silicon.freestanding.o

cross: | $(BUILD)
	$(CLANG) --target=x86_64-linux-gnu $(COMMON_OPT) $(FREESTANDING) \
		$(FREESTANDING_DEFS) -DRMR_USE_ARCH_ASM=1 $(INC) -c $(CORE) \
		-o $(BUILD)/rmr_core.x86_64.o
	$(CLANG) --target=x86_64-linux-gnu -c arch/x86_64/rmr_mix32.S \
		-o $(BUILD)/rmr_mix32.x86_64.o
	$(CLANG) --target=x86_64-linux-gnu -nostdlib -no-pie -fuse-ld=lld -Wl,-r \
		$(BUILD)/rmr_core.x86_64.o $(BUILD)/rmr_mix32.x86_64.o \
		-o $(BUILD)/rmr_silicon.x86_64.linked.o

	$(CLANG) --target=aarch64-linux-gnu -march=armv8-a+crypto+crc+simd \
		$(COMMON_OPT) $(FREESTANDING) $(FREESTANDING_DEFS) \
		-DRMR_USE_ARCH_ASM=1 $(INC) -c $(CORE) -o $(BUILD)/rmr_core.aarch64.o
	$(CLANG) --target=aarch64-linux-gnu -march=armv8-a+crypto+crc+simd \
		-c arch/aarch64/rmr_mix32.S -o $(BUILD)/rmr_mix32.aarch64.o
	$(CLANG) --target=aarch64-linux-gnu -nostdlib -no-pie -fuse-ld=lld -Wl,-r \
		$(BUILD)/rmr_core.aarch64.o $(BUILD)/rmr_mix32.aarch64.o \
		-o $(BUILD)/rmr_silicon.aarch64.linked.o

	$(CLANG) --target=armv7a-linux-gnueabi -march=armv7-a -mfpu=neon-vfpv4 \
		-mfloat-abi=softfp $(COMMON_OPT) $(FREESTANDING) $(FREESTANDING_DEFS) \
		-DRMR_USE_ARCH_ASM=1 $(INC) -c $(CORE) -o $(BUILD)/rmr_core.armv7.o
	$(CLANG) --target=armv7a-linux-gnueabi -march=armv7-a -mfpu=neon-vfpv4 \
		-mfloat-abi=softfp -c arch/armv7/rmr_mix32.S -o $(BUILD)/rmr_mix32.armv7.o
	$(CLANG) --target=armv7a-linux-gnueabi -nostdlib -no-pie -fuse-ld=lld -Wl,-r \
		$(BUILD)/rmr_core.armv7.o $(BUILD)/rmr_mix32.armv7.o \
		-o $(BUILD)/rmr_silicon.armv7.linked.o

probe: | $(BUILD)
	$(CC) $(COMMON_WARN) $(COMMON_OPT) $(POLICY_DEFS) $(INC) \
		$(CORE) tools/openssl_probe.c -lcrypto -o $(BUILD)/rmr_openssl_probe
	$(BUILD)/rmr_openssl_probe

audit-host: freestanding
	@echo "== host freestanding undefined symbols (must be empty) =="
	@test -z "$$($(NM) -u $(BUILD)/rmr_silicon.freestanding.o)"
	@echo "== host RMR sections =="
	@$(READELF) -SW $(BUILD)/rmr_silicon.freestanding.o | grep -E '\.rmr\.(text|manifest|rodata)'

audit-cross: cross
	@echo "== linked cross objects undefined symbols (must be empty) =="
	@for f in $(BUILD)/rmr_silicon.*.linked.o; do \
		test -z "$$($(NM) -u $$f)" || { echo "undefined in $$f"; $(NM) -u $$f; exit 1; }; \
	done
	@echo "== linked cross RMR sections =="
	@for f in $(BUILD)/rmr_silicon.*.linked.o; do \
		$(READELF) -SW $$f | grep -q '\.rmr.text' || exit 1; \
	done

audit: audit-host audit-cross

clean:
	rm -rf $(BUILD)
