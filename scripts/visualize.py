#!/usr/bin/env python3
"""
Script auxiliar para gerar visualizações a partir de CSV/JSON de saída.
Uso: `python3 visualize.py data.csv` (placeholder)
"""
import sys

def main():
    if len(sys.argv) < 2:
        print("Usage: visualize.py <data-file>")
        return
    print("Visualização placeholder para:", sys.argv[1])

if __name__ == '__main__':
    main()
