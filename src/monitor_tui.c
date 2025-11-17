/**
 * monitor_tui.c - Interface TUI (Text User Interface) com ncurses
 * 
 * Implementa um menu interativo para:
 * - Resource Monitoring em tempo real
 * - Namespace Analysis
 * - Cgroup Management
 * - Experimentos e visualizações
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <ncurses.h>

#include "resource_profiler.h"
#include "namespace.h"
#include "cgroup.h"

/* Forward declarations */
int show_main_menu(void);
int show_monitor_menu(void);
int show_namespace_menu(void);
int show_cgroup_menu(void);
int show_experiments_menu(void);

/**
 * Main entry point para o programa
 */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <command> [options]\n", argv[0]);
        printf("\nCommands:\n");
        printf("  menu                 - Menu interativo (padrão)\n");
        printf("  tui <PID> [INT] [N]  - Monitoramento em tempo real\n");
        printf("  process <PID> ...    - Monitoramento com exportação\n");
        printf("  namespace <cmd>      - Análise de namespaces\n");
        printf("  cgroup <cmd>         - Gerenciamento de cgroups\n");
        printf("  experiment <N>       - Executar experimento N (1-5)\n");
        printf("  --help               - Mostrar esta mensagem\n");
        return 1;
    }

    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        printf("Resource Monitor - Monitoramento de Recursos Linux\n\n");
        printf("Usage: %s <command> [options]\n\n", argv[0]);
        
        printf("Commands:\n");
        printf("  menu                         Menu interativo com todas opções\n");
        printf("  tui <PID> [interval] [count] Modo TUI (tempo real)\n");
        printf("                               interval: ms entre amostras (padrão 1000)\n");
        printf("                               count: número de amostras (padrão 60)\n\n");
        
        printf("  process <PID> <INT> <COUNT> [FORMAT]\n");
        printf("                               Monitoramento com exportação\n");
        printf("                               FORMAT: csv, json (padrão csv)\n\n");
        
        printf("  namespace <subcommand> [args]\n");
        printf("                               list <PID>        - Listar namespaces\n");
        printf("                               compare <P1> <P2> - Comparar dois processos\n");
        printf("                               report            - Relatório do sistema\n\n");
        
        printf("  cgroup <subcommand> [args]\n");
        printf("                               list              - Listar cgroups\n");
        printf("                               create <NAME>     - Criar cgroup\n");
        printf("                               move <PID> <PATH> - Mover processo\n\n");
        
        printf("  experiment <N>               Executar experimento 1-5\n");
        printf("                               1: Overhead de monitoramento\n");
        printf("                               2: Isolamento via namespaces\n");
        printf("                               3: CPU throttling\n");
        printf("                               4: Limite de memória\n");
        printf("                               5: Limite de I/O\n\n");
        
        printf("Examples:\n");
        printf("  %s menu                     # Menu interativo\n", argv[0]);
        printf("  %s tui 1234                 # Monitorar PID 1234\n", argv[0]);
        printf("  %s process 1234 5 60 json   # Exportar para JSON\n", argv[0]);
        printf("  %s namespace list 1         # Ver namespaces do PID 1\n", argv[0]);
        printf("  %s experiment 1             # Rodar experimento 1\n\n", argv[0]);
        
        return 0;
    }

    if (strcmp(argv[1], "menu") == 0) {
        return show_main_menu();
    }

    if (strcmp(argv[1], "tui") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Error: tui requer PID\n");
            return 1;
        }
        pid_t pid = atoi(argv[2]);
        int interval = (argc > 3) ? atoi(argv[3]) : 1000;
        int count = (argc > 4) ? atoi(argv[4]) : 60;
        
        printf("Monitorando PID %d...\n", pid);
        printf("Intervalo: %dms, Amostras: %d\n", interval, count);
        printf("Pressione Ctrl+C para sair\n\n");
        
        // TODO: Implementar TUI real com ncurses
        sleep(2);
        return 0;
    }

    if (strcmp(argv[1], "process") == 0) {
        if (argc < 5) {
            fprintf(stderr, "Error: process requer <PID> <INTERVAL> <COUNT> [FORMAT]\n");
            return 1;
        }
        printf("Processando métricas...\n");
        // TODO: Implementar exportação
        return 0;
    }

    if (strcmp(argv[1], "namespace") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Error: namespace requer subcommand\n");
            return 1;
        }
        printf("Analisando namespaces: %s\n", argv[2]);
        // TODO: Implementar análise
        return 0;
    }

    if (strcmp(argv[1], "cgroup") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Error: cgroup requer subcommand\n");
            return 1;
        }
        printf("Gerenciando cgroups: %s\n", argv[2]);
        // TODO: Implementar gerenciamento
        return 0;
    }

    if (strcmp(argv[1], "experiment") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Error: experiment requer número 1-5\n");
            return 1;
        }
        int exp = atoi(argv[2]);
        printf("Executando experimento %d...\n", exp);
        // TODO: Implementar experimentos
        return 0;
    }

    fprintf(stderr, "Comando desconhecido: %s\n", argv[1]);
    printf("Use '%s --help' para ver opções\n", argv[0]);
    return 1;
}

/**
 * Menu principal interativo
 */
int show_main_menu(void) {
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(FALSE);

    int ch;
    int option = 0;

    while (1) {
        clear();
        
        printw("╔════════════════════════════════════════════════════════════════╗\n");
        printw("║         Resource Monitor - Menu Principal                      ║\n");
        printw("╚════════════════════════════════════════════════════════════════╝\n\n");

        printw("1) Resource Monitor (TUI em tempo real)\n");
        printw("2) Namespace Analyzer (análise de isolamento)\n");
        printw("3) Control Group Manager (gerenciamento de cgroups)\n");
        printw("4) Experimentos (1-5 + visualizações)\n");
        printw("5) Sair\n\n");

        printw("Escolha uma opção [1-5]: ");
        refresh();

        ch = getch();
        if (ch >= '1' && ch <= '5') {
            option = ch - '0';
            break;
        }
    }

    endwin();

    switch (option) {
        case 1:
            printf("Iniciando Resource Monitor...\n");
            // show_monitor_menu();
            break;
        case 2:
            printf("Iniciando Namespace Analyzer...\n");
            // show_namespace_menu();
            break;
        case 3:
            printf("Iniciando Cgroup Manager...\n");
            // show_cgroup_menu();
            break;
        case 4:
            printf("Iniciando Experimentos...\n");
            // show_experiments_menu();
            break;
        case 5:
            printf("Saindo...\n");
            return 0;
        default:
            return 1;
    }

    return 0;
}

/**
 * Menu de monitoramento de recursos
 */
int show_monitor_menu(void) {
    printf("Monitor Menu\n");
    // TODO: Implementar
    return 0;
}

/**
 * Menu de análise de namespaces
 */
int show_namespace_menu(void) {
    printf("Namespace Menu\n");
    // TODO: Implementar
    return 0;
}

/**
 * Menu de gerenciamento de cgroups
 */
int show_cgroup_menu(void) {
    printf("Cgroup Menu\n");
    // TODO: Implementar
    return 0;
}

/**
 * Menu de experimentos
 */
int show_experiments_menu(void) {
    printf("Experiments Menu\n");
    // TODO: Implementar
    return 0;
}
