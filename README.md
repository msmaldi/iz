# Projeto iz

Bem-vindo ao projeto iz!

## Descrição

Este projeto é uma aplicação para um simples compilador.

## Compilação

```
 ┌───────────────────┐
 |      source       | input source file
 └───────────────────┘
           ↓
 ┌───────────────────┐
 |  syntax analysis  | syntax analysis lex and parse source and transform
 └───────────────────┘ in AST (declaration, statement, expression and types)
           ↓
 ┌───────────────────┐ semantic analysis check AST for errors:
 |                   | - all branchs have return
 | semantic analysis | - return type mismatch
 |                   | - redefinition of identifier
 └───────────────────┘ - undeclared identifier
           ↓
 ┌───────────────────┐
 |    llvm codegen   | generate llvm-ir and performe optimization
 └───────────────────┘
           ↓
 ┌───────────────────┐
 |   object files    | compiler current generate files .ll, .s and .o
 └───────────────────┘

```














