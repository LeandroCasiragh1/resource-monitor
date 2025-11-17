# 🐧 Resource Monitor - Containers e Recursos

> **Monitoramento de recursos Linux com suporte a namespaces, cgroups e experimentos de isolamento**

Este projeto implementa um profiler de recursos de sistema em C que permite estudar os mecanismos fundamentais de containers Linux.

## 📖 Descrição do Projeto

Sistema completo de monitoramento de processos Linux que demonstra na prática:

- **Monitoramento em tempo real** de processos (CPU, memória, I/O, rede)
- **Análise de namespaces** para validação de isolamento entre processos
- **Gerenciamento de cgroups v2** para limitação e controle de recursos
- **Cinco experimentos práticos** demonstrando conceitos de containerização
- **Interface TUI** com visualização gráfica de métricas em tempo real
- **Visualizações gráficas** geradas automaticamente com matplotlib

Desenvolvido como trabalho acadêmico (RA3) para a disciplina de Sistemas Operacionais.

## 🔧 Requisitos e Dependências

### Requisitos de Sistema

- **Sistema Operacional**: Linux (kernel 4.5+)
- **Arquitetura**: x86_64
- **Cgroups v2**: Habilitado no kernel
- **Privilégios**: Root necessário para experimentos 2-5 e gerenciamento de cgroups

### Dependências Obrigatórias

```bash
# Ubuntu/Debian
sudo apt-get install gcc make libncurses-dev util-linux iproute2 coreutils

# Fedora/RHEL
sudo dnf install gcc make ncurses-devel util-linux iproute coreutils

# Arch Linux
sudo pacman -S gcc make ncurses util-linux iproute2 coreutils
```

**Pacotes necessários:**
- `gcc` - Compilador C (GCC 9.0+)
- `make` - Sistema de build
- `ncurses` - Biblioteca para interface TUI
- `util-linux` - Ferramentas (unshare, nsenter)
- `iproute2` - Ferramentas de rede (ip)
- `coreutils` - Utilitários GNU (dd, cat, etc.)

### Dependências Opcionais (Visualização)

Para gerar gráficos automaticamente:

```bash
# Criar ambiente virtual Python
python3 -m venv venv
source venv/bin/activate

# Instalar dependências Python
pip install matplotlib numpy
```

### Verificar Suporte a Cgroups v2

```bash
# Verificar se cgroups v2 está montado
mount | grep cgroup2
# Saída esperada: cgroup2 on /sys/fs/cgroup type cgroup2 (rw,...)

# Verificar controllers disponíveis
cat /sys/fs/cgroup/cgroup.controllers
# Saída esperada: cpu memory io pids ...
```

## 🛠️ Instruções de Compilação

### Método 1: Build Rápido (Recomendado)

```bash
./build.sh
```

Este script:
1. Compila todo o código fonte
2. Cria diretórios de saída
3. Executa automaticamente o menu interativo

### Método 2: Build Manual

```bash
# Compilação limpa
make clean && make

# Apenas compilar (sem limpar)
make

# Limpar arquivos de build
make clean
```

**Saída da compilação:**
- `bin/monitor` - Binário principal (todas as funcionalidades)
- `bin/cgroup_manager` - Utilitário de gerenciamento de cgroups
- `obj/*.o` - Arquivos objeto intermediários

### Verificar Compilação

```bash
# Verificar binários criados
ls -lh bin/

# Testar execução
./bin/monitor --help
```

## 📚 Instruções de Uso

### Menu Interativo (Modo Recomendado)

```bash
./bin/monitor menu
```

Menu principal oferece:
1. **Resource Monitor** - TUI em tempo real
2. **Namespace Analyzer** - análise de isolamento
3. **Control Group Manager** - gerenciamento de cgroups
4. **Experimentos** - 1-5 + geração de visualizações

### Modo Linha de Comando

#### Monitoramento de Processos

```bash
# TUI - Interface em tempo real (pressione 'q' para sair)
./bin/monitor tui <PID>

# Exemplo: monitorar processo 1234
./bin/monitor tui 1234

# TUI com tempo determinado (5s de intervalo por 60s)
./bin/monitor tui 1234 5 60

# Monitoramento com exportação JSON
./bin/monitor process 1234 5 60 json

# Monitoramento com exportação CSV
./bin/monitor process 1234 5 60 csv
```

**Exemplo prático - Monitorar navegador:**

```bash
# Encontrar PID do Firefox
pgrep firefox

# Monitorar em tempo real
./bin/monitor tui $(pgrep firefox | head -1)
```

#### Análise de Namespaces

```bash
# Listar namespaces de um processo
./bin/monitor namespace list <PID>

# Comparar namespaces entre dois processos
./bin/monitor namespace compare <PID1> <PID2>

# Encontrar processos em um namespace específico
./bin/monitor namespace find /proc/1/ns/pid

# Gerar relatório completo do sistema
./bin/monitor namespace report

# Medir overhead de criação de namespaces
./bin/monitor namespace overhead
```

**Exemplo prático:**

```bash
# Comparar processo normal com processo em container
./bin/monitor namespace compare 1 $(pgrep dockerd)
```

#### Gerenciamento de Cgroups

```bash
# Modo interativo (requer root)
sudo ./bin/cgroup_manager

# Ou através do menu principal
sudo ./bin/monitor menu
# Escolha opção 3 (Control Group Manager)
```

#### Execução de Experimentos

```bash
# Experimento 1: Overhead de Monitoramento (sem root)
./bin/monitor experiment 1

# Experimento 2: Isolamento via Namespaces (requer root)
sudo ./bin/monitor experiment 2

# Experimento 3: CPU Throttling (requer root)
sudo ./bin/monitor experiment 3

# Experimento 4: Limite de Memória (requer root)
sudo ./bin/monitor experiment 4

# Experimento 5: Limite de I/O (requer root)
sudo ./bin/monitor experiment 5
```

### Exemplos de Uso Completos

#### Exemplo 1: Monitorar Servidor Web

```bash
# Iniciar servidor (exemplo)
python3 -m http.server 8000 &

# Obter PID
PID=$(pgrep -f "http.server")

# Monitorar em tempo real
./bin/monitor tui $PID

# Ou exportar métricas para análise
./bin/monitor process $PID 2 120 json
```

Saída: `output/process_monitoring.json`

#### Exemplo 2: Validar Isolamento de Container

```bash
# Comparar namespaces do sistema vs container Docker
sudo ./bin/monitor namespace compare 1 $(docker inspect -f '{{.State.Pid}}' <container_name>)
```

#### Exemplo 3: Limitar CPU de Processo

```bash
# Via menu interativo
sudo ./bin/monitor menu
# 1. Escolha opção 3 (Control Groups)
# 2. Escolha opção 4 (Criar cgroup)
# 3. Escolha opção 6 (Mover processo)
# 4. Escolha opção 7 (Aplicar limites)

# Ou execute o experimento 3 que demonstra isso
sudo ./bin/monitor experiment 3
```

#### Exemplo 4: Executar Todos os Experimentos e Gerar Visualizações

```bash
# Via menu
sudo ./bin/monitor menu
# Escolha opção 4 (Experimentos)
# Escolha opção 6 (Executar TODOS)
# Aguarde conclusão...
# Escolha opção 7 (Gerar visualizações)

# Visualizar gráficos gerados
ls -lh output/graphs/
# exp1_overhead.png
# exp1_context_switches.png
# exp1_execution_time.png
# exp3_cpu_usage.png
# exp4_memory_usage.png
# exp5_io_operations.png
```

#### Exemplo 5: Gerar Visualizações Manualmente

```bash
# Ativar ambiente virtual Python
source venv/bin/activate

# Gerar visualizações de todos os experimentos
venv/bin/python scripts/visualize.py --experiments output/graphs

# Gerar visualização de experimento específico
venv/bin/python scripts/visualize.py output/experiment1_overhead.csv output/graphs
```

### Estrutura de Saída

```
output/
├── experiment1_overhead.csv              # Dados do experimento 1
├── experiment3_cpu_throttling.csv        # Dados do experimento 3
├── experiment4_memory_limit.csv          # Dados do experimento 4
├── experiment5_io_limit.csv              # Dados do experimento 5
├── experiments/
│   └── exp2_namespace_isolation.json     # Dados do experimento 2
├── graphs/
│   ├── exp1_overhead.png                 # Gráficos gerados
│   ├── exp1_context_switches.png
│   ├── exp1_execution_time.png
│   ├── exp3_cpu_usage.png
│   ├── exp4_memory_usage.png
│   └── exp5_io_operations.png
└── process_monitoring.json               # Dados de monitoramento contínuo
```

## 📁 Estrutura do Projeto

```
resource-monitor/
├── bin/                         # Binários compilados
│   ├── monitor                  # Aplicação principal
│   └── cgroup_manager           # Utilitário de cgroups
├── obj/                         # Arquivos objeto (.o)
├── output/                      # Dados gerados
│   ├── graphs/                  # Gráficos PNG
│   └── experiments/             # Dados de experimentos
├── src/                         # Código-fonte
│   ├── main.c                   # Entry point
│   ├── monitor_tui.c            # Interface TUI
│   ├── resource_profiler.c      # Profiler de recursos
│   ├── namespace_analyzer.c     # Análise de namespaces
│   ├── cgroup_v2.c              # Gerenciamento de cgroups
│   ├── cgroup_manager.c         # CLI para cgroups
│   ├── experiments.c            # Implementação dos experimentos
│   ├── cpu_monitor.c            # Coleta de métricas CPU
│   ├── memory_monitor.c         # Coleta de métricas memória
│   ├── io_monitor.c             # Coleta de métricas I/O
│   ├── network_monitor.c        # Coleta de métricas rede
│   ├── utils.c                  # Funções auxiliares
│   └── process_monitor.c        # Monitoramento de processos
├── include/                     # Headers
│   ├── resource_profiler.h
│   ├── namespace.h
│   ├── cgroup.h
│   ├── monitor.h
│   └── utils.h
├── tests/                       # Testes
│   └── test_*.c
├── scripts/                     # Scripts auxiliares
│   ├── compare_tools.sh         # Comparação de ferramentas
│   └── visualize.py             # Geração de gráficos
├── docs/                        # Documentação
│   ├── QUICK_START.md           # Guia rápido
│   ├── ARCHITECTURE.md          # Arquitetura do sistema
│   └── EXPERIMENTS_REPORT.md    # Relatório dos experimentos
├── Makefile                     # Sistema de build
├── build.sh                     # Script de build rápido
└── README.md                    # Este arquivo
```

## 📊 Componentes Principais

| Componente | Arquivos | Descrição |
|-----------|----------|-----------|
| **Core do Monitor** | `src/main.c`, `src/monitor_tui.c` | Menu interativo, interface TUI, loop de monitoramento |
| **Coleta de Métricas** | `src/cpu_monitor.c`, `src/memory_monitor.c`, `src/io_monitor.c`, `src/network_monitor.c` | Leitura de dados do /proc e cálculos de uso |
| **Namespace Analyzer** | `src/namespace_analyzer.c` | Análise, comparação e relatórios de namespaces |
| **Cgroup Manager** | `src/cgroup_v2.c`, `src/cgroup_manager.c` | Gerenciamento de cgroups v2, aplicação de limites |
| **Experimento 1** | `src/experiment_overhead.c` | Medição de overhead de monitoramento |
| **Experimento 2** | `src/experiments.c` (namespace) | Validação de isolamento via namespaces |
| **Experimento 3** | `src/experiment_cpu_throttling.c` | Demonstração de CPU throttling |
| **Experimento 4** | `src/experiment_memory_limit.c` | Demonstração de limites de memória |
| **Experimento 5** | `src/experiment_io_limit.c` | Demonstração de limites de I/O |
| **Visualização** | `scripts/visualize.py` | Geração de gráficos com matplotlib |
| **Utilitários** | `src/utils.c`, `src/process_monitor.c` | Funções auxiliares e exportação de dados |

## 👥 Autor

Leandro Casiragh - Grupo 9

Este projeto foi desenvolvido individualmente como trabalho RA3 da disciplina de Sistemas Operacionais.

## 📄 Licença

Projeto acadêmico desenvolvido para fins educacionais - Sistemas Operacionais, 2025.

## 📞 Suporte

Para questões sobre o projeto:

- Consulte a documentação em `docs/`
- Verifique `docs/QUICK_START.md` para início rápido
- Leia `docs/EXPERIMENTS_REPORT.md` para detalhes dos experimentos

---

**Última atualização**: 2025-11-17  
**Status**: Em desenvolvimento  
**Versão**: 1.0.0-dev

Estrutura exigida pelo enunciado já mapeada aqui.

---

Para detalhes e metodologia dos experimentos, veja `docs/ARCHITECTURE.md`.
