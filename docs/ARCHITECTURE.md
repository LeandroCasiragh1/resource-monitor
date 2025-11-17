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

Design mínimo:

- Cada componente (cpu, memory, io, namespace, cgroup) tem um módulo em `src/` e um header correspondente em `include/`.
- Cada componente fornece uma API C mínima (por exemplo, `rp_run`, `namespace_list_for_pid`, `cgroup_read_metrics`) e um binário/CLI wrapper em `src/*_main.c`.
- Build: `Makefile` central compila artefatos em `bin/` (por exemplo `bin/resource-monitor`, `bin/resource-profiler`).

Camadas e fluxo de dados:

- Instrumentação (User-space): módulos em `src/` leem interfaces do kernel em `/proc` e `/sys/fs/cgroup`.
- Export: cada módulo pode exportar CSV/JSON para análise posterior (scripts em `scripts/` fornecem transformação/visualização).
- Experimentos: diretório `docs/experiments/` conterá metodologia, tabelas de resultados e gráficos (gerados por `scripts/visualize.py`).

Decisões de design iniciais

- Linguagem: C (compatibilidade com C23 exigida; atual skeleton utiliza C11/posix para portabilidade inicial).
- Evitar bibliotecas externas não-padrão: usar apenas stdlib e syscalls via leitura de arquivos em `/proc` e `/sys`.
- Testes: simples tests unitários em `tests/` — futuros testes de integração exigirão ambiente Linux com permissões (WSL ou VM).

Extensões futuras

- Adicionar suporte a cgroups v1 e v2 de forma explícita (detecção automática).
- Gerar JSON estruturado para facilitar ingestão por ferramentas de análise.
- Adicionar CI (GitHub Actions) para compilar e executar testes em runners Linux.

Documentar experimentos e metodologia nesta pasta conforme exigido.
