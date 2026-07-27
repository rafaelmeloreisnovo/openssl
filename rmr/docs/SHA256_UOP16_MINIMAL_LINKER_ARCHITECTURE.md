# RMR SHA-256 + UOP16 — arquitetura freestanding por blocos

**Estado:** `IMPLEMENTED / HOST_KAT_LOCAL / DEVICE_EXECUTION_TOKEN_VAZIO`  
**Escopo:** somente `rmr/**`; o OpenSSL upstream permanece intocado.

## 1. Invariante

```text
fonte e política
→ módulo mínimo
→ flags target-scoped
→ estado do caller
→ transformação bit-exata
→ seção de linker
→ KAT
→ receipt + SHA-256
```

A fábrica pode usar CMake, compilador, linker e ferramentas de auditoria. O
artefato RMR não recebe heap, GC, interpretador ou runtime externo por causa
disso.

## 2. Módulos

```text
rmr_module
├── descriptor fixo
├── flags semânticas
└── fold do contrato

rmr_microop
├── 16 registradores
├── stack fixa de 16 palavras
├── 16 passos máximos
├── MOV XOR OR AND ADD ROTR SHR
├── PATCH PUSH PULL JUMP
└── CH MAJ BSIG0 BSIG1

rmr_sha256
├── estado: struct rmr_sha256_ctx
├── scratch: struct rmr_sha256_workspace
├── 64 rodadas determinísticas
├── update/final/digest
└── vetores KAT vazio e abc
```

Não existe `malloc`, `calloc`, `realloc`, `free`, heap ou GC. O chamador fornece
o contexto e o workspace.

## 3. Patch seletivo

A troca semântica é:

```text
novo = atual XOR ((atual XOR valor) AND mascara)
```

Bits fora da máscara permanecem idênticos. Isso não é uma alegação de escrita
física de um bit isolado: RAM, cache, flash ou storage podem regravar palavra,
linha, página ou bloco.

## 4. JUMP e loops

`JUMP` existe, mas o executor realiza exatamente 16 passos por chamada.
Consequentemente:

```text
salto permitido
+ repetição possível dentro do bloco
+ receipt de slots e saltos
- loop infinito
- scheduler oculto
```

Os loops inevitáveis do SHA-256 são limites matemáticos explícitos: 16 palavras
de entrada, 64 palavras de agenda e 64 rodadas. Eles são fricção útil e
auditável, não repetição acidental.

## 5. Branchless

A ALU do micro-op calcula candidatos e seleciona por máscaras constantes para:

- `MOV`, `XOR`, `OR`, `AND`, `ADD`;
- `ROTR`, `SHR`;
- `PATCH`;
- `CH`, `MAJ`, `BSIG0`, `BSIG1`;
- escrita de registrador e stack;
- decisão de `PUSH`, `PULL` e `JUMP`.

Validação de ponteiro e controle de tamanho continuam usando branches quando
necessários para impedir acesso inválido. `branchless` não é aplicado quando
criaria comportamento indefinido ou mais instruções sem ganho provado.

## 6. Comentários vivos

Comentários C comuns são removidos antes do link. Portanto, não se declara que
eles aparecem magicamente no binário.

A concatenação auditável é explícita:

```text
.rmr.notes
→ strings arquiteturais mínimas

.rmr.modules
→ descritores binários fixos
```

O linker preserva somente esses metadados intencionais. Código permanece em
seções por função e pode ser eliminado por `--gc-sections`.

## 7. Flags do módulo estrito

Além do perfil RMR existente:

```text
-ffreestanding
-fno-builtin
-fno-stack-protector
-fno-optimize-sibling-calls
-fno-common
-fvisibility=hidden
-ffunction-sections
-fdata-sections
-fno-unwind-tables
-fno-asynchronous-unwind-tables
-Wall -Wextra -Wpedantic -Werror
-Wshadow
-Wconversion
-Wsign-conversion
-Wstrict-prototypes
-Wmissing-prototypes
-Wmissing-declarations
-Wundef
-Wl,--gc-sections
-Wl,--build-id=none
```

As flags são aplicadas somente ao alvo auditado. Não são despejadas globalmente
no OpenSSL.

## 8. Linker

`linker/rmr_modules.ld` agrega:

```text
.rmr.modules
.rmr.notes
.rmr.rodata.sha256
```

Somente módulos e notas usam `KEEP`. O código não usa `KEEP`, preservando a
eliminação de funções não alcançadas.

## 9. Prova

```bash
cd rmr
cmake --preset portable-release
cmake --build --preset portable-release
ctest --preset portable-release
sh tools/audit_sha256_uop_contract.sh out/build/portable-release
```

O gate verifica:

- KAT SHA-256 vazio;
- KAT SHA-256 `abc`;
- `MOV/XOR/PUSH/PULL/PATCH/JUMP`;
- flags estritas no `compile_commands.json`;
- ausência de símbolos de heap/GC/runtime na biblioteca;
- presença das três seções RMR;
- hashes do executável e da biblioteca.

## 10. Fronteiras honestas

```text
SHA-256 escalar freestanding: IMPLEMENTED
UOP16 lógico e limitado: IMPLEMENTED
paralelismo físico de 16 instruções: TOKEN_VAZIO_HARDWARE
backend SHA ARM/x86 dedicado: TOKEN_VAZIO
integração no TLS/SSL produtivo: TOKEN_VAZIO
mais rápido que OpenSSL upstream: NOT_CLAIMED
FIPS/ISO/certificação: NOT_CLAIMED
```

O próximo gate técnico é comparar o SHA-256 RMR com o EVP por vetores maiores,
streaming fragmentado, p50/p95/p99 e execução real ARMv7/AArch64 antes de criar
qualquer dispatch produtivo.
