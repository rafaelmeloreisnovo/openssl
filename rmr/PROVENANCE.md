# Proveniência — OpenSSL upstream × RMR

## Fronteira de autoria

- `crypto/`, `ssl/`, `providers/`, `include/`, `Configurations/` e demais caminhos existentes permanecem OpenSSL upstream, sob suas licenças e autoria originais.
- `rmr/` é camada autoral externa de Rafael Melo Reis, adicionada sem modificar o núcleo criptográfico.
- O workflow RMR é escopado apenas para a camada nova.

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
- algoritmos criptográficos;
- detecção oficial de CPU;
- assembly upstream;
- validação FIPS;
- segurança ou desempenho do OpenSSL.

Qualquer integração futura com internals exige patch separado, revisão de
licença, testes upstream e claim gate próprio.
