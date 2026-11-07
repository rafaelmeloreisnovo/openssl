# RAFCODE Ω — autoria isolada e fronteira de licenças (proposta V2)

**Estado atualizado:** PR #9 mesclada ao `master` (merge `0b1c6fcd8d753aebb3af98900804f3128486e618`) em 2026-11-07. O texto MIT e os SPDX aplicam-se aos arquivos novos explicitados; sua verificação de proveniência por arquivo permanece pendente. Este documento não reatribui o material upstream.

| Escopo por caminho | Origem | Licença aplicável | Observações |
|---|---|---|---|
| Repositório herdado de openssl/openssl, `LICENSE.txt`, `NOTICE` e arquivos próprios do upstream | Terceiros/OpenSSL | **Apache-2.0**, salvo exceções/avisos por arquivo | Preservar textos e atribuições. O fork não transfere autoria. |
| `rafcode/l0/**` existente no `master` | Contribuição RAFCODE pré-existente | **Apache-2.0** (SPDX já presente) | **Não relicenciado** por esta proposta. |
| `rafcode/freestanding/**` criado nesta proposta | Código novo escrito independentemente para RAFCODE, sob responsabilidade do autor | **MIT**, texto integral em `rafcode/freestanding/LICENSE-MIT` | A licença está no código integrado; escopo isolado por arquivos, não abrange upstream nem outras contribuições. |
| `.github/workflows/rafcode-freestanding-v2.yml` novo | Script de validação RAFCODE | **MIT** (SPDX no arquivo) | É automação de teste, não código OpenSSL. |

`MIT` é uma **licença permissiva pública declarada para os novos arquivos do núcleo autoral**; não é uma alegação de licença exclusiva, de autoria sobre a árvore inteira, nem uma licença personalizada RAFCODE. Alterar sua escolha exige decisão do titular dos direitos e revisão de compatibilidade antes de nova publicação ou relicenciamento. Caso haja código misturado ou derivado de terceiros, a classificação `MIT` deve ser revista por proveniência *por arquivo*.

**Separação:** duas licenças coexistem por escopo; não se afirma que o mesmo arquivo é dual-licensed automaticamente. A distribuição combinada deve cumprir os avisos aplicáveis de ambas. Não remover `LICENSE.txt` nem `NOTICE` herdados, nem revisar licenças de componentes externos sem inventário e autorização.

**Ordem de verificação:** upstream original → hash/base commit do fork → arquivos adicionados → revisão de derivação/autoria → gate objeto sem símbolos indefinidos → receipt → revisão jurídica/autoral → decisão humana de merge.

Não foi realizada auditoria completa de licenças de todas as centenas de arquivos do fork. Resultado global: `TOKEN_VAZIO`; a separação desta PR é escopada.
