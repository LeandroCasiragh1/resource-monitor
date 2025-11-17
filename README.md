# Resource Monitor

Sistema completo de monitoramento de recursos do sistema com suporte a cgroups v2, namespaces e experimentos de controle de recursos.

## Funcionalidades

### Monitoramento em Tempo Real
- **CPU**: Uso percentual, tempo de usuário/sistema, idle
- **Memória**: Total, usado, disponível, cache, buffers, swap
- **I/O**: Leituras/escritas por segundo, operações completadas
- **Rede**: Taxa de RX/TX, pacotes, erros

### Controle de Recursos com Cgroups v2
- Limitação de CPU (quota/period e weight)
- Limitação de memória (max, high watermark)
- Limitação de I/O (read/write bps)
- Estatísticas de uso por cgroup

### Isolamento com Namespaces
- PID: Isolamento de processos
- NET: Stack de rede isolada
- MNT: Pontos de montagem isolados
- UTS: Hostname isolado
- IPC: Inter-process communication isolado
- USER: Mapeamento de usuários

### Experimentos
1. **Overhead de Monitoramento**: Mede o impacto do monitoramento no desempenho
2. **CPU Throttling**: Testa limitação de CPU com cgroups
3. **Memory Limit**: Testa comportamento sob pressão de memória
4. **I/O Limit**: Testa limitação de I/O em disco
5. **Namespace Isolation**: Verifica isolamento de namespaces

### Visualização
- Gráficos automáticos com Python/Matplotlib
- Exportação em CSV para análise
- Gráficos salvos como PNG de alta resolução

## Requisitos

### Sistema Operacional
- Linux kernel 4.5+ (para cgroups v2 completo)
- Ubuntu 20.04+ / Debian 11+ / Fedora 31+ recomendado

### Build
```bash
sudo apt-get install build-essential libncurses-dev pkg-config
```

### Visualização (opcional)
```bash
pip3 install -r requirements.txt
# ou
pip3 install pandas matplotlib numpy
```

### Permissões
- Root ou sudo para operações com cgroups
- Permissões de leitura em /proc e /sys/fs/cgroup

## Instalação

### Compilação Rápida
```bash
./build.sh
```

### Compilação Manual
```bash
make clean
make
```

### Instalação no Sistema
```bash
sudo make install
```

## Uso

### Menu Interativo (TUI)
```bash
./bin/monitor menu
```

Navegação:
- Setas UP/DOWN: Navegar entre opções
- Números 1-5: Selecionar diretamente
- ENTER: Confirmar seleção
- Q: Sair

### Monitoramento de CPU
```bash
# Menu interativo: Opção 1 > CPU Monitor
# Ou via linha de comando:
./bin/monitor cpu 60  # 60 segundos
```
Gera: `output/cpu_monitor.csv`

### Monitoramento de Memória
```bash
# Menu interativo: Opção 1 > Memory Monitor
./bin/monitor memory 60
```
Gera: `output/memory_monitor.csv`

### Monitoramento de I/O
```bash
# Menu interativo: Opção 1 > I/O Monitor
./bin/monitor io sda 60  # Monitorar device sda por 60s
```
Gera: `output/io_monitor.csv`

### Monitoramento de Rede
```bash
# Menu interativo: Opção 1 > Network Monitor
./bin/monitor network eth0 60  # Interface eth0 por 60s
```
Gera: `output/network_monitor.csv`

### Análise de Namespaces
```bash
# Menu interativo: Opção 2
./bin/monitor namespace <PID>
```
Mostra todos os namespaces do processo especificado.

### Gerenciamento de Cgroups
```bash
# Menu interativo: Opção 3

# Via linha de comando:
sudo ./bin/cgroup_manager create my_group
sudo ./bin/cgroup_manager set-cpu my_group 50000 100000  # 50% CPU
sudo ./bin/cgroup_manager set-memory my_group 512M
sudo ./bin/cgroup_manager add-process my_group 1234
sudo ./bin/cgroup_manager delete my_group
```

### Experimentos
```bash
# Menu interativo: Opção 4 > Selecionar experimento

# Overhead de monitoramento
sudo ./bin/monitor experiment overhead

# CPU throttling (50% por 30 segundos)
sudo ./bin/monitor experiment cpu-throttle 50 30

# Memory limit (256 MB)
sudo ./bin/monitor experiment memory-limit 256

# I/O limit (50 MB/s por 30 segundos)
sudo ./bin/monitor experiment io-limit 50 30

# Namespace isolation (PID namespace)
sudo ./bin/monitor experiment namespace 0
```

Resultados salvos em: `output/experiments/`

### Visualização de Dados
```bash
# Gerar todos os gráficos
python3 scripts/visualize.py output/experiments output/graphs

# Gráfico específico
python3 scripts/visualize.py output output/graphs
```

Gráficos gerados:
- `cpu_usage.png`: Uso de CPU ao longo do tempo
- `memory_usage.png`: Uso de memória
- `io_stats.png`: Estatísticas de I/O
- `network_stats.png`: Estatísticas de rede
- Gráficos de experimentos individuais

## Estrutura do Projeto

```
resource-monitor/
├── src/                    # Código fonte
│   ├── monitor_tui.c      # Interface TUI principal
│   ├── cpu_monitor.c      # Monitor de CPU
│   ├── memory_monitor.c   # Monitor de memória
│   ├── io_monitor.c       # Monitor de I/O
│   ├── network_monitor.c  # Monitor de rede
│   ├── cgroup_v2.c        # API de cgroups v2
│   ├── process_monitor.c  # Monitor de processos
│   ├── namespace_analyzer.c # Análise de namespaces
│   ├── utils.c            # Funções utilitárias
│   ├── experiments.c      # Framework de experimentos
│   ├── experiment_overhead.c
│   ├── experiment_cpu_throttling.c
│   ├── experiment_memory_limit.c
│   └── experiment_io_limit.c
├── include/               # Headers
│   ├── monitors.h
│   ├── cgroup_v2.h
│   ├── process_monitor.h
│   ├── experiments.h
│   └── utils.h
├── scripts/               # Scripts utilitários
│   └── visualize.py       # Geração de gráficos
├── bin/                   # Binários compilados
│   ├── monitor            # Programa principal
│   └── cgroup_manager     # Gerenciador de cgroups
├── output/                # Dados de saída
│   ├── experiments/       # Resultados de experimentos
│   └── graphs/            # Gráficos gerados
├── Makefile              # Sistema de build
├── build.sh              # Script de build automatizado
├── requirements.txt      # Dependências Python
└── README.md             # Esta documentação
```

## Exemplos de Uso

### Monitorar um Processo Específico
```bash
# Encontrar PID do processo
ps aux | grep firefox

# Monitorar CPU e memória
./bin/monitor process 1234 60
```

### Experimento Completo
```bash
# 1. Executar experimentos
sudo ./bin/monitor menu
# Selecionar: 4 (Experiments) > 1-5 (cada experimento)

# 2. Visualizar resultados
python3 scripts/visualize.py output/experiments output/graphs

# 3. Ver gráficos
xdg-open output/graphs/cpu_usage.png
```

### Pipeline de CI/CD
```bash
#!/bin/bash
# Executar testes automatizados
./build.sh
sudo ./bin/monitor experiment overhead
sudo ./bin/monitor experiment cpu-throttle 50 30
python3 scripts/visualize.py output/experiments output/graphs
# Verificar resultados...
```

## Troubleshooting

### Erro: "Permission denied" ao criar cgroup
```bash
# Solução: Executar com sudo
sudo ./bin/monitor menu
```

### Erro: "cgroups v2 not available"
```bash
# Verificar se cgroups v2 está montado
mount | grep cgroup2

# Montar manualmente se necessário
sudo mount -t cgroup2 none /sys/fs/cgroup
```

### Erro: "Failed to open /proc/stat"
```bash
# Verificar permissões
ls -la /proc/stat

# Deve ser legível por todos
# -r--r--r-- 1 root root ...
```

### Gráficos não são gerados
```bash
# Instalar dependências Python
pip3 install pandas matplotlib numpy

# Verificar instalação
python3 -c "import pandas, matplotlib, numpy"
```

### Caracteres estranhos no terminal
```bash
# Usar terminal com suporte UTF-8
export LANG=en_US.UTF-8
export LC_ALL=en_US.UTF-8
```

## Desenvolvimento

### Adicionar Novo Monitor
1. Criar `src/my_monitor.c` e `include/my_monitor.h`
2. Implementar funções `read_my_stats()` e `monitor_my()`
3. Adicionar ao Makefile
4. Integrar no `monitor_tui.c`

### Adicionar Novo Experimento
1. Criar `src/experiment_my_test.c`
2. Definir estrutura de resultado em `include/experiments.h`
3. Implementar função `experiment_my_test()`
4. Adicionar opção no menu TUI

### Contribuir
1. Fork o repositório
2. Criar branch: `git checkout -b feature/my-feature`
3. Commit: `git commit -am 'Add new feature'`
4. Push: `git push origin feature/my-feature`
5. Criar Pull Request

## Referências

- [Linux Control Groups v2](https://www.kernel.org/doc/html/latest/admin-guide/cgroup-v2.html)
- [Linux Namespaces](https://man7.org/linux/man-pages/man7/namespaces.7.html)
- [proc(5) Manual](https://man7.org/linux/man-pages/man5/proc.5.html)
- [ncurses Programming Guide](https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/)

## Licença

MIT License - Veja LICENSE para detalhes

## Autor

Leandro Casiraghini
GitHub: LeandroCasiragh1/resource-monitor
