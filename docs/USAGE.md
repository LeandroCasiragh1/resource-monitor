# Usage Guide

Build and run instructions. This project is designed for Linux (WSL or native). Some operations require root.

Prerequisites
- GCC (or clang) and `make`.
- `sudo` for cgroup and namespace experiments.
- Optional: `gh` (GitHub CLI) for PR automation.

Build (recommended: WSL or Linux)

1. Open a shell in the project root.
2. Run:

```bash
make
```

This will produce binaries in `bin/` such as `resource-monitor` and `resource-profiler`.

Running the tools

- Resource profiler (examples):
  - `./bin/resource-profiler <PID> [interval_ms] [samples] [out.csv]`

- Namespace analyzer:
  - `./bin/resource-monitor list <PID>`
  - `./bin/resource-monitor compare <PID1> <PID2>`
  - `./bin/resource-monitor map pid`

- Cgroup manager (may need root):
  - `sudo ./bin/resource-monitor create my-experiment`
  - `sudo ./bin/resource-monitor move my-experiment 12345`
  - `./bin/resource-monitor read my-experiment`

Notes
- For accurate kernel-level metrics and experiments, run the tools on native Linux or in WSL2 with a proper kernel. Some features (cgroup v2) depend on the distro configuration.
- Use the `docs/EXPERIMENTS.md` template to document each experiment and attach raw CSV outputs.
