# Experiments Template

Use this template to document experiments required by the assignment. Copy one file per experiment under `docs/experiments/` (e.g. `docs/experiments/experiment1-overhead-profiler.md`).

1. Title

2. Objective

3. Hypothesis

4. Environment
- Kernel version, distro, hardware, whether test runs used WSL/VM
- CPU, RAM, disk type
- Whether root privileges were used

5. Methodology (step-by-step)
- Workload description (commands / scripts)
- Measurement procedure (what was measured, sampling interval)
- Number of repetitions and randomization

6. Metrics
- Execution time
- CPU usage (%), CPU steal
- Memory usage (RSS, VSZ), swap
- I/O throughput (B/s)
- cgroup-specific metrics (throttling, memory.failcnt)

7. Results (tables + graphs)
- Provide CSV attachments and a brief summary

8. Analysis
- Statistical treatment, discussion of variance

9. Conclusion

10. Raw Data
