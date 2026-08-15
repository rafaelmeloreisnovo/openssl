# Proveniência — OpenSSL upstream × RMR

## Fronteira de autoria

- `crypto/`, `ssl/`, `providers/`, `include/`, `Configurations/` e demais caminhos existentes permanecem OpenSSL upstream, sob suas licenças e autoria originais.
- `rmr/` é uma camada adicionada separadamente ao fork e não modifica o núcleo criptográfico upstream por esta integração.
- A independência autoral de `rmr/` **não é presumida pela separação de diretório, linguagem, volume de código ou reimplementação**. Cada componente deve ser verificado quanto à origem de sua concepção, estrutura, decomposição, lógica, interfaces e implementação específica.
- Até essa verificação componente a componente, a classificação de independência autoral de `rmr/**` é `UNKNOWN_ORIGIN/TOKEN_VAZIO_PER_COMPONENT`.
- O workflow RMR é escopado apenas para essa camada nova; isso demonstra isolamento de build, não independência autoral.

## Integração permitida nesta versão

O adaptador hospedado usa somente APIs públicas:

```text
OPENSSL_info(OPENSSL_INFO_CPU_SETTINGS)
OpenSSL_version_num()
OpenSSL_version()
```

A camada não lê nem altera diretamente `OPENSSL_armcap_P`,
`OPENSSL_ia32cap_P`, dispatch interno, providers ou tabelas de assembly.

## Não reivindicado

A camada RMR não reivindica autoria sobre:

- TLS, DTLS ou QUIC;
- EVP e providers;
- algoritmos criptográficos padronizados, incluindo a estrutura do SHA-256;
- detecção oficial de CPU;
- assembly upstream;
- validação FIPS;
- segurança ou desempenho do OpenSSL.

Implementar um padrão técnico como SHA-256 pode ser necessário para interoperabilidade, mas isso não demonstra por si só autoria independente da estrutura ou implementação usada. A fonte técnica/normativa e a genealogia da implementação devem ser registradas quando pertinentes.

Qualquer integração futura com internals exige patch separado, revisão de
licença, testes upstream e claim gate próprio. Consequências jurídicas específicas
que não possam ser determinadas apenas pela proveniência técnica ficam como
`LEGAL_REVIEW_REQUIRED`.
