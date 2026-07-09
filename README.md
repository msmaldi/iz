# Projeto iz

Bem-vindo ao projeto iz!

## Descrição

Este projeto é um compilador para "iz", uma pequena linguagem imperativa
estaticamente tipada, escrita em C. A implementação segue um pipeline
clássico de compilador: análise léxica/sintática, análise semântica, geração
de código via LLVM e emissão de arquivos objeto.

O projeto está em desenvolvimento incremental (versão atual: `0.0.4`, ver
`meson.build`), com amostras de código-fonte da linguagem em
`docs/samples/` e o roadmap de funcionalidades por versão em `docs/readme.md`.

### Exemplo (`docs/samples/v0.0.4.iz`)

```
bool is_true()
    return true;

bool is_leap_year(int year)
    return year % 4 == 0 && year % 100 != 0 || year % 400 == 0;

bool op_and(bool lhs, bool rhs)
    return lhs && rhs;
```

A linguagem atualmente suporta: tipos `int` e `bool`, funções com argumentos,
expressões (constantes, identificadores, binárias, chamadas, atribuição,
condicionais `&&`/`||`), declaração e atribuição de variáveis (`var`),
`if`/`else` e blocos.

## Estrutura do projeto

```
lib/            biblioteca principal do compilador (libiz)
  src/ast/      nós da AST: declaration, expression, statement, type, unit
  src/common/   utilitários: array genérico, source, span
  src/parser/   lexer e parser (análise léxica e sintática)
  src/sema/     análise semântica: escopos e checagem de tipos/erros
  test/         testes unitários (cmocka), espelhando a estrutura de src/

backend/llvm/   backend de geração de código LLVM (libllvmiz)
  src/llvm/     codegen de tipos, expressões, statements e funções para LLVM IR
  test/         testes do backend

src/            executável principal `iz` (ponto de entrada da CLI)
test/           testes de integração adicionais (ex.: sanity check da API LLVM)
docs/           roadmap (docs/readme.md) e amostras de código-fonte da linguagem
scripts/        scripts de build e cobertura (clang/gcc)
```

## Pipeline de compilação

```
 ┌───────────────────┐
 |      source       | arquivo fonte de entrada
 └───────────────────┘
           ↓
 ┌───────────────────┐
 |  syntax analysis  | análise léxica e sintática, transforma o source
 └───────────────────┘ em AST (declaration, statement, expression e types)
           ↓
 ┌───────────────────┐ análise semântica verifica a AST em busca de erros:
 |                   | - todos os caminhos possuem return
 | semantic analysis | - incompatibilidade de tipo de retorno
 |                   | - redefinição de identificador
 └───────────────────┘ - identificador não declarado
           ↓
 ┌───────────────────┐
 |    llvm codegen   | gera llvm-ir e realiza otimização
 └───────────────────┘
           ↓
 ┌───────────────────┐
 |   object files    | o compilador atualmente gera arquivos .ll, .s e .o
 └───────────────────┘
```

## Build and Install

Project is developed in ubuntu 24.04, to build and install:

``` bash
sudo apt install meson ninja-build clang llvm-dev libcmocka-dev lld pkg-config lcov
./scripts/build_clang.sh
sudo ninja -C builddir install
```

### Testes e cobertura

O projeto usa `cmocka` para testes unitários (em `lib/test/` e
`backend/llvm/test/`). Para rodar os testes com relatório de cobertura:

``` bash
./scripts/coverage_clang.sh
```

Também estão disponíveis `scripts/build_gcc.sh` e `scripts/coverage_gcc.sh`
para compilar/testar com GCC em vez de clang.
