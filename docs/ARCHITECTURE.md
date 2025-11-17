# Architecture Overview

Este documento descreve a arquitetura inicial do `resource-monitor`.

- `src/`: implementações em C dos coletores e ferramentas.
- `include/`: interfaces públicas (headers).
- `tests/`: testes unitários mínimos.
- `scripts/`: scripts auxiliares em Python/Shell.

Design mínimo:

- cada componente (cpu, memory, io, namespace, cgroup) tem um módulo em `src/` e um header em `include/`.
- `main.c` fornece CLI simples para invocar os componentes.

Documentar experimentos e metodologia nesta pasta conforme exigido.
