# Resource Monitor 📊

Sistema completo de monitoramento de recursos do sistema Linux com interface TUI interativa, suporte a cgroups v2, namespaces e experimentos de controle de recursos.

## ✨ Funcionalidades

### 🎮 Interface TUI Interativa
- **Menu principal com navegação intuitiva** (setas, números, teclas)
- **4 módulos principais**:
  1. **Resource Monitor**: Coleta métricas de CPU, memória, I/O e rede
  2. **Namespace Analyzer**: Analisa e compara namespaces de processos
  3. **Cgroup Manager**: Cria e gerencia cgroups v2
  4. **Experimentos**: 5 experimentos prontos para executar

### 📈 Monitoramento em Tempo Real
- **CPU**: Uso percentual, tempo de usuário/sistema, idle
- **Memória**: Total, usado, disponível, cache, buffers, swap
- **I/O**: Leituras/escritas por segundo, operações completadas
- **Rede**: Taxa de RX/TX, pacotes, erros
- **Exportação em CSV** para análise posterior

### 🎯 Controle de Recursos com Cgroups v2
- Limitação de CPU (quota/period e weight)
- Limitação de memória (max, high watermark)
- Limitação de I/O (read/write bps)
- Estatísticas de uso por cgroup
- Interface interativa para criar, mover PIDs e configurar limites

### 🔒 Isolamento com Namespaces
- **PID**: Isolamento de processos
- **NET**: Stack de rede isolada
- **MNT**: Pontos de montagem isolados
- **UTS**: Hostname isolado
- **IPC**: Inter-process communication isolado
- **USER**: Mapeamento de usuários
- **Análise comparativa** entre processos

### 🧪 Experimentos Automatizados
1. **Overhead de Monitoramento**: Mede o impacto do monitoramento no desempenho
2. **CPU Throttling**: Testa limitação de CPU com cgroups (50% por 30s)
3. **Memory Limit**: Testa comportamento sob pressão de memória (256MB)
4. **I/O Limit**: Testa limitação de I/O em disco (50 MB/s)
5. **Namespace Isolation**: Verifica isolamento de namespaces (PID)
- Todos salvam resultados em CSV em `output/experiments/`

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

## 🚀 Uso

### Menu Interativo (TUI) - Recomendado
```bash
# Para funcionalidade completa (opções 2, 3 e 4 exigem root)
sudo ./bin/monitor

# Ou sem privilégios (apenas opção 1 - Resource Monitor)
./bin/monitor
```

**Navegação**:
- **Setas ↑↓**: Navegar entre opções
- **Números 1-5**: Selecionar diretamente
- **ENTER**: Confirmar seleção
- **Q**: Voltar/Sair

#### Opção 1: Resource Monitor
Coleta métricas detalhadas de um processo:
1. Digite o PID do processo
2. Define intervalo de coleta (ms)
3. Define número de amostras
4. Resultado salvo em `output/monitor.csv`

#### Opção 2: Namespace Analyzer
Analisa namespaces Linux:
- **Listar namespaces de um PID**: Mostra todos os 7 tipos
- **Comparar dois PIDs**: Verifica se compartilham namespaces
- **Mapear processos por tipo**: Lista todos os processos com namespace específico
- **Overhead de criação**: Mede tempo de criação de namespaces
- **Relatório global**: Estatísticas do sistema

⚠️ **Algumas operações exigem sudo** - veja [docs/PERMISSIONS.md](docs/PERMISSIONS.md)

#### Opção 3: Cgroup Manager
Gerencia cgroups v2:
- **Criar cgroup**: mkdir em /sys/fs/cgroup/<nome>
- **Ler métricas**: cpu.stat, memory.current, io.stat
- **Mover PID**: Adiciona processo a um cgroup
- **Set CPU quota**: Limita uso de CPU (quota/period)
- **Set Mem max**: Define limite de memória em bytes

⚠️ **Requer sudo** - veja [docs/PERMISSIONS.md](docs/PERMISSIONS.md)

#### Opção 4: Experimentos
Executa 5 experimentos automatizados:
1. **Overhead**: Mede impacto do profiler (~0.5-2%)
2. **CPU Throttling**: Limita processo a 50% CPU por 30s
3. **Memory Limit**: Força OOM com limite de 256MB
4. **I/O Limit**: Throttle de disco a 50 MB/s por 30s
5. **Namespace Isolation**: Cria e verifica isolamento PID

Todos salvam CSVs em `output/experiments/`

⚠️ **Requer sudo** - veja [docs/PERMISSIONS.md](docs/PERMISSIONS.md)

---

### Uso via Linha de Comando (CLI)

#### Gerenciamento de Cgroups
```bash
# Criar novo cgroup
sudo ./bin/cgroup_manager create my_group

# Configurar CPU (50% = 50000/100000)
sudo ./bin/cgroup_manager set-cpu my_group 50000 100000

# Configurar memória (512MB em bytes)
sudo ./bin/cgroup_manager set-mem my_group 536870912

# Mover processo para cgroup
sudo ./bin/cgroup_manager move /sys/fs/cgroup/my_group 1234

# Ler métricas
sudo ./bin/cgroup_manager read /sys/fs/cgroup/my_group
```

## 📁 Estrutura do Projeto

```
resource-monitor/
├── bin/                   # Binários compilados
│   ├── monitor            # Programa principal com TUI
│   └── cgroup_manager     # CLI gerenciador de cgroups
├── src/                   # Código fonte C
│   ├── monitor_tui.c      # Interface TUI interativa (menu ncurses)
│   ├── resource_profiler.c # Profiler integrado de recursos
│   ├── cpu_monitor.c      # Coleta de métricas de CPU
│   ├── memory_monitor.c   # Coleta de métricas de memória
│   ├── io_monitor.c       # Coleta de métricas de I/O
│   ├── network_monitor.c  # Coleta de métricas de rede
│   ├── namespace_analyzer.c # API de análise de namespaces
│   ├── cgroup_manager.c   # API de gerenciamento de cgroups
│   ├── cgroup_v2.c        # Implementação cgroups v2
│   ├── process_monitor.c  # Monitor de processos
│   ├── experiments.c      # Framework de experimentos
│   ├── experiment_overhead.c
│   ├── experiment_cpu_throttling.c
│   ├── experiment_memory_limit.c
│   ├── experiment_io_limit.c
│   └── utils.c            # Funções utilitárias
├── include/               # Headers públicos
│   ├── resource_profiler.h
│   ├── monitors.h
│   ├── namespace.h
│   ├── cgroup.h
│   ├── experiments.h
│   └── utils.h
├── docs/                  # Documentação adicional
│   └── PERMISSIONS.md     # Guia de permissões e sudo
├── output/                # Dados gerados
│   ├── experiments/       # CSVs dos experimentos
│   └── monitor.csv        # Dados do profiler
├── tests/                 # Testes unitários
│   ├── test_cpu.c
│   ├── test_memory.c
│   ├── test_io.c
│   └── test_namespace.c
├── Makefile              # Sistema de build GNU Make
└── readme.md             # Esta documentação
```

## 💡 Exemplos de Uso

### Exemplo 1: Monitorar Firefox
```bash
# 1. Encontrar PID do Firefox
ps aux | grep firefox
# firefox   12345  2.5  8.3 ...

# 2. Executar monitor
sudo ./bin/monitor

# 3. Opção 1 (Resource Monitor)
# Digite: PID=12345, intervalo=1000ms, amostras=60

# 4. Resultado em output/monitor.csv
```

---

### Exemplo 2: Limitar CPU de um Processo
```bash
# 1. Executar monitor com sudo
sudo ./bin/monitor

# 2. Opção 3 (Cgroup Manager) > Criar cgroup
# Nome: firefox_limited

# 3. Opção 3 > Set CPU quota
# Nome: firefox_limited
# Quota: 50000
# Period: 100000
# (50% CPU)

# 4. Opção 3 > Mover PID
# Path: firefox_limited
# PID: 12345

# 5. Firefox agora está limitado a 50% de uma CPU!
```

---

### Exemplo 3: Executar Todos os Experimentos
```bash
# 1. Executar com sudo
sudo ./bin/monitor

# 2. Opção 4 (Experimentos)
# Selecionar: 1 (Overhead) - aguardar
# Selecionar: 2 (CPU Throttling) - aguardar ~30s
# Selecionar: 3 (Memory Limit) - aguardar
# Selecionar: 4 (I/O Limit) - aguardar ~30s
# Selecionar: 5 (Namespace Isolation) - aguardar

# 3. Verificar resultados
ls -lh output/experiments/
# overhead.csv
# cpu_throttling.csv
# memory_limit.csv
# io_limit.csv
# namespace.csv
```

---

### Exemplo 4: Comparar Namespaces de Containers
```bash
# 1. Iniciar dois containers Docker
docker run -d --name container1 nginx
docker run -d --name container2 nginx

# 2. Obter PIDs
PID1=$(docker inspect -f '{{.State.Pid}}' container1)
PID2=$(docker inspect -f '{{.State.Pid}}' container2)

# 3. Executar monitor
sudo ./bin/monitor

# 4. Opção 2 (Namespace Analyzer) > Comparar dois PIDs
# PID1: <valor>
# PID2: <valor>

# Resultado mostra quais namespaces são compartilhados
```

## 🔧 Troubleshooting

### ❌ Erro: "Permission denied" ao criar cgroup
**Sintoma**:
```
mkdir: Permission denied
create(leandro) => -1
```

**Solução**:
```bash
# Executar com sudo
sudo ./bin/monitor
```
📖 Veja explicação completa em [docs/PERMISSIONS.md](docs/PERMISSIONS.md)

---

### ❌ Erro: "cgroup: (unavailable)"
**Sintoma**:
```
cgroup: (unavailable)
```

**Causas possíveis**:
1. Cgroups v2 não habilitado
2. Processo não está em nenhum cgroup

**Solução**:
```bash
# Verificar se cgroups v2 está montado
mount | grep cgroup2

# Montar se necessário
sudo mount -t cgroup2 none /sys/fs/cgroup
```

---

### ❌ Erro: Namespaces aparecem "(unavailable)"
**Sintoma**:
```
Namespaces for pid 10:
  mnt: (unavailable)
  pid: (unavailable)
```

**Causa**: PID pertence a outro usuário ou processo do sistema

**Solução**:
```bash
# Use sudo para acessar qualquer PID
sudo ./bin/monitor

# Ou use seu próprio PID
ps aux | grep $USER  # encontrar seus PIDs
```

---

### ❌ Erro: "Failed to move pid into cgroup"
**Sintoma**:
```
Failed to move pid 10 into cgroup 10 (no writable cgroup.procs/tasks found)
```

**Causas**:
1. Cgroup não existe
2. Sem permissão de escrita

**Solução**:
```bash
# 1. Criar o cgroup primeiro (com sudo)
sudo ./bin/monitor  # opção 3 > criar cgroup

# 2. Depois mover o PID
# opção 3 > mover PID
```

---

### ⚠️ Caracteres estranhos no terminal
**Sintoma**: Setas aparecem como `�~F~Q�~F~S`

**Solução**:
```bash
# Configurar UTF-8
export LANG=en_US.UTF-8
export LC_ALL=en_US.UTF-8

# Usar terminal moderno (GNOME Terminal, Konsole, etc.)
```

## 🛠️ Desenvolvimento

### Adicionar Novo Experimento
```c
// 1. Criar src/experiment_my_test.c
#include "../include/experiments.h"

typedef struct {
    double metric1;
    double metric2;
    bool success;
} MyTestResult;

int experiment_my_test(MyTestResult *result, const char *output_file) {
    // Sua lógica aqui
    result->success = true;
    
    // Salvar CSV
    FILE *f = fopen(output_file, "w");
    fprintf(f, "metric1,metric2\n");
    fprintf(f, "%.2f,%.2f\n", result->metric1, result->metric2);
    fclose(f);
    
    return 0;
}
```

```c
// 2. Adicionar em include/experiments.h
typedef struct {
    double metric1;
    double metric2;
    bool success;
} MyTestResult;

int experiment_my_test(MyTestResult *result, const char *output_file);
```

```c
// 3. Integrar no menu TUI (src/monitor_tui.c)
case 6: /* Meu Teste */
    mvprintw(4, 2, "Executando meu experimento...");
    refresh();
    end_ncurses();
    mkdir("output/experiments", 0755);
    MyTestResult my_res = {0};
    experiment_my_test(&my_res, "output/experiments/my_test.csv");
    init_ncurses();
    mvprintw(9, 2, "Resultado: %.2f", my_res.metric1);
    break;
```

```makefile
# 4. Adicionar no Makefile
$(MONITOR_BIN): ... $(OBJ_DIR)/experiment_my_test.o
```

---

### Estrutura de Código Recomendada
- **Funções de leitura**: Retornar 0 em sucesso, -1 em erro
- **Validação de entrada**: Sempre verificar argumentos
- **Logs**: Usar printf() com prefixos `[OK]`, `[ERRO]`, `[INFO]`
- **Cleanup**: Sempre fechar arquivos e liberar memória
- **CSV headers**: Primeira linha com nomes das colunas

---

### Contribuir
1. Fork o repositório: `LeandroCasiragh1/resource-monitor`
2. Criar branch: `git checkout -b feature/minha-feature`
3. Implementar funcionalidade
4. Testar: `make clean && make && sudo ./bin/monitor`
5. Commit: `git commit -am 'feat: adiciona nova feature'`
6. Push: `git push origin feature/minha-feature`
7. Criar Pull Request no GitHub

**Padrão de commits**:
- `feat:` Nova funcionalidade
- `fix:` Correção de bug
- `docs:` Atualização de documentação
- `refactor:` Refatoração de código
- `test:` Adicionar testes
- `chore:` Tarefas de manutenção

## 📚 Referências

- [Linux Control Groups v2 Documentation](https://www.kernel.org/doc/html/latest/admin-guide/cgroup-v2.html)
- [Linux Namespaces man page](https://man7.org/linux/man-pages/man7/namespaces.7.html)
- [proc(5) - Process Information Pseudo-filesystem](https://man7.org/linux/man-pages/man5/proc.5.html)
- [ncurses Programming HOWTO](https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/)
- [Linux Performance Monitoring](https://www.brendangregg.com/linuxperf.html)

---

## 📄 Licença

MIT License - Veja LICENSE para detalhes

---

## 👤 Autor

**Leandro Casiraghini**
- GitHub: [@LeandroCasiragh1](https://github.com/LeandroCasiragh1)
- Repositório: [resource-monitor](https://github.com/LeandroCasiragh1/resource-monitor)

---

## 🎯 Status do Projeto

✅ **Funcionalidades Implementadas**:
- [x] Interface TUI completa com ncurses
- [x] Resource profiler integrado (CPU, mem, I/O, rede)
- [x] Namespace analyzer (listar, comparar, mapear, overhead, relatório)
- [x] Cgroup manager (criar, ler, mover, set-cpu, set-mem)
- [x] 5 experimentos automatizados
- [x] Exportação CSV
- [x] Validação de entrada e tratamento de erros
- [x] Documentação completa (README + PERMISSIONS.md)
- [x] Sistema de build com Makefile

📊 **Estatísticas**:
- **~3.500 linhas de código C**
- **15+ arquivos fonte**
- **2 binários** (monitor + cgroup_manager)
- **4 módulos principais**
- **5 experimentos**
- **7 tipos de namespace suportados**

---

**Última atualização**: Novembro 2025
