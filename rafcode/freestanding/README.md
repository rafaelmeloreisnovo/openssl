# RAFCODE Ω — L1 freestanding autoral, isolado do OpenSSL

**Escopo:** camada de controle e transporte de estado **não criptográfica**. Não é implementação de SSL/TLS, cifra, hash, KDF, RNG, assinatura, provider nem substituto auditado do OpenSSL. Não chamar isto de biblioteca OpenSSL independente.

## Dependências por fase

- **Fonte/objeto L1:** C11 sem libc, headers hospedados, malloc, printf, syscalls ou referências externas não resolvidas (verificador `nm -u`).
- **Fábrica/CI:** requer compilador C11, `nm`, `clang`/`readelf` para cross objects e um sistema host para executar o harness. Dependência de **compilação/testes ≠ dependência de runtime**.
- **Execução real:** hardware, ABI, integridade de transporte, tratamento de erros e chamadas do kernel ficam sob responsabilidade de um *adapter* explícito, fora deste core. Este diretório não inclui adaptador de syscall nem integração em libcrypto. `TOKEN_VAZIO` para runtime Android/ARM.
- **Thread safety:** single-owner; não é API atômica ou sincronizada.
- **Persistência:** `raf_fs_encode/decode` transportam 24 bytes com tag de versão e verificações de domínio, **sem autenticação/integridade criptográfica**. Fonte não confiável exige uma camada externa autenticada antes de aceitar comandos.

## Arquitetura modular
```
include/raf_fs.h           → interface C11 (void*, word, state)
src/raf_fs_core.c          → permissões, transição, rollback
src/raf_fs_codec.c         → formato little-endian explícito (24 bytes)
tests/host_test.c          → controle adversarial somente no host
verify.sh                  → GCC/Clang, -Os / -O2 / -O3, nm -u + harness
verify_cross.sh            → objetos ARMv7/AArch64 via Clang (não executa ARM)
LICENSE-MIT                → licença apenas do novo diretório
LICENSE_SCOPE.md           → atribuição upstream × original por caminho
```

## Flags e o papel de cada ferramenta

`-ffreestanding -fno-builtin` limita pressupostos do compilador e chamadas implícitas. `-std=c11 -Wall -Wextra -Werror -Wpedantic` audita linguagem. `-Os/-O2/-O3` são **hipóteses de perfil**, nunca garantias de velocidade ou tamanho. O gate mede artefatos e só promove reivindicação de ganho mediante benchmark pareado por dispositivo e ref.

**Pré-processador:** `#if/#define` e flags `-D` são operações reais; comentários comuns são descartados lexicalmente, **não** ordenam ao compilador o alinhamento dos bits. **Compilador:** transformação/reordenação deve preservar o comportamento observável segundo C11; otimizações potencialmente inseguras precisam de teste. **Linker:** eliminação de seções (`-ffunction-sections -fdata-sections` + `--gc-sections` onde suportado) depende do binário final e de raízes de retenção explicitadas, nunca será inferida de comentários.

**ASM inline e hex/binário:** nenhum caminho de execução troca C por asm por estética. Um backend ASM só poderá entrar como módulo separado, `#if` por arquitetura, com prova de ABI, equivalência de testes, análise de símbolos, benchmark e rollback. Micro-otimização prematura em código de segurança pode aumentar fricção e risco.

## Executar os gates (em checkout do branch)
```sh
CC=gcc sh rafcode/freestanding/verify.sh
CC=clang sh rafcode/freestanding/verify.sh
sh rafcode/freestanding/verify_cross.sh
```

**Contrato:** `SOURCE != ARTIFACT != EXECUTION != EVIDENCE != CLAIM`. `PASS` do gate só certifica as verificações exibidas; não valida segurança criptográfica, Android nem desempenho. `TOKEN_VAZIO` permanece tipado.

Rollback: branch/PR draft não altera master; correção por novo commit e receipt de supersessão, sem apagar a história. A escolha MIT exige revisão do autor antes do merge.
