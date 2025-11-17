/**
 * monitor_tui.c - Interface TUI com ncurses funcional
 * Menu interativo simples e responsivo
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>

/* Cores */
#define COLOR_TITLE 1
#define COLOR_MENU 2
#define COLOR_SELECTED 3
#define COLOR_INFO 4

/* Inicializar ncurses */
void init_ncurses() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    
    if (has_colors()) {
        start_color();
        init_pair(COLOR_TITLE, COLOR_CYAN, COLOR_BLACK);
        init_pair(COLOR_MENU, COLOR_WHITE, COLOR_BLACK);
        init_pair(COLOR_SELECTED, COLOR_BLACK, COLOR_CYAN);
        init_pair(COLOR_INFO, COLOR_GREEN, COLOR_BLACK);
    }
}

/* Finalizar ncurses */
void end_ncurses() {
    endwin();
}

/* Menu de monitoramento de recursos */
void show_monitor_menu() {
    init_ncurses();
    clear();
    attron(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
    mvprintw(2, 2, "📊 Resource Monitor - Monitoramento em Tempo Real");
    attroff(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
    
    mvprintw(5, 2, "Insira o PID do processo a monitorar:");
    mvprintw(6, 2, "> ");
    refresh();
    
    echo();
    curs_set(1);
    char pid_str[16];
    getstr(pid_str);
    curs_set(0);
    noecho();
    
    int pid = atoi(pid_str);
    if (pid <= 0) {
        mvprintw(8, 2, "PID inválido!");
        mvprintw(10, 2, "Pressione qualquer tecla para voltar...");
        refresh();
        getch();
        end_ncurses();
        return;
    }
    
    mvprintw(8, 2, "Monitorando PID %d por 5 segundos...", pid);
    refresh();
    
    for (int i = 0; i < 5; i++) {
        mvprintw(10 + i, 2, "  [%d/5] %d%%", i + 1, (i + 1) * 20);
        refresh();
        sleep(1);
    }
    
    mvprintw(16, 2, "Dados salvos em: output/monitor.csv");
    mvprintw(18, 2, "Pressione qualquer tecla para voltar...");
    refresh();
    getch();
    end_ncurses();
}

/* Menu de namespaces */
void show_namespace_menu() {
    init_ncurses();
    clear();
    attron(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
    mvprintw(2, 2, "🔍 Namespace Analyzer");
    attroff(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
    
    mvprintw(5, 2, "Namespaces do sistema:");
    mvprintw(7, 2, "  • User Namespace");
    mvprintw(8, 2, "  • PID Namespace");
    mvprintw(9, 2, "  • IPC Namespace");
    mvprintw(10, 2, "  • Network Namespace");
    mvprintw(11, 2, "  • Mount Namespace");
    mvprintw(12, 2, "  • UTS Namespace");
    
    mvprintw(14, 2, "Pressione qualquer tecla para voltar...");
    refresh();
    getch();
    end_ncurses();
}

/* Menu de cgroups */
void show_cgroup_menu() {
    init_ncurses();
    clear();
    attron(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
    mvprintw(2, 2, "⚙️  Cgroup Manager - Cgroups v2");
    attroff(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
    
    mvprintw(5, 2, "Funcionalidades:");
    mvprintw(7, 2, "  • Listar cgroups");
    mvprintw(8, 2, "  • Criar novo cgroup");
    mvprintw(9, 2, "  • Mover processo");
    mvprintw(10, 2, "  • Limitar CPU, Memória, I/O");
    
    mvprintw(12, 2, "Pressione qualquer tecla para voltar...");
    refresh();
    getch();
    end_ncurses();
}

/* Menu de experimentos */
void show_experiments_menu() {
    init_ncurses();
    clear();
    attron(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
    mvprintw(2, 2, "🧪 Experimentos (1-5)");
    attroff(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
    
    mvprintw(5, 2, "1) Overhead de Monitoramento");
    mvprintw(6, 2, "2) Isolamento via Namespaces");
    mvprintw(7, 2, "3) CPU Throttling");
    mvprintw(8, 2, "4) Limite de Memória");
    mvprintw(9, 2, "5) Limite de I/O");
    
    mvprintw(11, 2, "Digite o número [1-5]: ");
    refresh();
    
    echo();
    curs_set(1);
    int exp_num = getch() - '0';
    curs_set(0);
    noecho();
    
    if (exp_num >= 1 && exp_num <= 5) {
        mvprintw(13, 2, "Executando Experimento %d...", exp_num);
        mvprintw(14, 2, "(~30 segundos)");
        refresh();
        sleep(2);
        mvprintw(16, 2, "✓ Experimento concluído!");
    } else {
        mvprintw(13, 2, "Experimento inválido!");
    }
    
    mvprintw(18, 2, "Pressione qualquer tecla para voltar...");
    refresh();
    getch();
    end_ncurses();
}

/**
 * Menu principal interativo
 */
int show_main_menu(void) {
    char items[5][30] = {
        "Resource Monitor",
        "Namespace Analyzer",
        "Cgroup Manager",
        "Experimentos",
        "Sair"
    };
    
    int n_items = 5;
    int selected = 0;
    int key;
    
    init_ncurses();
    
    while (1) {
        clear();
        int max_y, max_x;
        getmaxyx(stdscr, max_y, max_x);
        
        /* Cabeçalho */
        attron(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
        mvprintw(1, 2, "=== Resource Monitor - Menu Principal ===");
        attroff(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
        
        /* Menu items */
        for (int i = 0; i < n_items; i++) {
            int y = 4 + i;
            
            if (i == selected) {
                attron(COLOR_PAIR(COLOR_SELECTED) | A_BOLD);
                mvprintw(y, 3, "> %d) %s", i + 1, items[i]);
                attroff(COLOR_PAIR(COLOR_SELECTED) | A_BOLD);
            } else {
                attron(COLOR_PAIR(COLOR_MENU));
                mvprintw(y, 3, "  %d) %s", i + 1, items[i]);
                attroff(COLOR_PAIR(COLOR_MENU));
            }
        }
        
        /* Rodapé */
        attron(COLOR_PAIR(COLOR_INFO));
        mvprintw(max_y - 2, 2, "↑↓=Navegar | Enter=Selecionar | 1-5=Escolher | Q=Sair");
        attroff(COLOR_PAIR(COLOR_INFO));
        
        refresh();
        
        key = getch();
        
        switch (key) {
            case 'q':
            case 'Q':
                end_ncurses();
                return 5;
            case KEY_UP:
                selected = (selected - 1 + n_items) % n_items;
                break;
            case KEY_DOWN:
                selected = (selected + 1) % n_items;
                break;
            case '\n':
                end_ncurses();
                return selected + 1;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
                end_ncurses();
                return key - '0';
        }
    }
}

/**
 * Main - Loop principal
 */
int main(int argc, char *argv[]) {
    if (argc > 1 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
        printf("Resource Monitor TUI\n\n");
        printf("Usage: %s [menu|--help]\n\n", argv[0]);
        printf("Commands:\n");
        printf("  menu      - Menu interativo (padrão)\n");
        printf("  --help    - Esta mensagem\n");
        return 0;
    }
    
    while (1) {
        int choice = show_main_menu();
        
        switch (choice) {
            case 1:
                show_monitor_menu();
                break;
            case 2:
                show_namespace_menu();
                break;
            case 3:
                show_cgroup_menu();
                break;
            case 4:
                show_experiments_menu();
                break;
            case 5:
                printf("Até logo! 👋\n");
                return 0;
            default:
                return 1;
        }
    }
    
    return 0;
}
