# Guia de Permissões - Resource Monitor

## 🔐 Por que preciso de sudo?

As opções **2 (Namespace Analyzer)** e **3 (Cgroup Manager)** interagem diretamente com recursos do kernel Linux que **exigem privilégios de root** para operações de modificação.

## ⚠️ Erros Comuns

### 1. Permission Denied ao criar cgroups
```
mkdir: Permission denied
create(leandro) => -1
```

**Causa**: Criar diretórios em `/sys/fs/cgroup/` requer root.

**Solução**:
```bash
sudo ./bin/monitor
```

### 2. Falha ao mover PID para cgroup
```
Failed to move pid 10 into cgroup 10 (no writable cgroup.procs/tasks found)
move(10,10) => -1
```

**Causa**: Escrever em `cgroup.procs` requer root e o cgroup deve existir.

**Solução**:
```bash
# Execute como root
sudo ./bin/monitor

# Ou crie o cgroup manualmente primeiro
sudo mkdir /sys/fs/cgroup/mygroup
```

### 3. Namespace unavailable
```
Namespaces for pid 10:
  mnt: (unavailable)
  pid: (unavailable)
```

**Causa**: Acesso a `/proc/<pid>/ns/*` pode ser restrito dependendo do processo.

**Solução**: 
- Use seu próprio PID (`$$` em bash) ou PIDs de processos seus
- Execute com sudo para acessar processos de outros usuários

## ✅ Modo de Uso Recomendado

### Para Testes Básicos (sem sudo)
```bash
# Opção 1: Resource Monitor - funciona sem root
./bin/monitor

# Selecione opção 1 e monitore seu próprio PID
```

### Para Funcionalidade Completa (com sudo)
```bash
# Execute com privilégios de root
sudo ./bin/monitor

# Agora todas as opções funcionam:
# 1. Resource Monitor
# 2. Namespace Analyzer (leitura e criação)
# 3. Cgroup Manager (criar, modificar, mover PIDs)
# 4. Experimentos (cgroups + namespaces)
```

## 📋 Resumo de Permissões

| Operação | Requer sudo? | Motivo |
|----------|--------------|--------|
| **Resource Monitor** (opção 1) | ❌ Não* | Lê apenas `/proc/<pid>/` do próprio usuário |
| **Namespace Analyzer - Listar** | ⚠️ Parcial | Só para PIDs próprios ou com sudo |
| **Namespace Analyzer - Criar** | ✅ Sim | `unshare()` syscall requer CAP_SYS_ADMIN |
| **Cgroup Manager - Ler** | ❌ Não | Leitura de `/sys/fs/cgroup/` é pública |
| **Cgroup Manager - Criar/Mover** | ✅ Sim | Escrever em `/sys/fs/cgroup/` requer root |
| **Experimentos** | ✅ Sim | Combinam cgroups + namespaces |

\* *Pode exigir sudo para monitorar PIDs de outros usuários*

## 🛡️ Segurança

### Por que Linux exige root para estas operações?

1. **Cgroups**: Controlam recursos do sistema (CPU, memória, I/O). Sem root, um usuário poderia:
   - Limitar recursos de outros processos
   - Escapar de limites impostos pelo sistema

2. **Namespaces**: Isolam processos. Sem root, um usuário poderia:
   - Criar ambientes isolados maliciosos
   - Bypassar restrições de rede/mount

3. **Acesso /proc**: Ler dados de processos de outros usuários expõe:
   - Argumentos de linha de comando (senhas?)
   - Arquivos abertos
   - Memória mapeada

## 🔧 Alternativas (Desenvolvimento)

Se você **não quer usar sudo** para testes:

### 1. Use containers
```bash
# Docker já roda com cgroups configurados
docker run -it --rm ubuntu bash
cd /sys/fs/cgroup
```

### 2. Configure cgroups delegados (cgroup v2)
```bash
# Uma vez como root, delegar controle ao seu usuário
sudo mkdir -p /sys/fs/cgroup/user.slice/user-$(id -u).slice/user@$(id -u).service/app.slice
sudo chown -R $USER:$USER /sys/fs/cgroup/user.slice/user-$(id -u).slice/user@$(id -u).service/app.slice
```

### 3. Use capabilidades específicas
```bash
# Em vez de sudo completo, dê apenas CAP_SYS_ADMIN
sudo setcap cap_sys_admin+ep ./bin/monitor
./bin/monitor  # agora funciona sem sudo
```

⚠️ **Cuidado**: `setcap` dá poderes permanentes ao binário!

## 📚 Referências

- [Linux Cgroups Documentation](https://www.kernel.org/doc/Documentation/cgroup-v2.txt)
- [Namespaces man page](https://man7.org/linux/man-pages/man7/namespaces.7.html)
- [Linux Capabilities](https://man7.org/linux/man-pages/man7/capabilities.7.html)
